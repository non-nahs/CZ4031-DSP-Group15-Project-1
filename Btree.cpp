#include "Btree.h"
#include "memory.h"
using namespace std;


BTree::BTree(int _n) {
    n = _n;
    root = new Node(_n);
    root->leaf = true;
}

void BTree::insert(float k, Record* rptr) {
	// Function to insert a key in the tree
    Node* new_node = root->insertRecord(k, rptr);
    if (new_node) {
        Node* new_root = new Node(n);
        // new_node will always be the bigger node
        new_root->child_ptr.push_back(root);
        new_root->child_ptr.push_back(new_node);
        new_root->keys.push_back(new_node->smallest());
        new_root->num_keys = 1;
        root = new_root;
    }
}
void BTree::deleteKey(float k)
{
    // Call the delete_record function on the root node
    Node *copyOfRoot = root;
    Node *mergedNode = root->delete_record(k, root);
    root->adjustSmallest(root, k);
    cout << "root size:" << root->keys.size() << endl;
    cout << "child ptr size:" << root->child_ptr.size() << endl;
    if (mergedNode && root->keys.size() == 0)
    {
        // If the root node is empty after deletion, update the root
        root = mergedNode;
        cout << "New root size:" << mergedNode->keys.size() << endl;
    }
}
void BTree::search(int min, int max) {
    if (max == -1) max = min;
    root->search(min, max);
}
vector<float> BTree::searchAverageRating(int min, int max) {
    if (max == -1) max = min;
    vector<float> average_rating;
    root->searchAverageRating(min, max, average_rating);
    return average_rating;
}

void BTree::printTree() {
    printNode(root, 0, 0);
}

void BTree::printNode(Node* node, int level, int child_id) {
    if (node != nullptr) {
        // Print the keys of the current node
        // added child node information for debugging
        cout << "Level " << level << " (child " << child_id << ") : ";
        for (int i = 0; i < node->num_keys; ++i) {
            cout << node->keys[i] << " ";
        }
        cout << endl;

        // Recursively print the child nodes
        if (!node->leaf) {
            for (int i = 0; i < node->num_keys + 1; ++i) {
                printNode(node->child_ptr[i], level + 1, i);
            }
        }
        else if (node->leaf) {
            for (int i = 0; i < node->num_keys; ++i) {
                if (node->child_ptr[i]) {
                    int num_duplicates = countDuplicateKeys(node->child_ptr[i]);
                    cout << num_duplicates << " duplicate(s) of key " << node->keys[i] << endl;
                }
            }
        }
    }
}

int BTree::countDuplicateKeys(Node* node) {
    if (node != nullptr) {
        // recursively add keys in current node
        return node->num_keys + countDuplicateKeys(node->ptr2next);
    }
    else 
        // terminal case: node does not exist, 0 keys
        return 0;
}

int BTree::countNodes(Node* node) {
    if (node != nullptr) {
        int counter = 0;

        // Recursively count the child nodes
        if (!node->leaf) {
            for (int i = 0; i < node->num_keys + 1; ++i) {
                counter += countNodes(node->child_ptr[i]);
            }
            // count current node
            return counter + 1;
        }
        else if (node->leaf) {
            for (int i = 0; i < node->num_keys; ++i) {
                // check for overflow nodes
                if (node->child_ptr[i]) {
                    Node* cur = node->child_ptr[i];
                    while (cur) {
                        counter++;
                        cur = cur->ptr2next;
                    }
                }
            }
            // count current leaf node 
            return counter + 1;
        }
    }
    return 0;
}

int BTree::countLevels(Node* node) {
    if (node != nullptr) {
        // Recursively count the child nodes
        if (!node->leaf) {
            return countLevels(node->child_ptr[0]) + 1;
        }
        else if (node->leaf) {
            // check for presence of overflow nodes due to duplicate keys
            Node* cur = node;
            while (cur) {
                for (int i = 0; i < node->num_keys; ++i) {
                    if (node->child_ptr[i]) {
                        // leaf level + overflow level
                        return 2;
                    }
                }
                cur = cur->ptr2next;
            }  
            // no duplicate keys
            return 1;
        }
    }
    return 0;
}

void BTree::printRoot()
{
    if (root != nullptr)
    {
        cout << "Level " << 0 << ":";
        for (int i = 0; i < root->num_keys; ++i)
        {
            cout << root->keys[i] << " ";
        }
        cout << endl;
    }
}