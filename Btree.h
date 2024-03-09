#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <vector>
#include <string>
#include "Node.h"
using namespace std;

struct Record;

class BTree {
public:
    Node* root;
    int n;

    BTree(int _t);
    void insert(float k, Record* rptr);
    void deleteKey(float k);
    void search(int min, int max = -1);
    vector<float> searchAverageRating(int min, int max = -1);
    void printTree();
    int countNodes(Node* node);
    int countLevels(Node* node);
    void printRoot();

private:
    void printNode(Node* node, int level, int child_id);
    int countDuplicateKeys(Node* node);
};

#endif // BTREE_H
