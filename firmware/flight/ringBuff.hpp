#pragma once
#include <cstddef>

template <class T, size_t N>
class RingBuffer {
private:
    T buffer[N];
    size_t head = 0;
    size_t tail = 0;
    bool full = false;
public:
    RingBuffer() { clear(); }
    
    void clear() {
        head = tail = 0;
        full = false;
    }
    
    bool empty() const { return (!full && (head == tail)); }
    bool isFull() const { return full; }
    size_t capacity() const { return N; }
    
    size_t size() const {
        if (full) return N;
        return (head >= tail) ? (head - tail) : (N + head - tail);
    }
    
    bool push(const T& item) {
        if (full) { return false; }
        buffer[head] = item;
        head = (head + 1) % N;
        if (head == tail) full = true;
        return true;
    }
    
    bool pop(T& item) {
        if (empty()) { return false; }
        item = buffer[tail];
        tail = (tail + 1) % N;
        full = false;
        return true;
    }

    T operator[](size_t idx) const {
        return buffer[(tail + idx) % N];
    }
};
