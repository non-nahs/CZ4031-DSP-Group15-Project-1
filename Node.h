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
    vector<float> keys;
    vector<Node*> child_ptr;
    vector<Record*> record_ptr;
    bool leaf;
    int num_keys;
    Node* ptr2next;

    Node(int _n, bool _leaf = false, Node* _ptr2next = NULL);
    Node* insert_record(float k, Record* rptr);
    float smallest();
    int split_check_duplicates(vector<float> keys);
    void printRecord(Record* record);
    Node* search(float k);
};

#endif // NODE_H
