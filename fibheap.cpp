// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2018 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#include "fibheap.h"

template <typename K, typename V>
FibonacciHeap<K,V>::FibonacciHeap() {
    rootlist = NULL;
    min = NULL;
    nodeCount = 0;
}

template <typename K, typename V>
FibonacciHeap<K,V>::FibonacciHeap(const FibonacciHeap<K,V>& orig) {

}

template <typename K, typename V>
FibonacciHeap<K,V>::~FibonacciHeap() {

}
