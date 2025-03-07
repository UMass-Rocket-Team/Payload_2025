#pragma once

class RunningCounter {
    public:
        RunningCounter(int thr) : count(0), thresh(thr) {}

        void update(bool status) {
            if (status) {
                count++;
            } else {
                count = 0;
            }
        }

        bool check() {
            return count >= thresh;
        }

        void reset() {
            count = 0;
        }

    private:
        int count;
        int thresh;

};