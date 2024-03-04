#include "Node.h"
#include <algorithm>
#include <vector>
using namespace std;

Record::Record(string _tconst, float _averageRating, int _numVotes) {
    tconst = _tconst;
    averageRating = _averageRating;
    numVotes = _numVotes;
}

Node::Node(int _t, bool _leaf, Node* _ptr2next) {
    t = _t;
    ptr2next = _ptr2next;
    leaf = _leaf;
    n = 0;
    keys.reserve(t);
    child_ptr.reserve(t+1);
    record_ptr.reserve(t);
    // keys.resize(MAX_KEYS);
    // child_ptr.resize(MAX_KEYS + 1);
}

// returns node pointer if a new node is created. otherwise, returns NULL
Node* Node::insert_record(float k, Record* rptr) {
    // terminal state: leaf node
    if (leaf) {
        // check if node is full, need to split
        if (n == t) {
            // create temporary vectors to hold keys and pointers
            vector<float> temp_keys = keys;
            temp_keys.push_back(k);
            vector<Record*> temp_record_ptr = record_ptr;
            temp_record_ptr.push_back(rptr);

            // ensure keys are sorted
            int i = n;
            while (i > 0 && temp_keys[i] < temp_keys[i-1]) {
                swap(temp_keys[i], temp_keys[i - 1]);
                swap(temp_record_ptr[i], temp_record_ptr[i-1]);
                i--;
            }
            size_t half_size = temp_keys.size() / 2;

            // update current node with first half of keys and record pointers
            int num_keys = temp_keys.size() - half_size;
            keys.resize(num_keys);
            copy(temp_keys.begin(), temp_keys.begin() + num_keys, keys.begin());
            record_ptr.resize(num_keys);
            copy(temp_record_ptr.begin(), temp_record_ptr.begin() + num_keys, record_ptr.begin());
            n = num_keys;

            // create a new node for the second half of keys and record pointers
            Node* new_node = new Node(t, leaf=true, ptr2next);

            new_node->keys.resize(half_size);
            copy(temp_keys.begin() + num_keys, temp_keys.end(), new_node->keys.begin());
            new_node->record_ptr.resize(half_size);
            copy(temp_record_ptr.begin() + num_keys, temp_record_ptr.end(), new_node->record_ptr.begin());
            new_node->n = half_size;

            // update next pointer for current node
            ptr2next = new_node;
            return new_node;
        }

        // not full, simply insert
        else {
            keys.push_back(k);
            record_ptr.push_back(rptr);

            n++;
            // sort inserted key into position
            int i = n - 1;
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
        while (i < n && k >= keys[i]) {
            i++;
        }
        // recursive call. returns pointer if a node is created, otherwise returns NULL
        Node* new_node = child_ptr[i]->insert_record(k, rptr);

        // if a new node is created by splitting child nodes
        if (new_node) {
            // current node is full, need to split
            if (n == t) {
                // create temporary vectors to hold keys and child pointers
                vector<float> temp_keys = keys;
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
                    }
                }

                size_t half_size = (temp_keys.size() - 1)/ 2;

                // update current node with first half of keys and child pointers
                int num_keys = temp_keys.size() - 1 - half_size;
                keys.resize(num_keys);
                copy(temp_keys.begin(), temp_keys.begin() + num_keys, keys.begin());
                child_ptr.resize(num_keys + 1);
                copy(temp_child_ptr.begin(), temp_child_ptr.begin() + num_keys + 1, child_ptr.begin());
                n = num_keys;

                // create a new node for the second half of keys and child pointers
                Node* new_node = new Node(t, leaf, ptr2next);

                new_node->keys.resize(half_size);
                copy(temp_keys.begin() + num_keys + 1, temp_keys.end(), new_node->keys.begin());
                new_node->child_ptr.resize(half_size + 1);
                copy(temp_child_ptr.begin() + num_keys + 1, temp_child_ptr.end(), new_node->child_ptr.begin());
                new_node->n = half_size;

                // update next pointer for current node
                ptr2next = new_node;
                return new_node;
            }

            // current node has space for the new node
            else {
                keys.push_back(new_node->keys[0]);
                child_ptr.push_back(new_node);

                n += 1;
                // sort inserted key into position
                int i = n - 1;
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

float Node::smallest() {
    if (!leaf) {
        return child_ptr.at(0)->smallest();
    }
    else {
        return keys[0];
    }
}

Node* Node::search(float k) {
    int i = 0;
    if (leaf) {
        while (i < n && k > keys[i]) {
            i += 1;
        }
        if (k == keys[i]) {
            printf("%s", record_ptr[i]->tconst.c_str());
            return new Node(3);
        }
        else return NULL;
    }
    
    else {
        while (i < n && k >= keys[i]) {
            i += 1;
        }
        return child_ptr[i]->search(k);
    }
}