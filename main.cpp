// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2018 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <numeric>
#include <limits>

#ifndef DEBUG
#define DEBUG
#endif

#include "fibheap.h"
using namespace std;

typedef FibonacciHeap<int,char> FibHeap;
typedef FibonacciHeap<int,int> FibInt;

#define TestPassed {return true;}
#define AssertEquals(exp, act) ({\
    if(exp!=act) {\
        cerr << endl << "Asseration failed: Expected <" << exp << "> but was <" << act << "> ";\
        return false;\
    }\
})
#define AssertTrue(x) ({if(!x) {return false;}})
#define AssertFalse(x) ({if(x) {return false;}})

typedef enum TestResult {
    SUCCESS = 0,
    FAILED = 1,
    UNTESTED = 2
} TestResult;

typedef bool (*TestFunc)();
int TestCounter = 1;

//Test case definition
class Test {
private:
    unsigned int id;
    string description;
    TestResult result;
    TestFunc test;

public:
    Test(string description, TestFunc test);
    void run();

    inline unsigned int getID() { return id; };
    inline unsigned int getResult() { return result; };
    inline string getDescription() { return description; };
};

Test::Test(string description, TestFunc test) {
    this->id = TestCounter++;
    this->description = description;
    this->result = UNTESTED;
    this->test = test;
}

void Test::run() {
    this->result = this->test() ? SUCCESS : FAILED;
}

//Test helper functions
class TestQueue {
    private:
        list<pair<int,int>> l;

    public:
        bool isEmpty();
        void insert(int key, int value);
        int extractMin();

        bool decreaseKey(int value, int newKey);
        bool remove(int value);
};

bool TestQueue::isEmpty() {
    return l.empty();
}

void TestQueue::insert(int key, int value) {
    l.push_front(make_pair(key,value));
}

int TestQueue::extractMin() {
    if (isEmpty()) {
        assert(false);

    } else {
        pair<int,int> min = l.front();

        for (auto const& p: l) {
            if (p.first < min.first) {
                min = p;
            }
        }

        l.remove(min);
        return min.second;
    }
}

bool TestQueue::decreaseKey(int value, int newKey) {
    bool decreasedKey = false;

    for (list<pair<int,int>>::iterator it = l.begin(); it != l.end(); ++it) {
        if (it->first == value && newKey < it->first) {
            *it = make_pair(newKey, it->second);
            decreasedKey = true;
        }
    }

    return decreasedKey;
}

bool TestQueue::remove(int value) {
    if (isEmpty()) {
        return false;

    } else {
        pair<int,int> target;

        for (auto const& p: l) {
            if (p.second == value) {
                target = p;
            }
        }

        l.remove(target);
        return true;
    }
}

bool randomTest(unsigned int inserts, unsigned int extracts, int p, unsigned int seed) {
    TestQueue testQueue;
    FibInt* h = new FibInt();
    srand(seed);

    //Number of elements in the queue
    unsigned int count = 0;

    //Insert operations already performed
    unsigned int insertsDone = 0;
    unsigned int extractsDone = 0;

    //Max random number
    int maxInt = numeric_limits<int>::max();

    //Generate the distinct keys and values
    vector<int> items(2 * inserts);
    iota(items.begin(), items.end(), 0);
    random_shuffle(items.begin(), items.end());
    int curItem = 0;

    while (insertsDone < inserts || extractsDone < extracts) {
        int r = (rand() % 100);
        //smallest r: 0
        //highest  r: 99

        if (r < p || count == 0) {
            //Insert
            int key = items.at(curItem++);
            int val = items.at(curItem++);

            h->insert(key, val);
            testQueue.insert(key, val);
            insertsDone++;
            count++;

            if (insertsDone == inserts) {
                p = -1;
            }

        } else {
            //Extract
            AssertEquals(testQueue.extractMin(), h->extractMin());
            extractsDone++;
            count--;
        }
    }

    if (inserts == extracts) {
        AssertTrue(h->isEmpty());
        AssertTrue(testQueue.isEmpty());
    }

    TestPassed;
}

int main() {
    Test testSuite[] = {
        {"Single element insert and remove", []() {
            FibHeap* h = new FibHeap();

            h->insert(1, 'A');
            AssertFalse(h->isEmpty());

            AssertEquals('A', h->extractMin());
            AssertTrue(h->isEmpty());
            delete h;
            TestPassed;
        }},
        {"Tree creation with 1 child", []() {
            FibHeap* h = new FibHeap();
            
            h->insert(2, 'B');
            h->insert(3, 'C');
            h->insert(1, 'A');
            h->insert(4, 'D');

            AssertEquals('A', h->extractMin());
            delete h;
            TestPassed;
        }},
        {"Removing node with 1 child", []() {
            FibHeap* h = new FibHeap();
            
            h->insert(2, 'B');
            h->insert(3, 'C');
            h->insert(1, 'A');
            h->insert(4, 'D');

            AssertEquals('A', h->extractMin());
            AssertEquals('B', h->extractMin());

            delete h;
            TestPassed;
        }},
        {"Consolidate with existing tree", []() {
            FibHeap* h = new FibHeap();
            
            h->insert(2, 'B');
            h->insert(3, 'C');
            h->insert(1, 'A');
            h->insert(4, 'D');
            h->insert(6, 'F');
            h->insert(5, 'E');

            AssertEquals('A', h->extractMin());
            AssertEquals('B', h->extractMin());

            delete h;
            TestPassed;
        }},
        {"Consolidate with tree list", []() {
            FibHeap* h = new FibHeap();
            
            h->insert(2, 'B');
            h->insert(3, 'C');
            h->insert(1, 'A');
            h->insert(4, 'D');
            h->insert(6, 'F');
            h->insert(5, 'E');
            h->insert(7, 'G');
            h->insert(8, 'H');
            h->insert(9, 'I');
            h->insert(10, 'J');
            h->insert(11, 'K');
            h->insert(12, 'L');
            h->insert(13, 'M');
            h->insert(14, 'N');
            h->insert(15, 'O');

            AssertEquals('A', h->extractMin());
            AssertEquals('B', h->extractMin());

            delete h;
            TestPassed;
        }},
        {"Decrease key in the rootlist", []() {
            FibHeap* h = new FibHeap();
            
            h->insert(4, 'B');
            h->insert(6, 'C');
            h->insert(2, 'A');
            h->insert(8, 'D');
            h->insert(9, 'F');
            h->insert(7, 'E');

            AssertEquals('A', h->extractMin());

            h->decreaseKey('E', 5);

            AssertEquals('B', h->extractMin());
            AssertEquals('E', h->extractMin());

            delete h;
            TestPassed;
        }},
        {"Decrease key with parent and single childlist", []() {
            FibHeap* h = new FibHeap();
            char c = 'A';

            for (int i = 1; i < 27; i++, c++) {
                h->insert((i == 7 ? 99 : i), c);
                if (c >= 'Z') {
                    c = 'A' - 1;
                }
            }

            AssertEquals('A', h->extractMin());
            AssertEquals('B', h->extractMin());
            AssertEquals('C', h->extractMin());
            
            h->decreaseKey('I', 7);

            AssertEquals('D', h->extractMin());
            AssertEquals('E', h->extractMin());
            AssertEquals('F', h->extractMin());
            AssertEquals('I', h->extractMin());

            delete h;
            TestPassed;
        }},
        {"Decrease key with parent and childlist", []() {
            FibHeap* h = new FibHeap();
            char c = 'A';

            for (int i = 1; i < 27; i++, c++) {
                h->insert(i, c);
                if (c >= 'Z') {
                    c = 'A' - 1;
                }
            }

            AssertEquals('A', h->extractMin());
            AssertEquals('B', h->extractMin());
            AssertEquals('C', h->extractMin());

            h->decreaseKey('K', 3);
            AssertEquals('K', h->extractMin());

            delete h;
            TestPassed;
        }},
        {"Decrease key with marked parent", []() {
            FibHeap* h = new FibHeap();
            char c = 'A';

            for (int i = 1; i < 27; i++, c++) {
                h->insert(2 * i, c);
                if (c >= 'Z') {
                    c = 'A' - 1;
                }
            }

            AssertEquals('A', h->extractMin());

            h->decreaseKey('W', 5);
            h->decreaseKey('X', 7);

            AssertEquals('B', h->extractMin());
            AssertEquals('W', h->extractMin());
            AssertEquals('C', h->extractMin());
            AssertEquals('X', h->extractMin());

            delete h;
            TestPassed;
        }},
        {"Decrease key with two marked parents", []() {
            FibHeap* h = new FibHeap();
            char c = 'a';

            for (int i = 1; i < 40; i++, c++) {
                h->insert(2 * i, c);
                if (c >= 'z') {
                    c = 'A' - 1;
                }
            }

            AssertEquals('a', h->extractMin());

            h->decreaseKey('z', 7);
            h->decreaseKey('x', 5);

            //Final decreaseKey with two marked parents
            h->decreaseKey('w', 9);

            AssertEquals('b', h->extractMin());
            AssertEquals('x', h->extractMin());
            AssertEquals('c', h->extractMin());
            AssertEquals('z', h->extractMin());
            AssertEquals('d', h->extractMin());
            AssertEquals('w', h->extractMin());
            
            delete h;
            TestPassed;
        }},
        {"Remove with single element", []() {
            FibHeap* h = new FibHeap();

            h->insert(5, 'A');
            h->remove('A');

            AssertTrue(h->isEmpty());

            delete h;
            TestPassed;
        }},
        {"Remove with 10 elements", []() {
            FibHeap* h = new FibHeap();
            char c = 'A';

            for (int i = 1; i < 27; i++, c++) {
                h->insert(i, c);
                if (c >= 'Z') {
                    c = 'A' - 1;
                }
            }

            h->remove('B');
            h->remove('F');
            h->remove('K');
            h->remove('O');
            h->remove('C');
            h->remove('E');
            h->remove('J');
            h->remove('R');
            h->remove('Q');
            h->remove('T');

            AssertEquals('A', h->extractMin());
            AssertEquals('D', h->extractMin());
            AssertEquals('G', h->extractMin());
            AssertEquals('H', h->extractMin());
            AssertEquals('I', h->extractMin());
            AssertEquals('L', h->extractMin());
            AssertEquals('M', h->extractMin());
            AssertEquals('N', h->extractMin());
            AssertEquals('P', h->extractMin());
            AssertEquals('S', h->extractMin());
            AssertEquals('U', h->extractMin());
            AssertEquals('V', h->extractMin());
            AssertEquals('W', h->extractMin());
            AssertEquals('X', h->extractMin());
            AssertEquals('Y', h->extractMin());
            AssertEquals('Z', h->extractMin());
            AssertTrue(h->isEmpty());

            delete h;
            TestPassed;
        }},
        {"Copy constructor test", []() {
            FibHeap h;

            h.insert(3, 'A');
            h.insert(4, 'B');
            h.insert(5, 'C');

            FibHeap h3 = h;
            AssertEquals('A', h3.extractMin());
            AssertEquals('B', h3.extractMin());
            AssertEquals('C', h3.extractMin());

            AssertEquals('A', h.extractMin());
            AssertEquals('B', h.extractMin());
            AssertEquals('C', h.extractMin());

            h.insert(7, 'D');
            AssertTrue(h3.isEmpty());

            AssertEquals('D', h.extractMin());
            AssertTrue(h.isEmpty());
            TestPassed;
        }},
        {"Random with 10 elements", []() {
            return randomTest(10, 10, 90, 314215183);
        }},
        {"Random with 100 elements", []() {
            return randomTest(100, 100, 75, 917114197);
        }},
        {"Random with 1000 elements", []() {
            return randomTest(1000, 1000, 65, 571381511);
        }},
        {"Random with 2000 elements", []() {
            return randomTest(2000, 2000, 55, 462969643);
        }},
        {"Random with 5000 elements", []() {
            return randomTest(5000, 5000, 75, 493825932);
        }}
    };

    unsigned int passed = 0;
    unsigned int failed = 0;

    for (unsigned int i = 0; i < sizeof(testSuite)/sizeof(testSuite[0]); i++) {
        Test t = testSuite[i];
        cout << "Running Test " << setfill('0') << setw(2) << t.getID() << ": ";
        t.run();

        if (t.getResult() == FAILED) {
            failed++;
            cout << "\033[1;31mFailed\033[0m";
            cout << " (" << t.getDescription() << ")" << endl;
        } else {
            passed++;
            cout << "\033[1;32mPassed\033[0m";
            cout << " (" << t.getDescription() << ")" << endl;
        }
    }
    
    unsigned int sucessRate = (passed * 100) / (passed + failed);

    cout << "--------------------" << endl;
    cout << "Tests passed: ";
    if (sucessRate == 100) {
        cout << "\033[1;32m" << sucessRate << "%\033[0m" << endl;
    } else {
        cout << "\033[1;31m" << sucessRate << "%\033[0m" << endl;
    }
    cout << "--------------------" << endl;
    return 0;
}
