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

template <typename K, typename V>
bool FibonacciHeap<K,V>::isEmpty() {
    return rootlist == NULL;
}

template <typename K, typename V>
void FibonacciHeap<K,V>::insert(K key, V value) {
    Node* node = new Node;

    //Initialize the node values
    node->prev = node;
    node->next = node;
    node->child = NULL;
    node->parent = NULL;
    node->key = key;
    node->value = value;
    node->degree = 0;
    node->marked = false;
    
    //Meld the node into the rootlist
    meldNode(node);
}

template <typename K, typename V>
void FibonacciHeap<K,V>::meldNode(Node* node) {
}

template class FibonacciHeap<int, char>;
