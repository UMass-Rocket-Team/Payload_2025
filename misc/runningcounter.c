#include <stdio.h>

struct RunningCounter {
private:
    float thresh;
    int count;
    int countThresh;

public:
    // Constructor
    RunningCounter(float threshold, int countThreshold)
        : thresh(threshold), count(0), countThresh(countThreshold) {}

    // Update function
    void Update(float val) {
        if (val > thresh) {
            count++;
        } else {
            count = 0;
        }
    }

    // Get/Set threshold value
    float GetThreshold() const { return thresh; }
    void SetThreshold(float newThresh) { thresh = newThresh; }

    // Get/Set counting threshold value
    int GetCountThreshold() const { return countThresh; }
    void SetCountThreshold(int newCountThresh) { countThresh = newCountThresh; }

    // Check if counting value exceeds counting threshold
    bool IsCountThresholdExceeded() const { return count >= countThresh; }

    // Get current count value (optional, for debugging)
    int GetCount() const { return count; }
};

// Example usage
int main() {
    RunningCounter counter(10.0, 3);
    
    counter.Update(12.0);
    counter.Update(15.0);
    counter.Update(8.0);  // Resets count
    counter.Update(11.0);
    counter.Update(12.5);
    counter.Update(13.0);
    
    if (counter.IsCountThresholdExceeded()) {
        std::cout << "Count threshold exceeded!" << std::endl;
    } else {
        std::cout << "Count threshold not met." << std::endl;
    }
    
    return 0;
}
