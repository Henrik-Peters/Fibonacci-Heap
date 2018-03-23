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
        Node* minChild = min->child;

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
        nodeCount--;

        if (minChild != NULL) {
            //Add all childs of the minimum to the rootlist
            Node* curNode = minChild;

            do {
                curNode->marked = false;
                curNode->parent = NULL;

                //In order not to break the iteration during node prepanding
                //save the current node and move the iteration to the next node
                Node* node = curNode;
                curNode = curNode->next;
                
                //Prepend node to the rootlist
                node->next = rootlist;
                node->prev = rootlist->prev;

                rootlist->prev->next = node;
                rootlist->prev = node;
                rootlist = node;

            } while (curNode != minChild);
        }

        consolidate();
        return minValue;
    }
}

template <typename K, typename V>
void FibonacciHeap<K,V>::consolidate() {
    //Empty heap or single node
    if (nodeCount <= 1) return;

    unsigned int maxTrees = 2 * log2(nodeCount);
    Node* trees[maxTrees] = {NULL};

    Node* curNode = rootlist;

    //Link rootlist nodes with the same degree
    do {
        Node* node = curNode;
        curNode = curNode->next;

        if (trees[node->degree] != NULL) {
            node = link(trees[node->degree], node);
        }

        trees[node->degree] = node;
        
    } while (curNode != rootlist);

    //Create the new rootlist
    rootlist = NULL;

    for (unsigned int i = 0; i < maxTrees; i++) {
        if (trees[i] != NULL) {

            //First node for the new rootlist
            if (rootlist == NULL) {
                rootlist = min = trees[i];
                rootlist->prev = rootlist->next = rootlist;
            }

            //Prepend the node to the rootlist
            trees[i]->next = rootlist;
            trees[i]->prev = rootlist->prev;

            rootlist->prev = trees[i];
            rootlist = trees[i];

            //Update the minimum pointer
            if (trees[i]->key < min->key) {
                min = trees[i];
            }
        }
    }
}

template <typename K, typename V>
typename FibonacciHeap<K,V>::Node* FibonacciHeap<K,V>::link(Node* a, Node* b) {
    #ifdef DEBUG
    //When linking two nodes they have the same degree
    assert (a->degree == b->degree);
    #endif

    //Make sure that a is the smaller node
    if (a->key > b->key) {
        Node* swap = a;
        a = b;
        b = swap;
    }

    //Pointers to b can be ignored because they will be overwritten
    //by the rootlist reconstruction during consolidation
    a->degree++;
    a->marked = false;

    //Move b to the childlist of a
    b->parent = a;

    if (a->child == NULL) {
        b->prev = b->next = b;
        a->child = b;

    } else {
        //Set b between the child of a and the last node
        b->prev = a->child->prev;
        b->next = a->child;

        //Update the next pointer of the last node
        b->prev->next = b;
        a->child->prev = b;
    }

    return a;
}

#ifdef DEBUG
template <typename K, typename V>
void FibonacciHeap<K,V>::dump(string dumpName) {
    system("mkdir -p dump");
    ofstream graphFile;
    graphFile.open("dump/" + dumpName + ".gv");

    graphFile << "digraph G {" << endl;
    graphFile << "node [style=filled];" << endl;
    graphFile << "graph [pad=\"0.1\", nodesep=\"1\", ranksep=\"1.5\"];" << endl;

    int maxDepth = 0;
    auto nodeList = new list<pair<Node*,int>>();

    //Add all nodes to the list
    dumpNode(nodeList, rootlist, 0);

    //Find the max depth
    for (auto curPair : *nodeList) {
        if (curPair.second > maxDepth) {
            maxDepth = curPair.second;
        }
    }

    //Add the rank headers
    for (int i = 0; i <= maxDepth; i++) {
        graphFile << "{rank=same; d" << i << "[style=invis, shape=point]; ";

        for (auto curPair : *nodeList) {
            if (curPair.second == i) {
                graphFile << curPair.first->key << ";";
            }
        }

        graphFile << "}" << endl;
    }

    //Dump the nodes
    for (auto curPair : *nodeList) {
        Node* node = curPair.first;

        string prevColor = "0.650 0.700 0.700"; //blue
        string nextColor = "0.348 0.839 0.839"; //green
        string parentColor = "#FF9900"; //red
        string childColor = "#000000"; //black

        graphFile << node->key << " -> " << node->prev->key << " [color=\"" << prevColor << "\"];" << endl;
        graphFile << node->key << " -> " << node->next->key << " [color=\"" << nextColor << "\"];" << endl;

        if (node->parent != NULL) {
            graphFile << node->key << " -> " << node->parent->key << " [color=\"" << parentColor << "\"];" << endl;
        }

        if (node->child != NULL) {
            graphFile << node->key << " -> " << node->child->key << " [color=\"" << childColor << "\"];" << endl;
        }

        if (node == min) {
            graphFile << node->key << " [shape=record, fillcolor=\"0.650 0.200 1.000\", style=filled,";
        } else {
            graphFile << node->key << " [shape=record,";
        }

        graphFile << "label=\"{{" << node->key << "|" << node->value << "}|" << node->degree << "}\"];" << endl;
    }

    //Add the rank footer
    graphFile << "edge[style=invis];" << endl;
    graphFile << "d0";

    for (int i = 1; i <= maxDepth; i++) {
        graphFile << "->d" << i;
    }

    graphFile << ";" << endl;
    graphFile << "}" << endl;

    delete nodeList;
    graphFile.close();

    string graphizCall = "dot -Tpng dump/" + dumpName + ".gv -o dump/" + dumpName + ".png";
    string openCall = "xdg-open dump/" + dumpName + ".png";

    system(graphizCall.c_str());
    system("cd dump && rm *.gv");
    system(openCall.c_str());
}

template <typename K, typename V>
void FibonacciHeap<K,V>::dumpNode(list<pair<Node*,int>>* nodeList, Node* node, int depth) {
    Node* curNode = node;

    do {
        nodeList->push_back(make_pair(curNode, depth));

        if (curNode->child != NULL) {
            dumpNode(nodeList, curNode->child, depth + 1);
        }

        curNode = curNode->next;
    } while (curNode != node);
}
#endif

template class FibonacciHeap<int, char>;
