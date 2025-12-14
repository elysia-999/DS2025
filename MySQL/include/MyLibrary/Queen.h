#ifndef QUEUE_H
#define QUEUE_H

#include "../../Vector.h"

template <typename T>
class Queue {
private:
    Vector<T> data;
    
public:
    Queue() {}
    
    void enqueue(const T& e) {
        data.insert(e);
    }
    
    T dequeue() {
        if (empty()) {
            throw "Queue is empty";
        }
        return data.remove(0);
    }
    
    T& front() {
        if (empty()) {
            throw "Queue is empty";
        }
        return data[0];
    }
    
    bool empty() const {
        return data.empty();
    }
    
    int size() const {
        return data.size();
    }
};

#endif