#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
using namespace std;

class Record {
public:
    string tconst;
    float averageRating;
    int numVotes;

    Record(string _tconst, float _averageRating, int _numVotes);
};

class Node {
public:
    int n;
    vector<int> keys;
    vector<Node*> child_ptr;
    vector<Record*> record_ptr;
    bool leaf;
    int num_keys;
    Node* ptr2next;

    Node(int _n, bool _leaf = false, Node* _ptr2next = NULL);
    Node* insert_record(int k, Record* rptr);
    int smallest();
    void insert_duplicate(int k, Record* rptr);
    bool printRecord(int min, int max);
    Node *delete_record(float k, Node *parentNode);
    Node *merge_left(Node *leftSibling, Node *parentNode, float parentKey);
    Node *merge_right(Node *rightSibling, Node *parentNode, float parentKey);
    Node *borrow_left(Node *leftSibling, Node *parentNode, float parentKey, Node *childNode);
    Node *borrow_right(Node *rightSibling, Node *parentNode, float parentKey, Node *childNode);
    Node *findRightSibling(Node *parentNode, float parentKey, Node *childNode);
    Node *findLeftSibling(Node *parentNode, float parentKey, Node *childNode);
    float findParentKey(Node *parentNode, Node *childNode);
    int findChildPtrIndex(Node *parentNode, Node *currNode);
    int findParentKeyIndex(Node *parentNode, float parentKey);
    void adjustSmallest(Node *parentNode, float k);
    void adjustPointers(Node *parentNode, float k);
    void printKeys();
    int index_inside_node(float k);
    void search(int min, int max = -1);
};

#endif // NODE_H
