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
int Node::smallest() {
    if (!leaf) {
        return child_ptr.at(0)->smallest();
    }
    else {
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
            // If the node is a leaf, simply delete the key
            keys.erase(keys.begin() + index);
            record_ptr.erase(record_ptr.begin() + index);
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
            // cout << "parentNode:" << parentNode << endl;
            // Check for underflow after deletion in the child node
            if (parentNode->keys.size()==0){
                return deletedNode;
            }
            if (deletedNode && num_keys < n / 2 && parentNode)
            {
                Node *leftSiblingCurr = findLeftSibling(parentNode, parentKey, this);
                Node *rightSiblingCurr = findRightSibling(parentNode, parentKey, this);
                if (leftSiblingCurr && leftSiblingCurr->num_keys> n / 2)
                {
                    return borrow_left(leftSiblingCurr, parentNode, parentKey, this);
                }
                if (rightSiblingCurr && rightSiblingCurr->num_keys > n / 2)
                {
                    cout << "borrowing from right for internal node" << endl;
                    return borrow_right(rightSiblingCurr, parentNode, parentKey, this);
                }
                if (parentNode->num_keys>1 && leftSiblingCurr){

                    
                    
                    // leftSiblingCurr->printKeys();
                    leftSiblingCurr->keys.push_back(deletedNode->keys[0]);
                    leftSiblingCurr->child_ptr.push_back(deletedNode);
                    leftSiblingCurr->num_keys++;
                    // cout << "left sibling child pointer size:" << leftSiblingCurr->child_ptr.size() << endl;

                    parentNode->keys.pop_back();
                    parentNode->child_ptr.pop_back();
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
                if (parentNode->keys.size()==1)
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
                if (rightSiblingCurr && rightSiblingCurr->num_keys > n/ 2)
                {
                    cout << "borrowing from right for internal node" << endl;
                    return borrow_right(rightSiblingCurr, parentNode, parentKey, this);
                }
                if (parentNode->num_keys>1 && leftSiblingCurr){
                    // leftSiblingCurr->printKeys();
                    leftSiblingCurr->keys.push_back(deletedNode->keys[0]);
                    leftSiblingCurr->child_ptr.push_back(deletedNode);
                    leftSiblingCurr->num_keys++;
                    // cout << "left sibling child pointer size:" << leftSiblingCurr->child_ptr.size() << endl;

                    parentNode->keys.pop_back();
                    parentNode->child_ptr.pop_back();
                    parentNode->num_keys--;
                    // // cout << "u"
                    // deletedNode->findParentKey(parentNode,deletedNode);
                    // cout << "current child pointer size:" << this->child_ptr.size() << endl;
                    // child_ptr.pop_back();
                    // cout << "current child pointer size:" << this->child_ptr.size() << endl;
                    // parentNode->printKeys();
                    return deletedNode;
                }
                if (parentNode->num_keys > 1 && rightSiblingCurr){
                    // deletedNode->printKeys();
                    rightSiblingCurr->keys.insert(rightSiblingCurr->keys.begin(),parentKey);
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
    num_keys += leftSibling->num_keys;
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
    if (parentNode->num_keys < n/ 2)
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

    // Move keys and pointers from the right sibling to the current node
    rightSibling->keys.insert(rightSibling->keys.begin(), keys.begin(), keys.end());
    rightSibling->record_ptr.insert(rightSibling->record_ptr.begin(), record_ptr.begin(), record_ptr.end());
    // keys.insert(keys.end(), rightSibling->keys.begin(), rightSibling->keys.end());
    // record_ptr.insert(record_ptr.end(), rightSibling->record_ptr.begin(), rightSibling->record_ptr.end());
    if (!leaf)
    {
        // child_ptr.insert(child_ptr.end(), rightSibling->child_ptr.begin(), rightSibling->child_ptr.end());
        rightSibling->child_ptr.insert(rightSibling->child_ptr.end(), child_ptr.begin(), child_ptr.end());
    }
    // n += rightSibling->n;
    rightSibling->num_keys += num_keys;
    cout << "current node size:" << rightSibling->num_keys << endl;
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

    // Move the parent key down to the current node
    keys.insert(keys.begin(), parentNode->keys[siblingIndex]);
    record_ptr.insert(record_ptr.begin(), parentNode->record_ptr[siblingIndex]);
    num_keys++;

    cout << "left sibling keys back:" << leftSibling->keys.back() << endl;
    cout << "parent node keys back:" << parentNode->keys[siblingIndex] << endl;
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
    // Move the parent key down to the current node
    keys.push_back(parentNode->keys[siblingIndex]);
    if (leaf)
    {
        record_ptr.push_back(parentNode->record_ptr[siblingIndex]);
    }
    num_keys++;
    printKeys();
    // Move the first key from the right sibling to the parent node
    parentNode->keys[siblingIndex] = rightSibling->keys.front();
    parentNode->record_ptr[siblingIndex] = rightSibling->record_ptr.front();
    // parentNode->printKeys();
    // Adjust sibling node

    rightSibling->keys.erase(rightSibling->keys.begin());
    // rightSibling->record_ptr.erase(rightSibling->record_ptr.begin());
    rightSibling->printKeys();
    if (leaf)
    {
        rightSibling->record_ptr.erase(rightSibling->record_ptr.begin());
    }
    // if (!leaf){
    // }
    cout << "right sibling keys size:" << rightSibling->keys.size() << endl;
    rightSibling->printKeys();
    rightSibling->num_keys--;
    printKeys();

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