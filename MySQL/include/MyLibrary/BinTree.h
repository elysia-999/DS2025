#ifndef BINTREE_H
#define BINTREE_H

#include "BinNode.h"
#include <cstdlib>
#include <queue>

template <typename T>
class BinTree {
protected:
    int _size;
    BinNodePosi(T) _root;
    virtual int updateHeight(BinNodePosi(T) x);
    void updateHeightAbove(BinNodePosi(T) x);

public:
    BinTree() : _size(0), _root(NULL) {}
    ~BinTree() { if (0 < _size) remove(_root); }

    int size() const { return _size; }
    bool empty() const { return !_root; }
    BinNodePosi(T) root() const { return _root; }

    BinNodePosi(T) insertAsRoot(T const& e);
    BinNodePosi(T) insertAsLC(BinNodePosi(T) x, T const& e);
    BinNodePosi(T) insertAsRC(BinNodePosi(T) x, T const& e);
    BinNodePosi(T) attachAsLC(BinNodePosi(T) x, BinTree<T>* &S);
    BinNodePosi(T) attachAsRC(BinNodePosi(T) x, BinTree<T>* &S);
    int remove(BinNodePosi(T) x);
    BinTree<T>* secede(BinNodePosi(T) x);

    template <typename VST>
    void travLevel(VST& visit) { if (_root) _root->travLevel(visit); }

    template <typename VST>
    void travPre(VST& visit) { if (_root) _root->travPre(visit); }

    template <typename VST>
    void travIn(VST& visit) { if (_root) _root->travIn(visit); }

    template <typename VST>
    void travPost(VST& visit) { if (_root) _root->travPost(visit); }

    bool operator<(BinTree<T> const& t) { return _root && t._root && (_root->data < t._root->data); }
    bool operator==(BinTree<T> const& t) { return _root && t._root && (_root == t._root); }
};

template <typename T>
int BinTree<T>::updateHeight(BinNodePosi(T) x) {
    return x->height = 1 + std::max(stature(x->lChild), stature(x->rChild));
}

template <typename T>
void BinTree<T>::updateHeightAbove(BinNodePosi(T) x) {
    while (x) {
        updateHeight(x);
        x = x->parent;
    }
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRoot(T const& e) {
    _size = 1;
    return _root = new BinNode<T>(e);
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsLC(BinNodePosi(T) x, T const& e) {
    _size++;
    x->insertAsLC(e);
    updateHeightAbove(x);
    return x->lChild;
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRC(BinNodePosi(T) x, T const& e) {
    _size++;
    x->insertAsRC(e);
    updateHeightAbove(x);
    return x->rChild;
}

template <typename T>
static int removeAt(BinNodePosi(T) x) {
    if (!x) return 0;
    int n = 1 + removeAt(x->lChild) + removeAt(x->rChild);
    delete x;
    return n;
}

template <typename T>
int BinTree<T>::remove(BinNodePosi(T) x) {
    FromParentTo(*x) = NULL;
    updateHeightAbove(x->parent);
    int n = removeAt(x);
    _size -= n;
    return n;
}

#endif