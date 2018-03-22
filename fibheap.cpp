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
void FibonacciHeap<K,V>::meld(FibonacciHeap<K,V>* other) {
    //There is no change when merging with an empty rootlist
    if (rootlist != NULL && other != NULL) {

        //Meld with the root node of the other rootlist
        meldNode(other->rootlist);

        //Reset the secound rootlist to prevent unexpected side-effects
        //This may be omitting for better performance
        other->rootlist = NULL;
        other->min = NULL;
        other->nodeCount = 0;
    }
}

template <typename K, typename V>
void FibonacciHeap<K,V>::meldNode(Node* node) {
    //There is no change when merging with an empty node
    if (rootlist == NULL) {
        rootlist = node;
        min = node;
        nodeCount = 1;
        
    } else if (node != NULL) {
        
        //Make sure the smaller node is in the current rootlist
        if (rootlist->key > node->key) {
            K swapKey = rootlist->key;
            V swapValue = rootlist->value;

            rootlist->key = node->key;
            rootlist->value = node->value;

            node->key = swapKey;
            node->value = swapValue;
        }

        //Save the intermediate nodes before concatenation
        Node* innerNext = rootlist->next;
        Node* innerPrev = node->prev;

        //Concatenate the rootlist nodes
        rootlist->next = node;
        node->prev = rootlist;

        //Concatenate intermediate nodes
        innerNext->prev = innerPrev;
        innerPrev->next = innerNext;

        //Update the min pointer
        if (node->key < min->key) {
            min = node;
        }

        nodeCount++;
    }
}

template <typename K, typename V>
V FibonacciHeap<K,V>::getMin() {
    if (min == NULL) {
        //Empty rootlist
        return (V)0;
    } else {
        return min->value;
    }
}

template <typename K, typename V>
V FibonacciHeap<K,V>::extractMin() {
    if (min == NULL) {
        //Empty rootlist
        return (V)0;
    } else {
        //Save values of the min node
        V minValue = min->value;

        //Remove the min node from the list
        min->next->prev = min->prev;
        min->prev->next = min->next;

        //Update the rootlist pointer
        if (rootlist == min) {
            rootlist = (min->next == min)
                ? NULL
                : min->next;
        }

        delete min;
        min = NULL; //can be omitted for better peformance
        nodeCount--;

        //TODO: Add all childs of the minimum to the rootlist
        //TODO: Link rootlist nodes with the same degree together

        return minValue;
    }
}

template class FibonacciHeap<int, char>;
