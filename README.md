# HTTP服务器

在先前写的Reactor服务器上封装http协议，实现一个简单的http服务器。

下面是一个简单的使用示例：

```c++
#include "HttpServer.h"
#include "HttpResponse.h"
#include "Connection.h"
int main(int argc, char *argv[]){
    if( argc!=3){
        printf("usage:./server ip port\n");
        return -1;
    }
    HttpServer server(argv[1],atoi(argv[2]));
    server.GET("/",[](HttpRequest *req,spConnection conn){
        HttpResponse resp;
        std::string html =
            "<!DOCTYPE html>\r\n"
            "<html>\r\n"
            "<head>\r\n"
            "    <title>Hello World</title>\r\n"
            "</head>\r\n"
            "<body>\r\n"
            "    <h1>Welcome to My Simple Page!</h1>\r\n"
            "    <p>This is a basic HTML page served via HTTP.</p>\r\n"
            "</body>\r\n"
            "</html>\r\n";
        resp.set_header("Content-Type","text/html");
        resp.set_body(html);
        conn->send(resp.to_string());
    });

    server.start();
}
```

#### 解析HTTP请求

http处理tcp读事件应该按一下方式进行：

1. 将tcp连接中的数据全部读取到解析器的缓冲区
2. 解析器尝试解析完整的http，如果解析到了完整的http请求则封装为请求并回调请求处理函数并，如果解析到了非完整的http请求则退出解析继续等待读事件，如果解析错误了则断开此连接

解析HTTP请求使用的方法是有限状态机，以解析一行为单位进行，如果读不到一行数据，继续等待读事件。当解析完成一整个http请求时才会修改parser里面的缓冲区。

优化：目前有读事件时都是先将接收缓冲区中的数据读到parser的buffer中，然后解析的时候每次从buffer中复制一行来进一步解析，这样会将数据复制两份影响性能。

#### 生成HTTP响应

根据http响应报文的格式编写一个HttpResponse类，并设置相应的函数，最关键的是to_string函数能将http报文转化为一个string，以提供给connection发送

#### 路由

为了同时支持静态路由和动态路由，这里使用trie树来进行路由匹配。web应用程序的url大多数都是以相同的前缀作为一个分组，比如：/api/a，/api/b，这两个url就是在/api这个分组下。这种特性正好与tried树的每个节点的子节点都有相同的前缀这一特性吻合。

要实现基本的路由算法，trie树要实现插入和查询算法。

**插入算法：**

给定一个pattern，将这个pattern插入到trie树中。有一个简单的递归的算法，给定当前要插入的深度和切分后的url的字符拆数组。

+ 如果深度等于url数组大小，则插入完成，结束递归，否则
+ 将当前深度的url部分字符串取出。
+ 在子节点中匹配一个孩子，如果匹配成功：
+ 递归地向子节点插入该url，否则：
+ 创建一个新的节点，并将其加入到当前节点的子节点数组，再递归地向子节点插入。

**查询算法：**

给定一个url，在trie树中查询是否有匹配这个url的pattern，如果有返回成功匹配到的节点，否则返回空指针。

+ 如果当前深度等于url数组大小或则当前节点为通配时，则匹配成功返回节点。否则：
+ 将当前深度的url部分字符串取出。
+ 在子节点中匹配所有孩子。
+ 递归的在匹配成功的孩子们中匹配。

**动态更新路由**

使用trie来实现路由，提供了插入算法和查询算法。假如我有一个后台，可以动态地添加路由和删除路由。现在有个问题，这三个算法都要访问trie树，会造成冲突，我可以添加一个锁解决这个问题，但是这又引申出另一个问题，用户每次有请求的时候都需要获取这个锁，太浪费性能，而且动态删除和插入有不是一件很频繁的事情，它们却也要获取锁，这种不太频繁的事件却要影响频繁的客户请求，有什么解决方案吗？

**方案一：读写锁 (Read-Write Lock)**

这是最直接、最经典的解决方案。

原理：允许多个**读**操作同时进行（共享锁）。保证只有一个**写**操作能执行，并且在写操作期间不允许任何读操作（独占锁）。

优点：
- 实现简单，`std::shared_mutex`。
- 能显著提高读多写少场景下的并发性能

缺点：

+ 如果写操作比较频繁或耗时较长，仍可能导致“读者饥饿”（大量读请求持续到达，写请求长时间等待）。
+ 读锁和写锁之间的切换仍然有一定的开销。

**方案二：Copy-on-Write (COW - 写时复制)**

利用不可变数据结构的思想，在修改时创建新版本的 trie。

原理：
- 维护一个指向当前 `trie_root` 的指针/引用。
- **读操作**：直接读取 `trie_root`，无需加锁，因为 `trie_root` 指向的数据结构是不可变的。
- 写操作：
  1. 获取一个**互斥写锁**（保护 `trie_root` 这个指针本身不被并发修改）。
  2. 从旧的 `trie_root` 复制一份新的 trie（实际是构建受影响路径的新节点，未受影响的部分共享旧节点以节省空间），并在这个新副本上进行插入或删除。
  3. 修改完成后，将 `trie_root` 指针原子性地指向新的 trie 根。
  4. 释放写锁。

优点：
- **读操作完全无锁**，性能极高，非常适合读多写少的场景。
- 读操作不会被写操作阻塞。

缺点：
- **写操作开销大**：每次写入都需要获取写锁、复制部分或全部结构（虽然有节点共享优化）、更新根指针。写操作会阻塞其他写操作。
- **内存开销**：在新旧 trie 同时存在的一小段时间内，内存中会有多余的节点。
- 实现相对复杂，需要构建支持共享节点的不可变 trie 结构。

**方案三：分段锁 (Segmented Locking)**

将大的 trie 分成多个段（例如，根据路径的第一个字符或前几个字符划分），每个段有自己的锁。

#### 增加工作线程池

当http服务器成功解析到http请求时，应该把http请求工作线程去处理，以防止业务阻塞了io线程导致无法接收其他用户的请求。

在httpServer中添加工作线程池，将路由、处理请求等任务交给工作线程。

#### 利用eventfd来通知有数据需要发送

在多reactor多线程模型中，收发数据的任务应该交给io线程去做，业务交给工作线程做。当我们在工作线程完成业务后，此时有数据需要发送，需要有一种机制通知io线程有数据需要发送。eventfd刚好完美符合我们的需求，evenfd是一个Linux维护的一个64位的计数器，更重要的时它是一个fd，只要是一个fd我们就可以使用epoll来进行监听。因此，我们可以这样设计：

+ 每个io线程对应的eventloop去监听一个发送事件的fd
+ 每当工作线程有数据需要发送的时候，将发送任务加入到connection所属的eventloop的发送队列，并对其fd写入数据。
+ 每当eventloop监听到fd有事件的时候，就将fd中的数据全部读出，并将发送队列中的数据全部发送出去。

#### 清除多余的连接

在http服务器中，我们需要将长时间空闲的tcp连接断开，以免占用服务器的资源。我们可以每一段时间检测所有tcp连接上次的请求时间是否超过设定的阈值，如果超过则断开该tcp连接。

马上能想到的一个方案可以另起一个线程，创建一个简单的定时任务，每隔一段时间进行检测，不过这样的话，我们会在多个线程同时读写connection的timestamp，这样就会涉及并发控制。

另一个方案是使用timerfd，这个timerfd和eventfd很像，有一个函数可以定时向timerfd中写入数据，这样就为使用epoll完成定时任务提供了便利。以下是使用timerfd的设计：

+ 在eventloop中每隔一定的时间向timerfd中写入数据
+ timerfd的处理函数中记录此刻的时间，然后查看所有的connection上次请求的时间，如果时间差超过阈值，则断开连接。

#### 并发控制

实际开发过程中，一般时io线程之间或是io线程与工作线程之间需要操作同一个数据。可以加锁解决，但为了避免加锁的负担，本项目采用的是通过`EventLoop::queue_in_loop`函数将不同线程中的操作放到同一个线程中串行执行。原理如下：

+ 每个`EventLoop`都有需要串行执行的任务队列
+ 每当其他线程有冲突的操作时，就会加锁的把操作加入到对应线程的任务队列

对同一个数据的操作都在一个线程中，就不会出现并发问题。

#### 性能测试

使用一个简单的脚本进行测试

```bash
for c in 50 100 200 400 800; do
  echo "=== c=$c ==="
  wrk -t4 -c$c -d20s http://127.0.0.1:8080/ping
done
```

结果：

在我笔记本上的一个ubuntu虚拟机上跑的，返回首页html的请求。

```
=== c=50 ===
Running 20s test @ http://127.0.0.1:5005/
  4 threads and 50 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.03ms  708.37us  16.75ms   89.29%
    Req/Sec    11.83k     2.14k   24.96k    64.79%
  942528 requests in 20.10s, 234.60MB read
Requests/sec:  46895.64
Transfer/sec:     11.67MB
=== c=100 ===
Running 20s test @ http://127.0.0.1:5005/
  4 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.88ms    1.13ms  23.95ms   84.92%
    Req/Sec    13.40k     2.28k   18.62k    68.25%
  1067058 requests in 20.02s, 265.60MB read
Requests/sec:  53311.39
Transfer/sec:     13.27MB
=== c=200 ===
Running 20s test @ http://127.0.0.1:5005/
  4 threads and 200 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.10ms    1.56ms  23.89ms   81.47%
    Req/Sec    15.98k     2.54k   21.11k    65.12%
  1274123 requests in 20.04s, 317.14MB read
Requests/sec:  63586.73
Transfer/sec:     15.83MB
=== c=400 ===
Running 20s test @ http://127.0.0.1:5005/
  4 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     5.58ms    2.22ms  39.34ms   81.05%
    Req/Sec    17.40k     2.36k   22.74k    69.00%
  1388474 requests in 20.07s, 345.60MB read
Requests/sec:  69182.82
Transfer/sec:     17.22MB
=== c=800 ===
Running 20s test @ http://127.0.0.1:5005/
  4 threads and 800 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    11.75ms    4.14ms  57.56ms   78.68%
    Req/Sec    16.59k     2.90k   39.84k    68.01%
  1321016 requests in 20.10s, 328.81MB read
Requests/sec:  65731.82
Transfer/sec:     16.36MB
```

