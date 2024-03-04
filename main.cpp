#include <iostream>
#include "Btree.h"
using namespace std;

void printTree(BTree* tree) {
    tree->printTree();
}

int main() {
    int record_len = 3;
    BTree* bplustree = new BTree(record_len);

    Record* r1 = new Record("tt0000001", 5.6, 1645);
    Record* r2 = new Record("tt0000002", 6.1, 198);
    Record* r3 = new Record("tt0000003", 6.5, 1342);
    Record* r4 = new Record("tt0000004", 6.2, 120);
    Record* r5 = new Record("tt0000005", 6.2, 2127);
    Record* r6 = new Record("tt0000006", 5.3, 115);
    Record* r7 = new Record("tt0000007", 5.5, 652);
    Record* r8 = new Record("tt0000008", 5.4, 1807);
    Record* r9 = new Record("tt0000009", 5.9, 154);
    Record* r10 = new Record("tt0000010", 6.9, 6018);
    Record* r11 = new Record("tt0000011", 5.2, 262);
    Record* r12 = new Record("tt0000012", 7.4, 10287);

    bplustree->insert(r1->averageRating, r1);
    bplustree->insert(r2->averageRating, r2);
    bplustree->insert(r3->averageRating, r3);
    bplustree->insert(r4->averageRating, r4);
    bplustree->insert(r5->averageRating, r5);
    bplustree->insert(r6->averageRating, r6);
    bplustree->insert(r7->averageRating, r7);
    bplustree->insert(r8->averageRating, r8);
    bplustree->insert(r9->averageRating, r9);
    bplustree->insert(r10->averageRating, r10);
    bplustree->insert(r11->averageRating, r11);
    bplustree->insert(r12->averageRating, r12);

    printTree(bplustree);

    // Search for each record in the B+ tree
    bplustree->search(5.6);
    bplustree->search(6.1);
    bplustree->search(6.5);
    bplustree->search(6.2);
    bplustree->search(6.2);
    bplustree->search(5.3);
    bplustree->search(5.5);
    bplustree->search(5.4);
    bplustree->search(5.9);
    bplustree->search(6.9);
    bplustree->search(5.2);
    bplustree->search(7.4);

    // if (bplustree->search(5.6) != NULL) {
    //     cout << "Found" << endl;
    // } else {
    //     cout << "Not found" << endl;
    // }

    return 0;
}
