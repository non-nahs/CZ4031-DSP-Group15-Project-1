#include "Btree.h"
using namespace std;


BTree::BTree(int _t) {
    root = new Node(_t);
    root->leaf = true;
}

void BTree::insert(float k, Record* rptr) {
	// Function to insert a key in the tree
    Node* new_node = root->insert_record(k, rptr);
    if (new_node) {
        Node* new_root = new Node(3);
        // new_node will always be the bigger node
        new_root->child_ptr.push_back(root);
        new_root->child_ptr.push_back(new_node);
        new_root->keys.push_back(new_node->smallest());

        new_root->n = 1;
        root = new_root;
    }
}

Node* BTree::search(float k) {
    return (root == nullptr) ? nullptr : root->search(k);
}

void BTree::printTree() {
    printNode(root, 0);
}

void BTree::printNode(Node* node, int level) {
    if (node != nullptr) {
        // Print the keys of the current node
        cout << "Level " << level << ": ";
        for (int i = 0; i < node->n; ++i) {
            cout << node->keys[i] << " ";
        }
        cout << endl;

        // Recursively print the child nodes
        for (int i = 0; i <= node->n; ++i) {
            printNode(node->child_ptr[i], level + 1);
        }
    }
}
