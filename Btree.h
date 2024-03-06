#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <vector>
#include <string>
#include "Node.h"
using namespace std;

class BTree {
public:
    Node* root;
    int t;

    BTree(int _t);
    void insert(float k, Record* rptr);
    void deleteKey(float k);
    void search(int min, int max = -1);
    void printTree();
private:
    void printNode(Node* node, int level);
};

#endif // BTREE_H
