// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2018 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#include <iostream>
#include <cmath>
#include <cstdlib>

#ifndef DEBUG
#define DEBUG
#endif

#ifdef DEBUG
#include <assert.h>
#include <fstream>
#include <list>
#include <utility>
#endif

using namespace std;

#ifndef FIBHEAP_H
#define FIBHEAP_H

template <typename K, typename V>
class FibonacciHeap final {
    private:
        struct Node {
            Node *prev, *next, *child, *parent;
            K key;
            V value;
            int degree;
            bool marked;
        } *rootlist, *min;

        unsigned int nodeCount;

        void meldNode(Node* node);
        void consolidate();

    public:
        FibonacciHeap();
        FibonacciHeap(const FibonacciHeap<K,V>& orig);
        ~FibonacciHeap();

        bool isEmpty();
        void insert(K key, V value);
        void meld(FibonacciHeap<K,V>* other);

        V getMin();
        V extractMin();

        #ifdef DEBUG
        void dump(string dumpName = "dump");
        void dumpNode(list<pair<Node*,int>>* nodeList, Node* node, int depth);
        #endif

};

#endif /* FIBHEAP_H */
