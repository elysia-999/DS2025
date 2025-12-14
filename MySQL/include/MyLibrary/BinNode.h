#ifndef BINNODE_H
#define BINNODE_H

#include <cstdlib>
#include <cstddef>
#include <queue>  // 添加 queue 支持

#define BinNodePosi(T) BinNode<T>* 
#define stature(p) ((p) ? (p)->height : -1)
typedef enum { RB_RED, RB_BLACK } RBColor;
typedef int Rank;

template <typename T>
inline void release(T* p){ if(p) delete p; }

template <typename T> struct BinNode {
    T data;
    BinNodePosi(T) parent; 
    BinNodePosi(T) lChild; 
    BinNodePosi(T) rChild;
    int height;
    int npl;
    RBColor color;

    BinNode() : parent(NULL), lChild(NULL), rChild(NULL), height(0), npl(1), color(RB_RED) { }
    BinNode(T e, BinNodePosi(T) p = NULL, BinNodePosi(T) lc = NULL, BinNodePosi(T) rc = NULL,
            int h = 0, int l = 1, RBColor c = RB_RED)
        : data(e), parent(p), lChild(lc), rChild(rc), height(h), npl(l), color(c) { }

    int size();
    BinNodePosi(T) insertAsLC(T const&);
    BinNodePosi(T) insertAsRC(T const&);
    BinNodePosi(T) succ();
    template <typename VST> void travLevel(VST&);
    template <typename VST> void travPre(VST&);
    template <typename VST> void travIn(VST&);
    template <typename VST> void travPost(VST&);

    bool operator<(BinNode const& bn) { return data < bn.data; }
    bool operator==(BinNode const& bn) { return data == bn.data; }
};

#define IsRoot(x) (!((x).parent))
#define IsLChild(x) (!IsRoot(x) && (&(x) == (x).parent->lChild))
#define IsRChild(x) (!IsRoot(x) && (&(x) == (x).parent->rChild))
#define HasParent(x) (!IsRoot(x))
#define HasLChild(x) ((x).lChild)
#define HasRChild(x) ((x).rChild)
#define HasChild(x) (HasLChild(x) || HasRChild(x))
#define HasBothChild(x) (HasLChild(x) && HasRChild(x))
#define IsLeaf(x) (!HasChild(x))
#define sibling(p) (IsLChild(*(p)) ? (p)->parent->rChild : (p)->parent->lChild)
#define uncle(x) (IsLChild(*((x)->parent)) ? (x)->parent->parent->rChild : (x)->parent->parent->lChild)
#define FromParentTo(x) (IsRoot(x) ? _root : (IsLChild(x) ? (x).parent->lChild : (x).parent->rChild))

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsLC(T const& e) {
    return lChild = new BinNode(e, this);
}

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsRC(T const& e) {
    return rChild = new BinNode(e, this);
}

// 移除重复的遍历函数声明，只保留一个版本

template <typename T>
int BinNode<T>::size() {
    int s = 1;
    if (lChild) s += lChild->size();
    if (rChild) s += rChild->size();
    return s;
}

// 保留唯一的 succ 函数实现
template <typename T>
BinNodePosi(T) BinNode<T>::succ() {
    BinNodePosi(T) s = this;
    if (rChild) {
        s = rChild;
        while (HasLChild(*s)) s = s->lChild;
    } else {
        while (IsRChild(*s)) s = s->parent;
        s = s->parent;
    }
    return s;
}

// 简化遍历实现
template <typename T> template <typename VST>
void BinNode<T>::travPre(VST& visit) {
    if (!this) return;
    visit(this->data);
    if (lChild) lChild->travPre(visit);
    if (rChild) rChild->travPre(visit);
}

template <typename T> template <typename VST>
void BinNode<T>::travIn(VST& visit) {
    if (!this) return;
    if (lChild) lChild->travIn(visit);
    visit(this->data);
    if (rChild) rChild->travIn(visit);
}

template <typename T> template <typename VST>
void BinNode<T>::travPost(VST& visit) {
    if (!this) return;
    if (lChild) lChild->travPost(visit);
    if (rChild) rChild->travPost(visit);
    visit(this->data);
}

template <typename T> template <typename VST>
void BinNode<T>::travLevel(VST& visit) {
    if (!this) return;
    std::queue<BinNodePosi(T)> Q;
    Q.push(this);
    while (!Q.empty()) {
        BinNodePosi(T) x = Q.front();
        Q.pop();
        visit(x->data);
        if (x->lChild) Q.push(x->lChild);
        if (x->rChild) Q.push(x->rChild);
    }
}

#endif