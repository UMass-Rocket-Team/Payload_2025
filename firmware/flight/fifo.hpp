#pragma once

#include <stdlib.h>

template <class T, size_t N>
class FIFO {

    public:

        FIFO() : head(0), tail(0) {}

        /// @brief Adds an element to the FIFO
        /// @param item The element to add
        bool push(T item) {

            if (full()) {

                return false;
            }

            buffer[tail] = item;
            tail = (tail + 1) % N;

            return true;
        }   

        bool pop_from_tail() {
            if (empty()) {
                return false;
            }
            tail = (tail - 1) % N;
            return true;
        }

        /// @brief Removes and returns the next element in the FIFO
        /// @return The next element in the FIFO
        T pop() {

            if (empty()) {

                return T();

            }

            T item = buffer[head];
            head = (head + 1) % N;
            return item;

        }

        /// @brief Returns the next element in the FIFO without removing it
        /// @return The next element in the FIFO
        T peek() {

            if (empty()) {

                return T();

            }

            return buffer[head];

        }

        /// @brief Returns whether the FIFO is empty
        bool empty() {

            return head == tail;

        }

        /// @brief Returns whether the FIFO is full
        bool full() {

            return (tail + 1) % N == head;

        }

        /// @brief Returns the maximum number of elements the FIFO can hold
        size_t capacity() {

            return N;

        }

        /// Returns the number of elements in the FIFO
        size_t size() {

            return (tail - head + N) % N;

        }

        /// Returns the number of elements the FIFO can hold before it is full
        size_t available() {

            return N - size();

        }

    private:

        T buffer[N];
        size_t head;
        size_t tail;

};
