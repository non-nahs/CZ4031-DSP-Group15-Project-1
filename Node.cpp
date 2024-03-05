#include "Node.h"
#include <algorithm>
#include <vector>
using namespace std;

Record::Record(string _tconst, float _averageRating, int _numVotes) {
    tconst = _tconst;
    averageRating = _averageRating;
    numVotes = _numVotes;
}

Node::Node(int _n, bool _leaf, Node* _ptr2next) {
    n = _n;
    ptr2next = _ptr2next;
    leaf = _leaf;
    num_keys = 0;
    keys.reserve(n);
    child_ptr.reserve(n+1);
    record_ptr.reserve(n);
}

// returns node pointer if a new node is created. otherwise, returns NULL
Node* Node::insert_record(int k, Record* rptr) {
    // terminal state: leaf node
    if (leaf) {
        // check if node is full, need to split
        if (num_keys == n) {
            // create temporary vectors to hold keys and pointers
            vector<int> temp_keys = keys;
            temp_keys.push_back(k);
            vector<Record*> temp_record_ptr = record_ptr;
            temp_record_ptr.push_back(rptr);

            // ensure keys are sorted
            int i = num_keys;
            while (i > 0 && temp_keys[i] < temp_keys[i-1]) {
                swap(temp_keys[i], temp_keys[i - 1]);
                swap(temp_record_ptr[i], temp_record_ptr[i-1]);
                i--;
            }

            // split_index is the number of keys in the first node
            // split_index = -1 if the whole vector is the same, need overflow block
            int split_index = split_check_duplicates(temp_keys);

            if (split_index == -1) {
                // create an overflow node
                Node* new_node = new Node(n, leaf=true, ptr2next);
                new_node->keys.push_back(k);
                new_node->record_ptr.push_back(rptr);
                new_node->num_keys = 1;
                ptr2next = new_node;
                return NULL;
            }

            // update current node with first set of keys and record pointers
            keys.resize(split_index);
            copy(temp_keys.begin(), temp_keys.begin() + split_index, keys.begin());
            record_ptr.resize(split_index);
            copy(temp_record_ptr.begin(), temp_record_ptr.begin() + split_index, record_ptr.begin());
            num_keys = split_index;

            // create a new node for the second set of keys and record pointers
            Node* new_node = new Node(n, leaf=true, ptr2next);

            int new_node_keys = temp_keys.size() - split_index;
            new_node->keys.resize(new_node_keys);
            copy(temp_keys.begin() + split_index, temp_keys.end(), new_node->keys.begin());
            new_node->record_ptr.resize(new_node_keys);
            copy(temp_record_ptr.begin() + split_index, temp_record_ptr.end(), new_node->record_ptr.begin());
            new_node->num_keys = new_node_keys;

            // update next pointer for current node
            ptr2next = new_node;
            return new_node;
        }

        // not full, simply insert
        else {
            keys.push_back(k);
            record_ptr.push_back(rptr);

            num_keys++;
            // sort inserted key into position
            int i = num_keys - 1;
            while (i > 0 && keys[i] < keys[i-1]) {
                swap(keys[i], keys[i - 1]);
                swap(record_ptr[i], record_ptr[i - 1]);
                i--;
            }

            return NULL;
        }
    } 
    
    else {
        // recursive case for internal nodes
        // find correct child pointer
        int i = 0;
        while (i < num_keys && k >= keys[i]) {
            i++;
        }
        // recursive call. returns pointer if a node is created, otherwise returns NULL
        Node* new_node = child_ptr[i]->insert_record(k, rptr);

        // if a new node is created by splitting child nodes
        if (new_node) {
            // current node is full, need to split
            if (num_keys == n) {
                // create temporary vectors to hold keys and child pointers
                vector<int> temp_keys = keys;
                temp_keys.push_back(new_node->smallest());
                vector<Node*> temp_child_ptr = child_ptr;
                temp_child_ptr.push_back(new_node);

                // ensure keys are sorted
                int i = n;
                while (i > 0 && temp_keys[i] < temp_keys[i-1]) {
                    swap(temp_keys[i], temp_keys[i - 1]);
                    swap(temp_child_ptr[i + 1], temp_child_ptr[i]);
                    i--;
                }
                // check if smaller than first child node
                if (i == 0) {
                    if (child_ptr.at(0)->keys.at(0) > keys.at(0)) {
                        keys.at(0) = child_ptr[0]->smallest();
                        swap(child_ptr.at(0), child_ptr.at(1));
                        // update the smallest value upwards
                    }
                }

                size_t half_size = (temp_keys.size() - 1)/ 2;

                // update current node with first half of keys and child pointers
                int first_half_keys = temp_keys.size() - 1 - half_size;
                keys.resize(first_half_keys);
                copy(temp_keys.begin(), temp_keys.begin() + first_half_keys, keys.begin());
                child_ptr.resize(first_half_keys + 1);
                copy(temp_child_ptr.begin(), temp_child_ptr.begin() + first_half_keys + 1, child_ptr.begin());
                num_keys = first_half_keys;

                // create a new node for the second half of keys and child pointers
                Node* new_node = new Node(n, leaf, ptr2next);

                new_node->keys.resize(half_size);
                copy(temp_keys.begin() + first_half_keys + 1, temp_keys.end(), new_node->keys.begin());
                new_node->child_ptr.resize(half_size + 1);
                copy(temp_child_ptr.begin() + first_half_keys + 1, temp_child_ptr.end(), new_node->child_ptr.begin());
                new_node->num_keys = half_size;

                // update next pointer for current node
                ptr2next = new_node;
                return new_node;
            }

            // current node has space for the new node
            else {
                keys.push_back(new_node->smallest());
                child_ptr.push_back(new_node);

                num_keys++;
                // sort inserted key into position
                int i = num_keys - 1;
                while (i > 0 && keys[i] < keys[i-1]) {
                    swap(keys[i], keys[i - 1]);
                    swap(child_ptr[i + 1], child_ptr[i]);
                    i--;
                }
                // check if smaller than first child node
                if (i == 0) {
                    if (child_ptr.at(0)->keys.at(0) > keys.at(0)) {
                        keys.at(0) = child_ptr.at(0)->keys.at(0);
                        swap(child_ptr.at(0), child_ptr.at(1));
                    }
                }
                return NULL;
            }
        }
        else return NULL;
    }
}

int Node::smallest() {
    if (!leaf) {
        return child_ptr.at(0)->smallest();
    }
    else {
        return keys[0];
    }
}

int Node::split_check_duplicates(vector<int> keys) {
    int size = keys.size();
    int split_index = size - (size / 2);
    for (int step = 0; step < size - 1; step++) {
        // split_index is the size of the first split
        // for a vector of size 4 (even), split_index tries splits in this order: 2, 1, 3
        // for a vector of size 5 (odd ), split_index tries splits in this order: 3, 2, 4, 1
        if (step % 2 == 0)
            split_index += step;
        else
            split_index -= step;
        // test if values are different at split_index
        if (keys[split_index - 1] != keys[split_index]) {
            // if values are different, return
            return split_index;
        }
    }
    // if no split works (all the same key), must use overflow block
    return -1;
}

bool Node::printRecord(int min, int max) {
    bool printed = false;
    for (int i = 0; i < num_keys; i++) {
        if (keys[i] >= min && keys[i] <= max) {
            cout << "tconst: " << record_ptr[i]->tconst;
            cout << ", average rating: " << record_ptr[i]->averageRating;
            cout << ", numVotes: " << record_ptr[i]->numVotes << endl;
            printed = true;
        }
        else if (keys[i] > max)
            return printed;
    }
    // keep calling next until they return
    if (ptr2next != NULL)
        printed = ptr2next->printRecord(min, max);
    return printed;
}

void Node::search(int min, int max) {
    int i = 0;
    if (leaf) {
        if (!printRecord(min, max)) {
            cout << "Record not found" << endl;
        }
    }
    
    else {
        while (i < num_keys && min >= keys[i]) {
            i++;
        }
        return child_ptr[i]->search(min, max);
    }
}