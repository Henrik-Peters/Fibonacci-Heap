// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2018 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#include "fibheap.h"

template <typename K, typename V>
void FibonacciHeap<K,V>::makeHeap() {
    rootlist = NULL;
    min = NULL;
    nodeCount = 0;
}

template <typename K, typename V>
FibonacciHeap<K,V>::FibonacciHeap() {
    makeHeap();
}

template <typename K, typename V>
FibonacciHeap<K,V>::FibonacciHeap(const FibonacciHeap<K,V>& orig) {
    makeHeap();

    if (!orig.isEmpty()) {
        insertList(orig.rootlist);
    }
}

template <typename K, typename V>
FibonacciHeap<K,V>& FibonacciHeap<K,V>::operator=(const FibonacciHeap<K,V>& rhs) {
    //Free the current nodes
    if (!isEmpty()) {
        freeList(rootlist);

        //Set the current heap to the empty state
        makeHeap();
    }

    if (!rhs.isEmpty()) {
        insertList(rhs.rootlist);
    }

    return *this;
}

template <typename K, typename V>
FibonacciHeap<K,V>::~FibonacciHeap() {
    if (!isEmpty()) {
        freeList(rootlist);
    }
}

template <typename K, typename V>
void FibonacciHeap<K,V>::freeList(Node* node) {
    Node* iterateNode = node->next;

    do {
        Node* curNode = iterateNode;
        iterateNode = iterateNode->next;

        //Recursive list freeing
        if (curNode->child != NULL) {
            freeList(curNode->child);
        }

        //Prevent double freeing of lists with 1 element
        if (curNode != node) {
            delete curNode;
        }

    } while (iterateNode != node);

    //Free the first element
    delete node;
}

template <typename K, typename V>
bool FibonacciHeap<K,V>::isEmpty() const {
    return rootlist == NULL;
}

template <typename K, typename V>
void FibonacciHeap<K,V>::insertList(Node* node) {
    Node* curNode = node;

    do {
        insert(curNode->key, curNode->value);

        //Recursive list insertion
        if (curNode->child != NULL) {
            insertList(curNode->child);
        }

        curNode = curNode->next;
    } while (curNode != node);
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

    #ifdef DEBUG
    assert (invariant());
    #endif
}

template <typename K, typename V>
void FibonacciHeap<K,V>::meld(FibonacciHeap<K,V>* other) {
    //There is no change when merging with an empty rootlist
    if (rootlist != NULL && other != NULL) {

        //Meld with the root node of the other rootlist
        meldNode(other->rootlist);

        #ifdef DEBUG
        assert (invariant());
        #endif

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
        rootlist = min = node;
        nodeCount = 1;
        
    } else if (node != NULL) {

        //Append the node to the rootlist
        appendNode(node);

        //Update the min pointer
        if (node->key < min->key) {
            min = node;
        }

        nodeCount++;
    }
}

template <typename K, typename V>
void FibonacciHeap<K,V>::appendNode(Node* node) {
    //Save the pointer to the old last node
    Node* lastNode = rootlist->prev;

    //Set the pointers to the new node
    lastNode->next = node;
    rootlist->prev = node;

    //Set the pointers from the new node
    node->prev = lastNode;
    node->next = rootlist;
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

                //In order not to break the iteration during node appending
                //save the current node and move the iteration to the next node
                Node* node = curNode;
                curNode = curNode->next;

                //Add the node to the rootlist
                if (rootlist == NULL) {
                    rootlist = node;
                    rootlist->next = rootlist;
                    rootlist->prev = rootlist;
                } else {
                    appendNode(node);
                }

            } while (curNode != minChild);
        }

        consolidate();
        return minValue;
    }
}

template <typename K, typename V>
void FibonacciHeap<K,V>::consolidate() {
    //Empty heap or single node
    if (nodeCount <= 1) {
        min = rootlist;
        return;
    }

    unsigned int maxTrees = 2 * log2(nodeCount);
    Node* trees[maxTrees] = {NULL};

    Node* curNode = rootlist;

    //Link rootlist nodes with the same degree
    do {
        Node* node = curNode;
        curNode = curNode->next;

        while (trees[node->degree] != NULL) {
            int prevDegree = node->degree;
            node = link(trees[node->degree], node);

            trees[prevDegree] = NULL;
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

            //Append the node to the rootlist
            appendNode(trees[i]);

            //Update the minimum pointer
            if (trees[i]->key < min->key) {
                min = trees[i];
            }
        }
    }

    #ifdef DEBUG
    assert (invariant());
    assert (normalized(rootlist));
    #endif
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

template <typename K, typename V>
bool FibonacciHeap<K,V>::decreaseKey(V value, K newKey) {
    Node* node = find(rootlist, value);

    //Check that the node exists and the new key is smaller
    if (node && newKey < node->key) {
        node->key = newKey;

        //Update the min pointer
        if (node->key < min->key) {
            min = node;
        }

        //Done when the node is in the rootlist or the parent still has a lower key
        if (node->parent != NULL && node->parent->key > newKey) {
            //Repair the heap order

            do {
                //Cascading node cuts
                Node* oldParent = node->parent;
                cut(node);
                node = oldParent;

            } while (node->marked && node->parent != NULL);

            //Mark the child lost
            if (node->parent != NULL) {
                node->marked = true;
            }
        }

        #ifdef DEBUG
        assert (invariant());
        #endif

        return true;
    } else {
        return false;
    }
}

template <typename K, typename V>
void FibonacciHeap<K,V>::cut(Node* node) {
    if (node->parent != NULL) {
        //Move node to the rootlist
        node->parent->degree--;

        //Update the child pointer
        if (node->parent->degree == 0) {
            node->parent->child = NULL;
        } else {
            node->parent->child = node->next;
        }

        //Nodes in the rootlist have no parent
        node->parent = NULL;

        //Remove node from its list
        node->next->prev = node->prev;
        node->prev->next = node->next;

        //Insert the node into the rootlist
        appendNode(node);
    }
}

template <typename K, typename V>
typename FibonacciHeap<K,V>::Node* FibonacciHeap<K,V>::find(Node* list, V value) {
    Node* curNode = list;

    do {
        if (curNode->value == value) {
            return curNode;
        }

        //Recursive search for the child lists
        if (curNode->child != NULL) {
            Node* childSearch = find(curNode->child, value);

            if (childSearch) {
                return childSearch;
            }
        }

        curNode = curNode->next;
    } while (curNode != list);

    return NULL;
}

template <typename K, typename V>
bool FibonacciHeap<K,V>::remove(V value) {
    //Decrease the key of the target node to become the new min node
    if (decreaseKey(value, min->key - (K)1)) {
        extractMin();
        return true;
    }

    return false;
}

#ifdef DEBUG
template <typename K, typename V>
bool FibonacciHeap<K,V>::invariant() {
    if (isEmpty()) {
        return rootlist == NULL && min == NULL && nodeCount == 0;

    } else {
        bool invNodeCount = (invariantNodeCount(rootlist) == nodeCount);
        bool invNonEmpty = rootlist != NULL && min != NULL && nodeCount > 0;

        //Check invariantNode for all nodes
        return invNodeCount && invNonEmpty && invariantList(rootlist);
    }
}

template <typename K, typename V>
bool FibonacciHeap<K,V>::invariantList(Node* node) {
    Node* curNode = node;
    bool invNodes = true;

    do {
        invNodes &= invariantNode(curNode);

        //Recursive descent for trees
        if (curNode->child != NULL) {
            invNodes &= invariantList(curNode->child);
        }

        curNode = curNode->next;
    } while (curNode != node);

    return invNodes;
}

template <typename K, typename V>
bool FibonacciHeap<K,V>::invariantNode(Node* node) {
    //All nodes below have a higher order
    bool heapOrder = (node->child == NULL) || invariantHeapOrder(node->child, node->key);
    bool invDegree = (node->child == NULL) || invariantDegree(node);

    //The min pointer has the lowest key
    bool minOrder = (node == min) || (min->key <= node->key);

    //Chaining of the list
    bool prevChain = node->next->prev == node;
    bool nextChain = node->prev->next == node;

    //Chaining of the tree
    bool childChain = node->child == NULL || node->child->parent == node;
    bool childListChain = true;

    if (node->child != NULL) {
        Node* curNode = node->child;

        do {
            childListChain &= (curNode->parent == node);
            curNode = curNode->next;
        } while (curNode != node->child);
    }
    
    return heapOrder && invDegree && minOrder && prevChain &&
           nextChain && childChain && childListChain;
}

template <typename K, typename V>
bool FibonacciHeap<K,V>::invariantHeapOrder(Node* node, K key) {
    Node* curNode = node;
    bool heapOrder = true;

    do {
        if (curNode->key < key) {
            heapOrder = false;
        }

        if (curNode->child != NULL) {
            heapOrder &= invariantHeapOrder(curNode->child, key);
        }

        curNode = curNode->next;
    } while (curNode != node);

    return heapOrder;
}

template <typename K, typename V>
bool FibonacciHeap<K,V>::invariantDegree(Node* node) {
    Node* curNode = node->child;
    unsigned int listCount = 0;

    do {
        listCount++;
        curNode = curNode->next;
    } while (curNode != node->child);

    return node->degree == listCount;
}

template <typename K, typename V>
unsigned int FibonacciHeap<K,V>::invariantNodeCount(Node* node) {
    unsigned int nodeSum = 0;
    Node* curNode = node;

    do {
        nodeSum++;

        if (curNode->child != NULL) {
            nodeSum += invariantNodeCount(curNode->child);
        }

        curNode = curNode->next;
    } while (curNode != node);

    return nodeSum;
}

template <typename K, typename V>
bool FibonacciHeap<K,V>::normalized(Node* node) {
    map<unsigned int,unsigned int> nodeMap;
    bool uniqueDegrees = true;
    Node* curNode = node;

    do {
        nodeMap[curNode->degree]++;

        if (nodeMap[curNode->degree] > 1) {
            uniqueDegrees = false;
        }

        curNode = curNode->next;
    } while (curNode != node);

    return uniqueDegrees;
}

template <typename K, typename V>
void FibonacciHeap<K,V>::dump(string dumpName) {
    system("mkdir -p dump");
    ofstream graphFile;
    graphFile.open("dump/" + dumpName + ".gv");

    graphFile << "digraph G {" << endl;
    graphFile << "node [style=filled, fontname = \"arial\"];" << endl;
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

        } else if (node->marked) {
            graphFile << node->key << " [shape=record, fillcolor=\"#EEC591\", style=filled,";

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

template class FibonacciHeap<int,char>;
template class FibonacciHeap<int,int>;
