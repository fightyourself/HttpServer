#include "Trie.h"
Trie::Trie(const std::string &part,Trie *parent,bool isWild):part_(part),parent_(parent),isWild_(isWild){

}

Trie::~Trie(){

}

bool Trie::is_wild()const{
    return isWild_;
}

std::string Trie::pattern() const{
    return pattern_;
}

std::string Trie::part() const{
    return part_;
}


Trie* Trie::match_child(const std::string& part){
    for(auto *child :children_){
        if(child->is_wild() || child->part()== part){
            return child;
        }
    }
    return nullptr;
}

std::vector<Trie *> Trie::match_children(const std::string &part){
    std::vector<Trie *> res;
    for(auto *child :children_){
        if(child->is_wild() || child->part()== part){
            res.push_back(child);
        }
    }
    return res;
}


void Trie::set_pattern(const std::string &pattern){
    pattern_ = pattern;
}

void Trie::insert(const std::string &pattern,const std::vector<std::string> &parts, int depth){
    if(depth==parts.size()){
        this->set_pattern(pattern);
        return;
    }
    std::string part = parts[depth];
    Trie * child = this->match_child(part);
    if(child==nullptr){
        child = new Trie(part,this,(part[0]=='*'||part[0]==':'));
        this->children_.push_back(child);
    }
    child->insert(pattern,parts,depth+1);
}

Trie * Trie::search(const std::vector<std::string> &parts, int depth){
    if(depth==parts.size() || this->part_[0]=='*'){
        if(this->pattern_.empty()){
            return nullptr;
        }
        return this;
    }
    std::string part = parts[depth];
    std::vector<Trie *> children = match_children(part);
    for(auto *child:children){
        Trie * res = child->search(parts,depth+1);
        if(res!=nullptr) return res;
    }
    return nullptr;
}


