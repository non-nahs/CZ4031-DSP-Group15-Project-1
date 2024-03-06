#include "Btree.h"
using namespace std;


BTree::BTree(int _n) {
    root = new Node(_n);
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

void BTree::printTree() {
    printNode(root, 0);
}

void BTree::printNode(Node* node, int level) {
    if (node != nullptr) {
        // Print the keys of the current node
        cout << "Level " << level << ": ";
        for (int i = 0; i < node->num_keys; ++i) {
            cout << node->keys[i] << " ";
        }
        cout << endl;

        // Recursively print the child nodes
        for (int i = 0; i <= node->num_keys; ++i) {
            printNode(node->child_ptr[i], level + 1);
        }
    }
}
