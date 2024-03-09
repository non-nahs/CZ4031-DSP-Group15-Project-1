#include <iostream>
#include <chrono>
#include "Btree.h"
#include "memory.h"
using namespace std;

void printTree(BTree* tree) {
    tree->printTree();
}

int main() {
    const string tsvFilename = "data.tsv"; 
    const string binaryFilename = "disk_storage_18byes_record.bin"; // Replace name with size of byte record
    int numRecordsToDisplay = 10;

    // B+ tree initialisation
    int max_keys = 5;
    BTree* bplustree = new BTree(max_keys);

    loadTSVData(tsvFilename, binaryFilename, bplustree);

    cout << "Data loading complete." << endl;

    // experiment 1
    showDatabaseStatistics(binaryFilename);

    // readAndDisplayBinaryFile(binaryFilename, numRecordsToDisplay);

    // printTree(bplustree);
    cout << endl;

    // experiment 2

    cout << "parameter n of the B+ tree: " << max_keys << endl;
    cout << "number of nodes of the B+ tree: " << bplustree->countNodes(bplustree->root) << endl;
    cout << "number of levels of the B+ tree (including root and overflow): " << bplustree->countLevels(bplustree->root) << endl;
    cout << "content of the root node: ";
    for (int i = 0; i < bplustree->root->num_keys; i++)
        cout << bplustree->root->keys[i] << " ";
    cout << endl << endl;

    // experiment 3

    cout << "number of index nodes the process accesses: " << endl; // TODO
    cout << "number of data blocks the process accesses: " << endl; // TODO

    // Record the start time
    auto start = chrono::high_resolution_clock::now();
    vector<float> avg_ratings = bplustree->searchAverageRating(500);
    // Record the end time
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    float sum = 0;
    for (float avg_rating : avg_ratings) {
        sum += avg_rating;
    }
    cout << "average of “averageRating’s”: " << sum / avg_ratings.size() << endl;

    cout << "running time of the retrieval process: " << duration.count() << endl;
    cout << "number of data blocks that would be accessed by a brute-force linear scan method: " << endl; // TODO

    // experiment 4
    // same as 3 but using a range

    // TODO: store B+ tree in blocks somehow

    return 0;
}
