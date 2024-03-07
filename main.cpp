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
    Record* r13 = new Record("tt0000013", 5.7, 1572);
    Record* r14 = new Record("tt0000014", 7.1, 4599);
    Record* r15 = new Record("tt0000015", 6.1, 817);
    Record* r16 = new Record("tt0000016", 5.9, 1200);
    Record* r17 = new Record("tt0000017", 4.6, 241);
    Record* r18 = new Record("tt0000018", 5.3, 478);
    Record* r19 = new Record("tt0000019", 5.3, 20);
    Record* r20 = new Record("tt0000020", 5.0, 267);
    Record *r21 = new Record("tt0000021", 5.0, 115);

    bplustree->insert(r1->numVotes, r1);
    bplustree->insert(r2->numVotes, r2);
    // bplustree->insert(r3->numVotes, r3);
    bplustree->insert(r4->numVotes, r4);
    bplustree->insert(r5->numVotes, r5);
    bplustree->insert(r6->numVotes, r6);
    bplustree->insert(r7->numVotes, r7);
    bplustree->insert(r8->numVotes, r8);
    bplustree->insert(r9->numVotes, r9);
    bplustree->insert(r10->numVotes, r10);
    bplustree->insert(r11->numVotes, r11);
    bplustree->insert(r12->numVotes, r12);
    bplustree->insert(r13->numVotes, r13);
    // bplustree->insert(r14->numVotes, r14);
    // bplustree->insert(r15->numVotes, r15);
    // bplustree->insert(r16->numVotes, r16);
    // bplustree->insert(r17->numVotes, r17);
    // bplustree->insert(r18->numVotes, r18);
    // bplustree->insert(r19->numVotes, r19);
    // bplustree->insert(r20->numVotes, r20);
    // bplustree->insert(r21->numVotes, r21);

    printTree(bplustree);

    // Search for each record in the B+ tree
    // bplustree->search(1645); // For r1
    // bplustree->search(198);  // For r2
    // bplustree->search(1342); // For r3
    // bplustree->search(120);  // For r4
    // bplustree->search(2127); // For r5
    // bplustree->search(115);  // For r6
    // bplustree->search(652);  // For r7
    // bplustree->search(1807); // For r8
    // bplustree->search(154);  // For r9
    // bplustree->search(6018); // For r10
    // bplustree->search(262);  // For r11
    // bplustree->search(10287); // For r12
    // bplustree->search(1572);  // For r13
    // bplustree->search(4599);  // For r14
    // bplustree->search(817);   // For r15
    // bplustree->search(1200);  // For r16
    // bplustree->search(241);   // For r17
    // bplustree->search(478);   // For r18
    // bplustree->search(20);    // For r19
    // bplustree->search(267);   // For r20

    // // merge with left leaf node
    // cout << "-------------------------------------" << endl;
    // cout << "First deletion of 1572 " << endl;
    // bplustree->deleteKey(1572);
    // printTree(bplustree);
    // // simple case
    // cout << "-------------------------------------" << endl;
    // cout << "Second deletion of 154" << endl;
    // bplustree->deleteKey(154);
    // printTree(bplustree);
    // // borrow from right leaf node
    // cout << "-------------------------------------" << endl;
    // cout << "Third deletion of 120 " << endl;
    // bplustree->deleteKey(120);
    // printTree(bplustree);
    // // merge with right internal node
    // cout << "-------------------------------------" << endl;
    // cout << "Fourth deletion of 262 " << endl;
    // bplustree->deleteKey(262);
    // printTree(bplustree);
    // // borrow from left leaf node
    // cout << "-------------------------------------" << endl;
    // cout << "Fourth deletion of 267 " << endl;
    // bplustree->deleteKey(267);
    // printTree(bplustree);
    // merge with left leaf node and merge with right internal node
    // cout << "-------------------------------------" << endl;
    // cout << "Fourth deletion of 198 " << endl;
    // bplustree->deleteKey(198);
    // printTree(bplustree);
    // if (bplustree->search(5.6) != NULL) {
    //     cout << "Found" << endl;
    // } else {
    //     cout << "Not found" << endl;
    // }

    return 0;
}
