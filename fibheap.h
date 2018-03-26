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
#include <utility>
#include <list>
#include <map>
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
            bool marked;
            unsigned int degree;
        } *rootlist, *min;

        unsigned int nodeCount;

        void meldNode(Node* node);
        void consolidate();
        Node* link(Node* a, Node* b);

        inline void appendNode(Node* node);
        Node* find(Node* list, V value);
        void cut(Node* node);

    public:
        FibonacciHeap();
        FibonacciHeap(const FibonacciHeap<K,V>& orig);
        ~FibonacciHeap();

        bool isEmpty();
        void insert(K key, V value);
        void meld(FibonacciHeap<K,V>* other);

        V getMin();
        V extractMin();

        void decreaseKey(V value, K newKey);

        #ifdef DEBUG
        bool invariant();
        bool invariantList(Node* node);
        bool invariantNode(Node* node);
        bool invariantHeapOrder(Node* node, K key);
        unsigned int invariantNodeCount(Node* node);

        bool normalized(Node* node);
        void dump(string dumpName = "dump");
        void dumpNode(list<pair<Node*,int>>* nodeList, Node* node, int depth);
        #endif

};

#endif /* FIBHEAP_H */
