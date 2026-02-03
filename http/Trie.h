#pragma once
#include <string>
#include <vector>
class Trie{
private:
    std::string pattern_;
    std::string part_;
    Trie * parent_;
    std::vector<Trie *>children_;
    bool isWild_;
    Trie * match_child(const std::string& part);
    std::vector<Trie *> match_children(const std::string& part);
public:
    Trie(const std::string &part="",Trie *parent=nullptr,bool isWild=false);
    ~Trie();
    bool is_wild() const;
    std::string part() const;
    std::string pattern() const;

    void set_pattern(const std::string&pattern);
    void insert(const std::string& pattern,const std::vector<std::string> &parts, int depth);
    Trie * search(const std::vector<std::string> &parts, int depth);
};