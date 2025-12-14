#ifndef MYLIBRARY_VECTOR_H
#define MYLIBRARY_VECTOR_H

typedef int Rank;               // 秩
#define DEFAULT_CAPACITY 3      // 默认初始容量（最小容量）
#include <cstdlib>  // 添加 rand() 和 srand() 的头文件
#include <cstring>  // 添加 memset 和 memcpy 的头文件
#include <cstdio>   // 添加 FILE 相关操作的头文件
#include <algorithm> // 添加 swap 等算法
#include <ctime>    // 添加时间函数

template <typename T>
class Vector {                  // 向量模板类
protected:
    Rank _size;                 // 当前规模
    int  _capacity;             // 当前容量
    T*   _elem;                 // 数据区首地址

    /* 内部工具函数 */
    void copyFrom(T const* A, Rank lo, Rank hi); // 以数组区间 A[lo, hi) 为蓝本复制向量
    void expand();              // 空间不足时扩容
    void shrink();              // 装载因子过小时压缩
    bool bubble(Rank lo, Rank hi);      // 一趟起泡扫描
    void merge(Rank lo, Rank mi, Rank hi); // 归并
    Rank partition(Rank lo, Rank hi);   // 快速划分

public:
    // 构造函数
    Vector(int c = DEFAULT_CAPACITY, int s = 0, T v = T())
    { 
        _elem = new T[_capacity = (c > DEFAULT_CAPACITY ? c : DEFAULT_CAPACITY)]; 
        _size = 0;
        for (int i = 0; i < s; i++) {
            _elem[i] = v;
            _size++;
        }
    }
    
    Vector(T const* A, Rank lo, Rank hi) { copyFrom(A, lo, hi); } //数组区间复制
    Vector(T const* A, Rank n) { copyFrom(A, 0, n); } //数组整体复制
    Vector(Vector<T> const& V, Rank lo, Rank hi) { copyFrom(V._elem, lo, hi); } //向量区间复制
    Vector(Vector<T> const& V) { copyFrom(V._elem, 0, V._size); } //向量整体复制

    /* 析构函数 */
    ~Vector() { if (_elem) delete[] _elem; }

    /* 只读接口 */
    Rank size() const { return _size; }
    bool empty() const { return !_size; }
    int  disordered() const;            // 判断向量是否已排序（返回逆序对数）
    Rank find(T const& e) const { return find(e, 0, _size); }
    Rank find(T const& e, Rank lo, Rank hi) const; // 无序区间查找
    Rank search(T const& e) const { return search(e, 0, _size); }
    Rank search(T const& e, Rank lo, Rank hi) const; // 有序区间查找

    /* 可写接口 */
    T& operator[](Rank r) { return _elem[r]; } // 非const版本
    const T& operator[](Rank r) const { return _elem[r]; } // const版本
    Vector<T>& operator=(Vector<T> const& V); // 重载赋值
    T    remove(Rank r);                // 删除秩为 r 的元素
    int  remove(Rank lo, Rank hi);      // 删除区间 [lo, hi)
    Rank insert(Rank r, T const& e);    // 在秩 r 处插入 e
    Rank insert(T const& e) { return insert(_size, e); } // 默认尾插
    void sort(Rank lo, Rank hi);        // 对区间 [lo, hi) 排序
    void sort() { sort(0, _size); }     // 整体排序
    void unsort(Rank lo, Rank hi);      // 将区间 [lo, hi) 随机置乱
    void unsort() { unsort(0, _size); }
    int  deduplicate();                 // 无序去重
    int  uniquify();                    // 有序去重

    /* 遍历 */
    void traverse(void (*)(T&));        // 函数指针遍历
    template <typename VST>
    void traverse(VST&);                // 函数对象遍历
    
    /* 排序算法接口 */
    void bubbleSort(Rank lo, Rank hi);
    void selectionSort(Rank lo, Rank hi);
    void mergeSort(Rank lo, Rank hi);
    void quickSort(Rank lo, Rank hi);
    void heapSort(Rank lo, Rank hi) { selectionSort(lo, hi); } // 简化为选择排序
    
    /* 获取内部指针 */
    T* data() const { return _elem; }
};

/* =====================  实现部分  ===================== */

template <typename T>
void Vector<T>::copyFrom(T const* A, Rank lo, Rank hi) {
    _elem = new T[_capacity = 2 * (hi - lo)];
    _size = 0;
    while (lo < hi) _elem[_size++] = A[lo++];
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T> const& V) {
    if (this == &V) return *this;
    if (_elem) delete[] _elem;
    copyFrom(V._elem, 0, V.size());
    return *this;
}

template <typename T>
void Vector<T>::expand() {
    if (_size < _capacity) return;
    if (_capacity < DEFAULT_CAPACITY) _capacity = DEFAULT_CAPACITY;
    T* oldElem = _elem;
    _elem = new T[_capacity <<= 1];
    for (int i = 0; i < _size; ++i) _elem[i] = oldElem[i];
    delete[] oldElem;
}

template <typename T>
void Vector<T>::shrink() {
    if (_capacity < (DEFAULT_CAPACITY << 1)) return;
    if (_size << 2 > _capacity) return;
    T* oldElem = _elem;
    _elem = new T[_capacity >>= 1];
    for (int i = 0; i < _size; ++i) _elem[i] = oldElem[i];
    delete[] oldElem;
}

template <typename T>
void Vector<T>::unsort(Rank lo, Rank hi) {
    T* V = _elem + lo;
    for (Rank i = hi - lo; i > 0; --i)
        std::swap(V[i - 1], V[rand() % i]);
}

template <typename T>
Rank Vector<T>::find(T const& e, Rank lo, Rank hi) const {
    while ((lo < hi--) && !(e == _elem[hi]));
    return hi;
}

template <typename T>
Rank Vector<T>::insert(Rank r, T const& e) {
    expand();
    for (int i = _size; i > r; --i) _elem[i] = _elem[i - 1];
    _elem[r] = e; ++_size;
    return r;
}

template <typename T>
int Vector<T>::remove(Rank lo, Rank hi) {
    if (lo == hi) return 0;
    while (hi < _size) _elem[lo++] = _elem[hi++];
    _size = lo;
    shrink();
    return hi - lo;
}

template <typename T>
T Vector<T>::remove(Rank r) {
    T e = _elem[r];
    remove(r, r + 1);
    return e;
}

template <typename T>
int Vector<T>::deduplicate() {
    int oldSize = _size;
    Rank i = 1;
    while (i < _size)
        (find(_elem[i], 0, i) < 0) ? ++i : remove(i);
    return oldSize - _size;
}

template <typename T>
int Vector<T>::uniquify() {
    if (_size < 2) return 0;
    Rank i = 0, j = 0;
    while (++j < _size) {
        if (!(_elem[i] == _elem[j])) {
            _elem[++i] = _elem[j];
        }
    }
    _size = i + 1;
    shrink();
    return j - _size;
}

template <typename T>
void Vector<T>::traverse(void (*visit)(T&)) {
    for (int i = 0; i < _size; ++i) visit(_elem[i]);
}

template <typename T>
template <typename VST>
void Vector<T>::traverse(VST& visit) {
    for (int i = 0; i < _size; ++i) visit(_elem[i]);
}

template <typename T>
int Vector<T>::disordered() const {
    int n = 0;
    for (int i = 1; i < _size; ++i)
        if (_elem[i - 1] > _elem[i]) ++n;
    return n;
}

/* 有序查找 - 二分查找 */
template <typename T>
Rank Vector<T>::search(T const& e, Rank lo, Rank hi) const {
    while (lo < hi) {
        Rank mi = (lo + hi) >> 1;
        if (e < _elem[mi]) 
            hi = mi;
        else 
            lo = mi + 1;
    }
    return --lo;
}

/* 排序主入口 */
template <typename T>
void Vector<T>::sort(Rank lo, Rank hi) {
    if (hi - lo < 2) return;
    quickSort(lo, hi);
}

/* 起泡排序 */
template <typename T>
bool Vector<T>::bubble(Rank lo, Rank hi) {
    bool sorted = true;
    while (++lo < hi) {
        if (_elem[lo - 1] > _elem[lo]) {
            sorted = false;
            std::swap(_elem[lo - 1], _elem[lo]);
        }
    }
    return sorted;
}

template <typename T>
void Vector<T>::bubbleSort(Rank lo, Rank hi) {
    while (!bubble(lo, hi--));
}

/* 归并排序 */
template <typename T>
void Vector<T>::mergeSort(Rank lo, Rank hi) {
    if (hi - lo < 2) return;
    Rank mi = (lo + hi) >> 1;
    mergeSort(lo, mi);
    mergeSort(mi, hi);
    merge(lo, mi, hi);
}

template <typename T>
void Vector<T>::merge(Rank lo, Rank mi, Rank hi) {
    T* A = _elem + lo;
    int lb = mi - lo;
    T* B = new T[lb];
    for (Rank i = 0; i < lb; ++i) B[i] = A[i];
    
    int lc = hi - mi;
    T* C = _elem + mi;
    
    for (Rank i = 0, j = 0, k = 0; j < lb; ) {
        if ((k < lc) && (C[k] < B[j])) 
            A[i++] = C[k++];
        else 
            A[i++] = B[j++];
    }
    
    delete[] B;
}

/* 选择排序 */
template <typename T>
void Vector<T>::selectionSort(Rank lo, Rank hi) {
    for (Rank i = lo; i < hi - 1; ++i) {
        Rank minIndex = i;
        for (Rank j = i + 1; j < hi; ++j) {
            if (_elem[j] < _elem[minIndex]) 
                minIndex = j;
        }
        if (minIndex != i) 
            std::swap(_elem[i], _elem[minIndex]);
    }
}

/* 快速排序 */
template <typename T>
Rank Vector<T>::partition(Rank lo, Rank hi) {
    // 随机选择枢轴
    std::swap(_elem[lo], _elem[lo + rand() % (hi - lo + 1)]);
    T pivot = _elem[lo];
    
    while (lo < hi) {
        while ((lo < hi) && (pivot <= _elem[hi])) hi--;
        _elem[lo] = _elem[hi];
        while ((lo < hi) && (_elem[lo] <= pivot)) lo++;
        _elem[hi] = _elem[lo];
    }
    _elem[lo] = pivot;
    return lo;
}

template <typename T>
void Vector<T>::quickSort(Rank lo, Rank hi) {
    if (hi - lo < 2) return;
    Rank mi = partition(lo, hi - 1);
    quickSort(lo, mi);
    quickSort(mi + 1, hi);
}

#endif // MYLIBRARY_VECTOR_H