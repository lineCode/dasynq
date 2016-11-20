#include "dasynq-stableheap.h"
#include "dasynq-btreequeue.h"
#include "dasynq-pairingheap.h"
#include "dasynq-binaryheap.h"
#include <functional>
#include <random>
#include <chrono>
#include <algorithm>

#include <iostream>

int main(int argc, char **argv)
{
    // Template arguments are: data type, priority type, comparator
    // auto heap = dasynq::PairingHeap<int, int, std::less<int>>();
    // auto heap = dasynq::BinaryHeap<int, int, std::less<int>>();
    // auto heap = dasynq::BTreeQueue<int, int, std::less<int>, 16>();
    
    auto heap = StableHeap<dasynq::PairingHeap, int, int>();
    
    constexpr int NUM = 10000000;
    // constexpr int NUM = 5;
    // constexpr int NUM = 100000;
    
    std::mt19937 gen(0);
    std::uniform_int_distribution<> r(0, NUM);
    
    // Ordered fill/dequeue
    
    auto starttime = std::chrono::high_resolution_clock::now();
    
    using handle_t = decltype(heap)::handle_t;
    
    handle_t *indexes = new handle_t[NUM];
    for (int i = 0; i < NUM; i++) {
        heap.allocate(indexes[i], i);
        heap.insert(indexes[i], i);
    }
    
    for (int i = 0; i < NUM; i++) {
        auto r = heap.get_root();
        // std::cout << heap.get_data(r) << std::endl;
        heap.pull_root();
        heap.deallocate(r);
    }
    
    auto endtime = std::chrono::high_resolution_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count();
    
    std::cout << "Ordered fill/dequeue: " << millis << std::endl;
    
    if (! heap.empty()) abort();

    // Flat priority fill/dequeue
    
    starttime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM; i++) {
        heap.allocate(indexes[i], i);
        heap.insert(indexes[i], 0);
    }
    
    for (int i = 0; i < NUM; i++) {
        auto r = heap.get_root();
        // std::cout << heap.get_data(r) << std::endl;
        heap.pull_root();
        heap.deallocate(r);
    }
    
    endtime = std::chrono::high_resolution_clock::now();
    millis = std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count();
    
    std::cout << "Flat priority fill/dequeue: " << millis << std::endl;

    if (! heap.empty()) abort();
    
    // Random fill/dequeue
    
    starttime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM; i++) {
        int ii = r(gen);
        heap.allocate(indexes[i], ii);
        heap.insert(indexes[i], ii);
    }
    
    for (int i = 0; i < NUM; i++) {
        auto r = heap.get_root();
        // std::cout << heap.get_data(r) << std::endl;
        heap.pull_root();
        heap.deallocate(r);
    }
    
    endtime = std::chrono::high_resolution_clock::now();
    millis = std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count();
    
    std::cout << "Random fill/dequeue: " << millis << std::endl;

    if (! heap.empty()) abort();
    
    // Queue cycle
    
    starttime = std::chrono::high_resolution_clock::now();
    
    int active = 0;
    for (int i = 0; i < NUM; i++) {
        int ii = r(gen);
        heap.allocate(indexes[i], ii);
        heap.insert(indexes[i], ii);
        active++;
        
        if (active > 1000) {
            auto r = heap.get_root();
            heap.pull_root();
            heap.deallocate(r);
            active--;
        }
    }
    
    for (int i = 0; i < 1000; i++) {
        auto r = heap.get_root();
        heap.pull_root();
        heap.deallocate(r);
    }

    endtime = std::chrono::high_resolution_clock::now();
    millis = std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count();
    
    std::cout << "Cycle fill/dequeue: " << millis << std::endl;

    if (! heap.empty()) abort();
    
    // Ordered fill/random remove

    int * order = new int[NUM];
    for (int i = 0; i < NUM; i++) order[i] = i;
    std::shuffle(order, order + NUM, gen);
    
    starttime = std::chrono::high_resolution_clock::now();
        
    for (int i = 0; i < NUM; i++) {
        heap.allocate(indexes[i], i);
        heap.insert(indexes[i], i);
    }
    
    // std::shuffle(indexes, indexes + NUM, gen);
    
    for (int i = 0; i < NUM; i++) {
        heap.remove(indexes[order[i]]);
        heap.deallocate(indexes[order[i]]);
    }    

    endtime = std::chrono::high_resolution_clock::now();
    millis = std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count();
    
    std::cout << "Ordered fill/random remove: " << millis << std::endl;
    
    delete[] order;
    if (! heap.empty()) abort();

    // Pathological fill/remove

    starttime = std::chrono::high_resolution_clock::now();
        
    for (int i = 0; i < NUM; i++) {
        heap.allocate(indexes[i], i);
        heap.insert(indexes[i], i);
    }
        
    for (int i = 1; i < NUM; i++) {
        heap.remove(indexes[i]);
        heap.deallocate(indexes[i]);
    }
    
    heap.remove(indexes[0]);
    heap.deallocate(indexes[0]);

    endtime = std::chrono::high_resolution_clock::now();
    millis = std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count();
    
    std::cout << "Pathological fill/remove: " << millis << std::endl;

    if (! heap.empty()) abort();

    return 0;
}
