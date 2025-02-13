#include <stdio.h>

struct RunningCounter {
    float thresh;
    int count;

    // Get/Set threshold value
    float get_threshold(void) { return thresh; }
    void set_threshold(float newThresh) { thresh = newThresh; }

    // Get/Set count
    int get_count() { return count; }
    void set_count(int newCount) { count = newCount; }

    // check exceeded
    bool thresh_exceeded() { return count >= thresh; }

    // Update function
    void update(float val) {
        if (val > thresh) {
            count++;
        } else {
            count = 0;
        }
    }
};
