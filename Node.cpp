#include "Node.h"
#include "memory.h"
#include <algorithm>
#include <vector>
using namespace std;

// Record::Record(string _tconst, float _averageRating, int _numVotes) {
//     tconst = _tconst;
//     averageRating = _averageRating;
//     numVotes = _numVotes;
// }

Node::Node(int _n, bool _leaf, Node* _ptr2next) {
    n = _n;
    ptr2next = _ptr2next;
    leaf = _leaf;
    num_keys = 0;
    keys.reserve(n);
    child_ptr.reserve(n + 1);
    record_ptr.reserve(n);
}

// returns node pointer if a new node is created. otherwise, returns NULL
Node* Node::insertRecord(int k, Record* rptr) {
    // terminal state: leaf node
    if (leaf) {
        // check if key already exists
        for (int i = 0; i < num_keys; i++) {
            if (k == keys[i]) {
                // key already exists. handle duplicate keys
                if (!child_ptr[i]) {
                    Node* overflow_node = new Node(n);
                    overflow_node->keys.push_back(k);
                    overflow_node->record_ptr.push_back(rptr);
                    overflow_node->num_keys = 1;
                    child_ptr[i] = overflow_node;
                }
                else {
                    // insert duplicate into existing overflow node(s)
                    child_ptr[i]->insertDuplicate(k, rptr);
                }
                return NULL;
            }
        }
        // check if node is full, need to split
        if (num_keys == n) {
            // create temporary vectors to hold keys and pointers
            vector<int> temp_keys = keys;
            temp_keys.push_back(k);
            vector<Record*> temp_record_ptr = record_ptr;
            temp_record_ptr.push_back(rptr);
            vector<Node*> temp_child_ptr = child_ptr;
            temp_child_ptr.push_back(NULL);

            // ensure keys are sorted
            int i = num_keys;
            while (i > 0 && temp_keys[i] < temp_keys[i-1]) {
                swap(temp_keys[i], temp_keys[i - 1]);
                swap(temp_record_ptr[i], temp_record_ptr[i - 1]);
                swap(temp_child_ptr[i], temp_child_ptr[i - 1]);
                i--;
            }

            int split_index = temp_keys.size() - (temp_keys.size() / 2);

            // update current node with first set of keys and record pointers
            keys.resize(split_index);
            copy(temp_keys.begin(), temp_keys.begin() + split_index, keys.begin());
            record_ptr.resize(split_index);
            copy(temp_record_ptr.begin(), temp_record_ptr.begin() + split_index, record_ptr.begin());
            child_ptr.resize(split_index);
            copy(temp_child_ptr.begin(), temp_child_ptr.begin() + split_index, child_ptr.begin());
            num_keys = split_index;

            // create a new node for the second set of keys and record pointers
            Node* new_node = new Node(n, leaf=true, ptr2next);

            int new_node_keys = temp_keys.size() - split_index;
            new_node->keys.resize(new_node_keys);
            copy(temp_keys.begin() + split_index, temp_keys.end(), new_node->keys.begin());
            new_node->record_ptr.resize(new_node_keys);
            copy(temp_record_ptr.begin() + split_index, temp_record_ptr.end(), new_node->record_ptr.begin());
            new_node->child_ptr.resize(new_node_keys);
            copy(temp_child_ptr.begin() + split_index, temp_child_ptr.end(), new_node->child_ptr.begin());
            new_node->num_keys = new_node_keys;

            // update next pointer for current node
            ptr2next = new_node;
            return new_node;
        }

        // not full, simply insert
        else {
            keys.push_back(k);
            record_ptr.push_back(rptr);
            child_ptr.push_back(NULL);

            num_keys++;
            // sort inserted key into position
            int i = num_keys - 1;
            while (i > 0 && keys[i] < keys[i-1]) {
                swap(keys[i], keys[i - 1]);
                swap(record_ptr[i], record_ptr[i - 1]);
                swap(child_ptr[i], child_ptr[i - 1]);
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
        Node* new_node = child_ptr[i]->insertRecord(k, rptr);

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
// check if the node has overflow node
bool Node::checkOverFlowNode(Node *currNode, float target)
{
    for (int i = 0; i < currNode->num_keys; i++)
    {
        cout << "i:" << i << endl;
        cout << "currNode->keys[i]:" << currNode->keys[i] << endl;
        if (currNode->keys[i] == target && currNode->child_ptr[i])
        {

            cout << "over flow node true" << endl;
            return true;
        }
    }
    cout << "over flow node false" << endl;
    return false;
}
// if a duplicate inside the overflow node, delete the duplicate key
void Node::delete_duplicate(Node *currNode, int k, int kIndex)
{
    cout << "over flow node size:" << currNode->child_ptr[kIndex]->num_keys << endl;
    if (!currNode->child_ptr[kIndex]->keys.empty())
    {
        currNode->child_ptr[kIndex] = NULL;

        // currNode->child_ptr[kIndex]->keys.erase(currNode->child_ptr[kIndex]->keys.begin());
        // currNode->child_ptr[kIndex]->record_ptr.erase(currNode->child_ptr[kIndex]->record_ptr.begin());
        // // Decrement the number of keys in the overflow node
        // currNode->child_ptr[kIndex]->num_keys--;
        // if (currNode->child_ptr[kIndex]->num_keys == 0)
        // {
        //     currNode->child_ptr[kIndex] = NULL;
        // }
    }
}
void Node::adjustSmallest(Node *currNode, float k)
{
    if (!currNode)
    {
        return;
    }
    if (currNode->leaf)
    {
        return;
    }
    if (!currNode->leaf)
    {
        int i = 0;
        cout << "k:" << k << endl;
        cout << "currNode child pointer size:" << currNode->child_ptr.size() << endl;
        currNode->printKeys();

        while (i < currNode->child_ptr.size() && k >= currNode->keys[i])
        {
            if (i == currNode->child_ptr.size() - 1)
            {
                break;
            }
            i++;
            cout << "currNode keys[i] after increment:" << currNode->keys[i] << endl;
        }
        currNode->printKeys();

        // i = findChildPtrIndex()
        cout << "i:" << i << endl;
        adjustSmallest(currNode->child_ptr[i], k);

        for (int i = 0; i < currNode->child_ptr.size() - 1; ++i)
        {
            currNode->keys[i] = currNode->child_ptr[i + 1]->smallest();
            cout << "smallest:" << currNode->child_ptr[i + 1]->smallest() << endl;
        }
    }
}
int Node::smallest()
{
    if (!leaf)
    {
        return child_ptr.at(0)->smallest();
    }
    else
    {
        return keys[0];
    }
}

int Node::index_inside_node(float k)
{
    for (int i = 0; i < n; ++i)
    {
        if (keys[i] == k)
        {
            return i;
        }
    }
    return -1;
}

// adjust child pointers after deletion in the leaf node
void Node::shift_childptr(Node *currNode, int index)
{
    cout << "shifting***********" << endl;
    // currNode->printKeys();
    // cout << "check child pointer size:" << currNode->child_ptr.size() << endl;

    // cout << "num_keys:" << currNode->num_keys << endl;
    // cout << "index:" << index << endl;
    // checkOverFlowNode(currNode, 117);
    vector<Node *> temp_child_ptr;
    for (int i = index + 1; i < currNode->child_ptr.size(); ++i)
    {
        temp_child_ptr.push_back(currNode->child_ptr[i]);
    }
    child_ptr.insert(child_ptr.begin() + index, temp_child_ptr.begin(), temp_child_ptr.end());
    int numToRemove = child_ptr.size() - n;
    for (int i = 0; i < numToRemove; ++i)
    {
        child_ptr.pop_back();
    }
    cout << "check child pointer size:" << currNode->child_ptr.size() << endl;
}
Node *Node::delete_record(float k, Node *parentNode)
{
    // Find the index of the key in the current node
    int index = index_inside_node(k);
    cout << "index inside node:" << index << endl;
    // If the key is found in the current node
    if (index != -1)
    {

        if (leaf)
        {
            cout << "index:" << index << endl;
            // check for over flow node, if yes then simply delete the duplicate
            
            // cout << "checkOverFlowNode:" << checkOverFlowNode(this) << endl;
            // If the node is a leaf, simply delete the key
            
                if (checkOverFlowNode(this, k))
                {
                    delete_duplicate(this, k, index);
                }
                keys.erase(keys.begin() + index);
                record_ptr.erase(record_ptr.begin() + index);
                shift_childptr(this, index);
                num_keys--;
            
            float parentKey = findParentKey(parentNode, this);
            cout << parentKey << endl;
            cout << "parentNode:" << parentNode << endl;
            // Check for underflow
            if (num_keys < (n + 1) / 2 && parentNode)
            {
                // cout << "Underflow occurred" << endl;
                // Borrow from left sibling if possible
                Node *leftSibling = findLeftSibling(parentNode, parentKey, this);
                if (leftSibling && leftSibling->num_keys > (n + 1) / 2)
                {
                    borrow_left(leftSibling, parentNode, parentKey, this);
                    return nullptr;
                }
                // cout << "Cannot borrow from left sibiling" << endl;
                // Borrow from right sibling if possible
                Node *rightSibling = findRightSibling(parentNode, parentKey, this);
                // cout << "right sibling size:" << rightSibling->n << endl;
                if (rightSibling && rightSibling->num_keys > (n + 1) / 2)
                {
                    cout << "borrow from right" << endl;
                    return borrow_right(rightSibling, parentNode, parentKey, this);
                }
                // cout << "Cannot borrow from right sibiling" << endl;
                // Merge with left or right sibling
                if (leftSibling)
                {
                    cout << "Merge with left sibiling" << endl;
                    return merge_left(leftSibling, parentNode, parentKey);
                }
                else
                {
                    cout << "Merge with right sibiling" << endl;
                    Node *mergedNode = merge_right(rightSibling, parentNode, parentKey);
                    cout << "Merged node size in delete_record:" << mergedNode->keys.size() << endl;
                    return mergedNode;
                }
            }
        }
        else
        {
            int i = 0;
            while (i < num_keys && k >= keys[i])
            {
                i++;
            }
            Node *childNode = child_ptr[i];
            // Node *mergedNode = childNode->delete_record(k, this);
            Node *deletedNode = childNode->delete_record(k, this);
            int index = index_inside_node(k);
            float parentKey = findParentKey(parentNode, this);
            cout << parentKey << endl;
            float parentKeyIndex = findParentKeyIndex(parentNode, parentKey);
            // cout << "parentNode:" << parentNode << endl;
            // Check for underflow after deletion in the child node
            if (parentNode->keys.size() == 0)
            {
                return deletedNode;
            }
            if (deletedNode && num_keys < n / 2 && parentNode)
            {
                Node *leftSiblingCurr = findLeftSibling(parentNode, parentKey, this);
                Node *rightSiblingCurr = findRightSibling(parentNode, parentKey, this);
                if (leftSiblingCurr && leftSiblingCurr->num_keys > n / 2)
                {
                    return borrow_left(leftSiblingCurr, parentNode, parentKey, this);
                }
                if (rightSiblingCurr && rightSiblingCurr->num_keys > n / 2)
                {
                    cout << "borrowing from right for internal node" << endl;
                    return borrow_right(rightSiblingCurr, parentNode, parentKey, this);
                }
                if (parentNode->num_keys > 1 && leftSiblingCurr)
                {
                    // leftSiblingCurr->printKeys();
                    cout << "parentKeyIndex:" << parentKeyIndex << endl;
                    leftSiblingCurr->keys.push_back(deletedNode->keys[0]);
                    leftSiblingCurr->child_ptr.push_back(deletedNode);
                    leftSiblingCurr->num_keys++;
                    // cout << "left sibling child pointer size:" << leftSiblingCurr->child_ptr.size() << endl;

                    // parentNode->keys.pop_back();
                    // parentNode->child_ptr.pop_back();
                    parentNode->keys.erase(parentNode->keys.begin() + parentKeyIndex);
                    parentNode->child_ptr.erase(parentNode->child_ptr.begin() + parentKeyIndex + 1);
                    parentNode->num_keys--;
                    // // cout << "u"
                    // deletedNode->findParentKey(parentNode,deletedNode);
                    // cout << "current child pointer size:" << this->child_ptr.size() << endl;
                    // child_ptr.pop_back();
                    // cout << "current child pointer size:" << this->child_ptr.size() << endl;
                    // parentNode->printKeys();
                    return deletedNode;
                }
                if (parentNode->num_keys > 1 && rightSiblingCurr)
                {
                    rightSiblingCurr->keys.insert(rightSiblingCurr->keys.begin(), parentKey);
                    rightSiblingCurr->child_ptr.insert(rightSiblingCurr->child_ptr.begin(), deletedNode);
                    cout << "print keys for right sibling curr:" << endl;
                    rightSiblingCurr->printKeys();
                    rightSiblingCurr->num_keys++;
                    parentNode->keys.erase(parentNode->keys.begin());
                    parentNode->child_ptr.erase(parentNode->child_ptr.begin());
                    parentNode->num_keys--;
                    return deletedNode;
                }
                if (parentNode->keys.size() == 1)
                {
                    cout << "merging for internal node " << endl;
                    int childIndexToCurr = findChildPtrIndex(parentNode, this);
                    parentNode->child_ptr[childIndexToCurr] = deletedNode;
                    Node *currNode = deletedNode;
                    cout << "index of current internal node in parent (found in internal node):" << childIndexToCurr << endl;
                    Node *leftSiblingNew = findLeftSibling(parentNode, parentKey, currNode);

                    if (leftSiblingNew)
                    {
                        int leftSiblingIndex = findChildPtrIndex(parentNode, leftSiblingNew);
                        cout << "index of left sibling node in parent (internal found):" << leftSiblingIndex << endl;
                        cout << "left sibling size (internal found):" << leftSiblingNew->keys.size() << endl;
                        for (int i = 0; i < leftSiblingNew->child_ptr.size(); i++)
                        {
                            cout << "left sibling i:" << i << endl;
                            if (i == leftSiblingNew->child_ptr.size() - 1)
                            {
                                parentNode->child_ptr[leftSiblingIndex + i] = leftSiblingNew->child_ptr[i];
                            }
                            else
                            {
                                // parentNode->child_ptr[leftSiblingIndex + i] = leftSibling->child_ptr[i];
                                parentNode->child_ptr.insert(parentNode->child_ptr.begin() + leftSiblingIndex + i, leftSiblingNew->child_ptr[i]);
                                cout << "left sibling keys[i]:" << leftSiblingNew->keys[i] << endl;
                                parentNode->keys.insert(parentNode->keys.begin() + i, leftSiblingNew->keys[i]);
                                parentNode->num_keys++;
                            }
                        }
                    }
                    else
                    {
                        Node *rightSiblingNew = findRightSibling(parentNode, parentKey, currNode);
                        // cout << "right sibling size:"
                        int rightSiblingIndex = findChildPtrIndex(parentNode, rightSiblingNew);
                        cout << "index of right sibling node in parent:" << rightSiblingIndex << endl;

                        if (rightSiblingNew)
                        {
                            for (int i = 0; i < rightSiblingNew->child_ptr.size(); i++)
                            {
                                if (rightSiblingIndex + i == parentNode->child_ptr.size())
                                {
                                    // cout << "right sibling child pointer key:" << rightSibling->child_ptr[i]->keys[0] << endl;
                                    parentNode->keys.push_back(rightSiblingNew->child_ptr[i]->keys[0]);
                                    parentNode->child_ptr.push_back(rightSiblingNew->child_ptr[i]);
                                    parentNode->num_keys++;
                                }
                                else
                                {
                                    parentNode->child_ptr[rightSiblingIndex + i] = rightSiblingNew->child_ptr[i];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        // Key not found in the current node, continue searching in child nodes
        if (!leaf)
        {
            int i = 0;
            while (i < num_keys && k >= keys[i])
            {
                i++;
            }
            Node *childNode = child_ptr[i];
            // Node *mergedNode = childNode->delete_record(k, this);
            Node *deletedNode = childNode->delete_record(k, this);
            float parentKey = findParentKey(parentNode, this);
            cout << "parent key:" << parentKey << endl;
            float parentKeyIndex = findParentKeyIndex(parentNode, parentKey);

            if (parentNode->keys.size() == 0)
            {
                return deletedNode;
            }
            if (deletedNode && num_keys < n / 2 && parentNode)
            {

                Node *leftSiblingCurr = findLeftSibling(parentNode, parentKey, this);
                Node *rightSiblingCurr = findRightSibling(parentNode, parentKey, this);
                if (leftSiblingCurr && leftSiblingCurr->num_keys > n / 2)
                {
                    return borrow_left(leftSiblingCurr, parentNode, parentKey, this);
                }
                if (rightSiblingCurr && rightSiblingCurr->num_keys > n / 2)
                {
                    cout << "borrowing from right for internal node" << endl;
                    return borrow_right(rightSiblingCurr, parentNode, parentKey, this);
                }
                if (parentNode->num_keys > 1 && leftSiblingCurr)
                {
                    // leftSiblingCurr->printKeys();
                    leftSiblingCurr->keys.push_back(deletedNode->keys[0]);
                    leftSiblingCurr->child_ptr.push_back(deletedNode);
                    leftSiblingCurr->num_keys++;
                    // cout << "left sibling child pointer size:" << leftSiblingCurr->child_ptr.size() << endl;

                    parentNode->keys.erase(parentNode->keys.begin() + parentKeyIndex);
                    parentNode->child_ptr.erase(parentNode->child_ptr.begin() + parentKeyIndex + 1);
                    parentNode->num_keys--;
                    // // cout << "u"
                    // deletedNode->findParentKey(parentNode,deletedNode);
                    // cout << "current child pointer size:" << this->child_ptr.size() << endl;
                    // child_ptr.pop_back();
                    // cout << "current child pointer size:" << this->child_ptr.size() << endl;
                    // parentNode->printKeys();
                    return deletedNode;
                }
                if (parentNode->num_keys > 1 && rightSiblingCurr)
                {
                    // deletedNode->printKeys();
                    rightSiblingCurr->keys.insert(rightSiblingCurr->keys.begin(), parentKey);
                    rightSiblingCurr->child_ptr.insert(rightSiblingCurr->child_ptr.begin(), deletedNode);
                    cout << "print keys for right sibling curr:" << endl;
                    rightSiblingCurr->printKeys();
                    rightSiblingCurr->num_keys++;
                    parentNode->keys.erase(parentNode->keys.begin());
                    parentNode->child_ptr.erase(parentNode->child_ptr.begin());
                    parentNode->num_keys--;
                    return deletedNode;
                }

                if (parentNode->num_keys == 1)
                {
                    int childIndexToCurr = findChildPtrIndex(parentNode, this);
                    parentNode->child_ptr[childIndexToCurr] = deletedNode;
                    Node *currNode = deletedNode;
                    cout << "index of current internal node in parent:" << childIndexToCurr << endl;
                    Node *leftSibling = findLeftSibling(parentNode, parentKey, currNode);

                    if (leftSibling)
                    {

                        int leftSiblingIndex = findChildPtrIndex(parentNode, leftSibling);
                        cout << "index of left sibling node in parent:" << leftSiblingIndex << endl;
                        cout << "left sibling size:" << leftSibling->keys.size() << endl;
                        for (int i = 0; i < leftSibling->child_ptr.size(); i++)
                        {
                            cout << "left sibling i:" << i << endl;
                            if (i == leftSibling->child_ptr.size() - 1)
                            {
                                parentNode->child_ptr[leftSiblingIndex + i] = leftSibling->child_ptr[i];
                            }
                            else
                            {
                                // parentNode->child_ptr[leftSiblingIndex + i] = leftSibling->child_ptr[i];
                                cout << "left sibling keys[i]:" << leftSibling->keys[i] << endl;
                                parentNode->child_ptr.insert(parentNode->child_ptr.begin() + leftSiblingIndex + i, leftSibling->child_ptr[i]);
                                parentNode->keys.insert(parentNode->keys.begin() + i, leftSibling->keys[i]);
                                parentNode->num_keys++;
                            }
                        }
                    }
                    else
                    {
                        Node *rightSibling = findRightSibling(parentNode, parentKey, currNode);
                        // cout << "right sibling size:"
                        int rightSiblingIndex = findChildPtrIndex(parentNode, rightSibling);
                        cout << "index of right sibling node in parent:" << rightSiblingIndex << endl;

                        if (rightSibling)
                        {
                            for (int i = 0; i < rightSibling->child_ptr.size(); i++)
                            {
                                if (rightSiblingIndex + i == parentNode->child_ptr.size())
                                {
                                    // cout << "right sibling child pointer key:" << rightSibling->child_ptr[i]->keys[0] << endl;
                                    parentNode->keys.push_back(rightSibling->child_ptr[i]->keys[0]);
                                    parentNode->child_ptr.push_back(rightSibling->child_ptr[i]);
                                    parentNode->num_keys++;
                                }
                                else
                                {
                                    parentNode->child_ptr[rightSiblingIndex + i] = rightSibling->child_ptr[i];
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    // cout << "returning null ptr " << endl;
    return nullptr;
}

int Node::findParentKeyIndex(Node *parentNode, float parentKey)
{
    for (int i = 0; i < parentNode->keys.size(); i++)
    {
        if (parentNode->keys[i] == parentKey)
        {
            return i;
        }
    }
    return -1;
}
float Node::findParentKey(Node *parentNode, Node *childNode)
{
    // Iterate through the child pointers of the parent node
    for (int i = 0; i < parentNode->child_ptr.size(); ++i)
    {
        // If the current child pointer matches the child node
        cout << "Parent keys:" << parentNode->keys[i] << endl;
        if (parentNode->child_ptr[i] == childNode)
        {
            if (i == 0)
            {
                return parentNode->keys[i];
            }
            else
            {
                return parentNode->keys[i - 1];
            }
            // cout << "ith child ptr:" << i << endl;
            // // Return the key at the same index in the parent node
            // return parentNode->keys[i]; // Assuming child_ptr[0] corresponds to keys[0]
        }
    }
    // If the child node is not found in the parent node's child pointers, return a default value
    return -1; // Or any other default value indicating that the parent key was not found
}

Node *Node::findRightSibling(Node *parentNode, float parentKey, Node *childNode)
{
    // Find the right sibling of the current node

    int childPtrIndex = 0;
    while (childPtrIndex < parentNode->child_ptr.size() && parentNode->child_ptr[childPtrIndex] != childNode)
    {
        childPtrIndex++;
    }
    cout << "child pointer index for find right sibling:" << childPtrIndex << endl;
    cout << "parent node child pointer size:" << parentNode->child_ptr.size() << endl;

    // if (childPtrIndex > 0 && childPtrIndex < parentNode->child_ptr.size())
    // {
    //     // Return the right sibling node
    //     return parentNode->child_ptr[childPtrIndex + 1];
    // }
    // else
    // {
    //     // If there's no right sibling, return nullptr
    //     return nullptr;
    // }
    // If there's no right sibling (last child), return nullptr
    if (childPtrIndex == parentNode->child_ptr.size() - 1)
    {
        cout << "returning null pointer for find right sibling" << endl;
        return nullptr;
    }

    // Otherwise, return the right sibling
    else
    {
        return parentNode->child_ptr[childPtrIndex + 1];
    }
}

Node *Node::findLeftSibling(Node *parentNode, float parentKey, Node *childNode)
{
    // Find the left sibling of the current node
    int childPtrIndex = 0;
    while (childPtrIndex < parentNode->child_ptr.size() && parentNode->child_ptr[childPtrIndex] != childNode)
    {
        childPtrIndex++;
    }
    cout << "child ptr index for left sibling of current node:" << childPtrIndex - 1 << endl;
    if (childPtrIndex > 0)
    {
        // Return the left sibling node
        return parentNode->child_ptr[childPtrIndex - 1];
    }
    else
    {
        // If there's no left sibling, return nullptr
        return nullptr;
    }
}

Node *Node::merge_left(Node *leftSibling, Node *parentNode, float parentKey)
{
    // Merge the current node with its left sibling
    int parentIndex = 0;
    while (parentIndex < parentNode->n && parentNode->keys[parentIndex] < parentKey)
    {
        parentIndex++;
    }

    // Find the index of the left sibling in the parent node
    int siblingIndex = parentIndex;
    cout << "siblingIndex:" << siblingIndex << endl;

    // Move keys and pointers from the left sibling to the current node
    keys.insert(keys.begin(), leftSibling->keys.begin(), leftSibling->keys.end());
    record_ptr.insert(record_ptr.begin(), leftSibling->record_ptr.begin(), leftSibling->record_ptr.end());
    child_ptr.insert(child_ptr.begin(), leftSibling->child_ptr.begin(), leftSibling->child_ptr.begin() + leftSibling->num_keys);
    num_keys += leftSibling->num_keys;
    // cout << "check child pointer size:" << child_ptr.size() << endl;
    int numToRemove = child_ptr.size() - n;
    for (int i = 0; i < numToRemove; ++i)
    {
        child_ptr.pop_back();
    }
    // cout << "check child pointer size afte delete:" << child_ptr.size() << endl;

    // cout << "checkOverflow node for keys[1]:" << checkOverFlowNode(this, keys[1]);

    cout << "size of n:" << num_keys << endl;
    leftSibling->printKeys();
    // Adjust parent pointers

    cout << "Parent key being erased: " << parentNode->keys[siblingIndex] << endl;
    parentNode->keys.erase(parentNode->keys.begin() + siblingIndex);
    // parentNode->printKeys();
    parentNode->child_ptr.erase(parentNode->child_ptr.begin() + siblingIndex);
    parentNode->num_keys--;
    cout << "size of n after adjusting parent pointers:" << parentNode->num_keys << endl;

    // Update next pointer if merging leaf nodes
    if (leaf && leftSibling->leaf)
    {
        ptr2next = leftSibling->ptr2next;
    }

    if (parentNode->keys.size() == 0)
    {
        // cout << "Deleting parent node" << endl;
        // delete parentNode;
        delete leftSibling;
        cout << "Size of merged node:" << keys.size() << endl;
        // findParentKey(parentNode, this);
        return this;
    }

    // Delete the left sibling node
    delete leftSibling;

    // Check for underflow in the parent node
    if (parentNode->num_keys < n / 2)
    {
        // Recursive call to handle underflow in the parent node
        return this;
    }
    if (parentNode->num_keys >= n / 2)
    {
        cout << "Size of merged node >=n/2:" << keys.size() << endl;
        return this;
    }

    return this;
}
Node *Node::merge_right(Node *rightSibling, Node *parentNode, float parentKey)
{
    // Find the index of the right sibling in the parent node
    int parentIndex = 0;
    while (parentIndex < parentNode->n && parentNode->keys[parentIndex] != parentKey)
    {
        parentIndex++;
    }
    cout << "Parent index:" << parentIndex << endl;
    cout << "Parent key:" << parentKey << endl;
    cout << "Parent node child_ptr size: " << parentNode->child_ptr.size() << endl;
    cout << "Parent node keys size: " << parentNode->keys.size() << endl;

    // Move keys and pointers from the current node to the right sibling
    rightSibling->keys.insert(rightSibling->keys.begin(), keys.begin(), keys.end());
    rightSibling->record_ptr.insert(rightSibling->record_ptr.begin(), record_ptr.begin(), record_ptr.end());
    rightSibling->child_ptr.insert(rightSibling->child_ptr.begin(), child_ptr.begin(), child_ptr.begin() + num_keys);

    if (!leaf)
    {
        // child_ptr.insert(child_ptr.end(), rightSibling->child_ptr.begin(), rightSibling->child_ptr.end());
        rightSibling->child_ptr.insert(rightSibling->child_ptr.end(), child_ptr.begin(), child_ptr.end());
    }
    // n += rightSibling->n;
    rightSibling->num_keys += num_keys;
    cout << "current node size:" << rightSibling->num_keys << endl;
    int numToRemove = child_ptr.size() - n;
    for (int i = 0; i < numToRemove; ++i)
    {
        child_ptr.pop_back();
    }
    // Delete the parent key from the parent node
    parentNode->keys.erase(parentNode->keys.begin() + parentIndex);
    parentNode->num_keys--;
    parentNode->child_ptr.erase(parentNode->child_ptr.begin() + parentIndex);
    cout << "ParentNode n:" << parentNode->num_keys << endl;
    cout << "ParentNode child pointer size:" << parentNode->child_ptr.size() << endl;
    // cout << "After deleting right sibling" << endl;
    // Update next pointer if merging leaf nodes
    if (leaf && rightSibling->leaf)
    {
        ptr2next = rightSibling->ptr2next;
    }
    // Delete the right sibling node
    delete this;
    // cout << "After updating current node" << endl;
    if (parentNode->keys.size() == 0)
    {
        // cout << "Deleting parent node" << endl;
        // delete parentNode;
        cout << "Size of merged node:" << rightSibling->keys.size() << endl;
        return rightSibling;
    }

    // cout << "Size of merged node:" << keys.size() << endl;

    // Check for underflow in the parent node
    if (parentNode->num_keys < n / 2)
    {
        cout << "Size of merged node:" << rightSibling->keys.size() << endl;

        // Recursive call to handle underflow in the parent node
        return parentNode->delete_record(parentNode->keys[0], parentNode);
    }
    if (parentNode->num_keys >= n / 2)
    {
        cout << "Size of merged node >=n/2:" << rightSibling->keys.size() << endl;
        return rightSibling;
    }
    cout << "Returning null pointer" << endl;
    return nullptr;
}

Node *Node::borrow_left(Node *leftSibling, Node *parentNode, float parentKey, Node *childNode)
{
    // Borrow a key from the left sibling
    // int parentIndex = 0;
    // while (parentIndex < parentNode->n && parentNode->keys[parentIndex] < parentKey)
    // {
    //     parentIndex++;
    // }

    int childPtrIndex = 0;
    while (childPtrIndex < parentNode->child_ptr.size() && parentNode->child_ptr[childPtrIndex] != childNode)
    {
        childPtrIndex++;
    }

    // Find the index of the left sibling in the parent node
    int siblingIndex = childPtrIndex - 1;

    // Move the last key from the left sibling to the parent node
    parentNode->keys[siblingIndex] = leftSibling->keys.back();
    parentNode->record_ptr[siblingIndex] = leftSibling->record_ptr.back();
    cout << "borrowed key:" << parentNode->keys[siblingIndex] << endl;
    int borrowedKey = parentNode->keys[siblingIndex];
    bool overflowNodeForBorrowed = checkOverFlowNode(leftSibling, borrowedKey);
    bool overFlowNodeForCurrFront = checkOverFlowNode(childNode, childNode->keys[0]);

    // Move the parent key down to the current node
    keys.insert(keys.begin(), parentNode->keys[siblingIndex]);
    record_ptr.insert(record_ptr.begin(), parentNode->record_ptr[siblingIndex]);
    if (leaf && overflowNodeForBorrowed)
    {
        vector<Node *> temp_child_ptr;
        temp_child_ptr.push_back(leftSibling->child_ptr.back());
        child_ptr.insert(child_ptr.begin(), temp_child_ptr.begin(), temp_child_ptr.end());
        leftSibling->child_ptr.back() = NULL;
    }
    else if (leaf && overFlowNodeForCurrFront)
    {
        vector<Node *> temp_child_ptr;
        temp_child_ptr.push_back(leftSibling->child_ptr.back());
        child_ptr.insert(child_ptr.begin(), temp_child_ptr.begin(), temp_child_ptr.end());
    }
    num_keys++;

    // remove any extra child pointers that has been pushed back
    if (leaf){
        int numToRemove = child_ptr.size() - n;
        for (int i = 0; i < numToRemove; ++i)
        {
            child_ptr.pop_back();
        }}

    // Adjust sibling node
    leftSibling->keys.pop_back();
    leftSibling->record_ptr.pop_back();
    leftSibling->num_keys--;

    // Adjust parent pointers
    if (!leaf)
    {
        Node *borrowedChild = leftSibling->child_ptr.back();
        // borrowedChild->printKeys();
        leftSibling->child_ptr.pop_back();
        child_ptr.insert(child_ptr.begin(), borrowedChild);
        // n++;
        printKeys();
        cout << "n:" << num_keys << endl;
    }
    return this;
}

Node *Node::borrow_right(Node *rightSibling, Node *parentNode, float parentKey, Node *childNode)
{
    int childPtrIndex = 0;
    while (childPtrIndex < parentNode->child_ptr.size() && parentNode->child_ptr[childPtrIndex] != childNode)
    {
        childPtrIndex++;
        // cout << "childPtrIndex:" << childPtrIndex << endl;
    }
    int siblingIndex = childPtrIndex++;
    cout << "index of right sibling index in parent node:" << siblingIndex << endl;
    cout << "borrowed key:" << parentNode->keys[siblingIndex] << endl;
    int borrowedKey = parentNode->keys[siblingIndex];
    bool overflowNodeForBorrowed = checkOverFlowNode(rightSibling, borrowedKey);
    // bool overFlowNodeForCurrFront = checkOverFlowNode(childNode,childNode->keys[0]);
    cout << checkOverFlowNode(childNode, childNode->keys[0]) << endl;
    // childNode->printKeys();

    // Move the parent key down to the current node
    keys.push_back(parentNode->keys[siblingIndex]);
    if (leaf)
    {
        record_ptr.push_back(parentNode->record_ptr[siblingIndex]);
    }
    vector<Node *> temp_child_ptr;
    temp_child_ptr.push_back(rightSibling->child_ptr.front());
    num_keys++;
    if (leaf){
        child_ptr.insert(child_ptr.begin() + num_keys - 1, temp_child_ptr.begin(), temp_child_ptr.end());
        if (overflowNodeForBorrowed)
        {
            // child_ptr.push_back()
            rightSibling->child_ptr.front() = NULL;
        }
        cout << "check child pointer size for borrow right:" << child_ptr.size() << endl;
        int numToRemove = child_ptr.size() - n;
        for (int i = 0; i < numToRemove; ++i)
        {
            child_ptr.pop_back();
        }
        cout << "check child pointer size for borrow right after delete: " << child_ptr.size() << endl;
    }
    // printKeys();

    // Move the first key from the right sibling to the parent node
    parentNode->keys[siblingIndex] = rightSibling->keys.front();
    parentNode->record_ptr[siblingIndex] = rightSibling->record_ptr.front();
    // parentNode->printKeys();

    // Adjust sibling node
    rightSibling->keys.erase(rightSibling->keys.begin());
    if (leaf)
    {
        rightSibling->record_ptr.erase(rightSibling->record_ptr.begin());
    }
    cout << "right sibling keys size:" << rightSibling->keys.size() << endl;
    rightSibling->num_keys--;

    // Adjust parent pointers
    if (!leaf)
    {
        Node *borrowedChild = rightSibling->child_ptr.front();
        rightSibling->child_ptr.erase(rightSibling->child_ptr.begin());
        child_ptr.push_back(borrowedChild);
        // n++;
    }
    return this;
}

void Node::printKeys()
{
    cout << "Keys in the node: ";
    for (int i = 0; i < keys.size(); ++i)
    {
        cout << keys[i];
        if (i != keys.size() - 1)
        {
            cout << " ";
        }
    }
    cout << endl;
}
int Node::findChildPtrIndex(Node *parentNode, Node *childNode)
{
    int childPtrIndex = 0;
    while (childPtrIndex < parentNode->child_ptr.size() && parentNode->child_ptr[childPtrIndex] != childNode)
    {
        childPtrIndex++;
    }
    return childPtrIndex;
}

void Node::insertDuplicate(int k, Record* rptr) {
    if (num_keys == n) {
        if (ptr2next) 
            ptr2next->insertDuplicate(k, rptr);
        else {
            Node* new_node = new Node(n);
            new_node->keys.push_back(k);
            new_node->record_ptr.push_back(rptr);
            new_node->num_keys++;
            ptr2next = new_node;
        }
        return;
    }
    else {
        keys.push_back(k);
        record_ptr.push_back(rptr);
        num_keys++;
        return;
    }
}

bool Node::printRecord(int min, int max) {
    bool printed = false;
    for (int i = 0; i < num_keys; i++) {
        if (keys[i] >= min && keys[i] <= max) {
            cout << "tconst: " << record_ptr[i]->tconst;
            cout << ", average rating: " << record_ptr[i]->averageRating;
            cout << ", numVotes: " << record_ptr[i]->numVotes << endl;
            printed = true;
            if (child_ptr[i]) {
                Node* cur_node = child_ptr[i];
                while (cur_node) {
                    for (int j = 0; j < cur_node->num_keys; j++) {
                        cout << "tconst: " << cur_node->record_ptr[j]->tconst;
                        cout << ", average rating: " << cur_node->record_ptr[j]->averageRating;
                        cout << ", numVotes: " << cur_node->record_ptr[j]->numVotes << endl;
                    }
                    cur_node = cur_node->ptr2next;
                }
            }
        }
        else if (keys[i] > max)
            return printed;
    }
    // keep calling next until they return
    if (ptr2next != NULL)
        printed = (ptr2next->printRecord(min, max) || printed);
    return printed;
}

void Node::search(int min, int max) {
    if (leaf) {
        cout << record_ptr[0]->averageRating << endl << endl;
        if (!printRecord(min, max)) {
            cout << "Record not found" << endl;
        }
    }
    
    else {
        int i = 0;
        while (i < num_keys && min >= keys[i]) {
            i++;
        }
        return child_ptr[i]->search(min, max);
    }
}

void Node::searchAverageRating(int min, int max, vector<float> &average_rating, int &index_accesses, int &data_accesses) {
    index_accesses++;
    if (leaf) {
        if (!collectAverageRatings(min, max, average_rating, index_accesses, data_accesses)) {
            cout << "Record not found" << endl;
            return;
        }
    }
    
    else {
        int i = 0;
        while (i < num_keys && min >= keys[i]) {
            i++;
        }
        return child_ptr[i]->searchAverageRating(min, max, average_rating, index_accesses, data_accesses);
    }
}

bool Node::collectAverageRatings(int min, int max, vector<float> &average_rating, int &index_accesses, int &data_accesses) {
    bool collected = false;
    for (int i = 0; i < num_keys; i++) {
        if (keys[i] >= min && keys[i] <= max) {
            data_accesses++;
            average_rating.push_back(record_ptr[i]->averageRating);
            collected = true;
            if (child_ptr[i]) {
                index_accesses++;
                Node* cur_node = child_ptr[i];
                while (cur_node) {
                    for (int j = 0; j < cur_node->num_keys; j++) {
                        average_rating.push_back(record_ptr[i]->averageRating);
                        data_accesses++;
                    }
                    cur_node = cur_node->ptr2next;
                }
            }
        }
        else if (keys[i] > max)
            return collected;
    }
    // keep calling next until they return
    if (ptr2next != NULL)
        collected = (ptr2next->collectAverageRatings(min, max, average_rating, index_accesses, data_accesses) || collected);
    return collected;
}