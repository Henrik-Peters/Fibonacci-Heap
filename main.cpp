// ---------------------------------------------------------------------
// MIT License
// Copyright (c) 2018 Henrik Peters
// See LICENSE file in the project root for full license information.
// ---------------------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <algorithm>

#ifndef DEBUG
#define DEBUG
#endif

#include "fibheap.h"
using namespace std;

typedef FibonacciHeap<int,char> FibHeap;

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
