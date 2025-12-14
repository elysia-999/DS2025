#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdlib>      // rand
#include <cstring>      // strlen/strcpy
#include "Vector.h"     // 复用你已有的 Vector 模板

template <typename K, typename V>
class Skiplist {
private:
    /* ---------- 内部节点 ---------- */
    struct Node {
        K key;
        V val;
        Vector<Node*> next;   // 多层前向指针
        Node(const K& k, const V& v, int lvl)
            : key(k), val(v), next(lvl, nullptr) {}
    };

    typedef Node* NodePtr;

    static constexpr int MAX_LEVEL = 16;
    static constexpr double P = 0.5;        // 晋升概率
    int level;                              // 当前最大层数（0 起）
    NodePtr header;                         // 头节点，不存数据
    int _size;

    /* 随机生成层数 [1, MAX_LEVEL] */
    int randomLevel() {
        int lvl = 1;
        while ((rand() / double(RAND_MAX)) < P && lvl < MAX_LEVEL) ++lvl;
        return lvl;
    }

public:
    /* ---------- 构造 / 析构 ---------- */
    Skiplist() : level(1), _size(0) {
        header = new Node(K(), V(), MAX_LEVEL);
    }
    ~Skiplist() {
        NodePtr p = header;
        while (p) {
            NodePtr nxt = p->next[0];
            delete p;
            p = nxt;
        }
    }

    /* ---------- 基本接口 ---------- */
    int size() const { return _size; }
    bool empty() const { return _size == 0; }

    /* 插入：若 key 已存在则覆盖 val */
    void insert(const K& key, const V& val) {
        Vector<NodePtr> prev(MAX_LEVEL, header);
        NodePtr p = header;

        /* 1. 逐层搜索前驱 */
        for (int i = level - 1; i >= 0; --i) {
            while (p->next[i] && p->next[i]->key < key)
                p = p->next[i];
            prev[i] = p;
        }

        /* 2. 是否已存在 */
        p = p->next[0];
        if (p && p->key == key) { p->val = val; return; }

        /* 3. 生成新节点 */
        int lvl = randomLevel();
        if (lvl > level) {
            for (int i = level; i < lvl; ++i) prev[i] = header;
            level = lvl;
        }
        NodePtr n = new Node(key, val, lvl);

        /* 4. 重链 */
        for (int i = 0; i < lvl; ++i) {
            n->next[i] = prev[i]->next[i];
            prev[i]->next[i] = n;
        }
        ++_size;
    }

    /* 查找：返回 val 指针，失败 nullptr */
    V* get(const K& key) {
        NodePtr p = header;
        for (int i = level - 1; i >= 0; --i)
            while (p->next[i] && p->next[i]->key < key)
                p = p->next[i];
        p = p->next[0];
        if (p && p->key == key) return &p->val;
        return nullptr;
    }

    bool contains(const K& key) { return get(key) != nullptr; }

    /* 删除：成功返回 true，失败 false */
    bool remove(const K& key) {
        Vector<NodePtr> prev(MAX_LEVEL, header);
        NodePtr p = header;

        for (int i = level - 1; i >= 0; --i) {
            while (p->next[i] && p->next[i]->key < key)
                p = p->next[i];
            prev[i] = p;
        }
        p = p->next[0];
        if (!p || p->key != key) return false;

        /* 重链 + 释放 */
        for (int i = 0; i < level; ++i) {
            if (prev[i]->next[i] != p) break;
            prev[i]->next[i] = p->next[i];
        }
        delete p;
        --_size;

        /* 可能降低全局高度 */
        while (level > 1 && header->next[level - 1] == nullptr) --level;
        return true;
    }

    /* 调试：打印 level 0 链表 */
    void dump() {
        NodePtr p = header->next[0];
        while (p) {
            printf("[%c:%s] ", p->key, p->val);   // 假设 K=char, V=char*
            p = p->next[0];
        }
        printf("\n");
    }
};

#endif // SKIPLIST_H