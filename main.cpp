#include <iostream>
#include <chrono>
#include <iomanip>
#include "Btree.h"
#include "memory.h"
using namespace std;
using namespace std::chrono;

void printTree(BTree* tree) {
    tree->printTree();
}

int main() {
    const string tsvFilename = "data.tsv"; 
    const string binaryFilename = "disk_storage_18byes_record.bin"; // Replace name with size of byte record
    int numRecordsToDisplay = 10;

    // B+ tree initialisation
    int max_keys = 3;
    BTree* bplustree = new BTree(max_keys);

    loadTSVData(tsvFilename, binaryFilename, bplustree);

    cout << "Data loading complete." << endl << endl;

    // experiment 1
    cout << "Experiment 1:" << endl;
    showDatabaseStatistics(binaryFilename);

    // readAndDisplayBinaryFile(binaryFilename, numRecordsToDisplay);

    // printTree(bplustree);
    cout << endl;

    // experiment 2
    cout << "Experiment 2:" << endl;
    cout << "Parameter n of the B+ tree: " << max_keys << endl;
    cout << "Number of nodes of the B+ tree: " << bplustree->countNodes(bplustree->root) << endl;
    cout << "Number of levels of the B+ tree (including root and overflow): " << bplustree->countLevels(bplustree->root) << endl;
    cout << "Content of the root node: ";
    for (int i = 0; i < bplustree->root->num_keys; i++)
        cout << bplustree->root->keys[i] << " ";
    cout << endl << endl;

    // experiment 3
    cout << "Experiment 3:" << endl;

    int index_accesses = 0;
    int data_accesses = 0;
    // Record the start time
    auto start = chrono::high_resolution_clock::now();
    vector<float> avg_ratings = bplustree->searchAverageRating(index_accesses, data_accesses, 500);
    // Record the end time
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);

    cout << "Number of index nodes the process accesses: " << index_accesses << endl;
    cout << "Number of data blocks the process accesses: " << data_accesses << endl;

    float sum = 0;
    for (float avg_rating : avg_ratings) {
        sum += avg_rating;
    }
    cout << "Average of “averageRating’s”: " << fixed << setprecision(3) << sum / avg_ratings.size() << endl;

    cout << "Running time of the retrieval process: " << duration.count() << " microseconds" << endl;
    scanBinaryFileExperiment3(binaryFilename);

    // experiment 4
    cout << endl << "Experiment 4:" << endl;

    int index_accesses2 = 0;
    int data_accesses2 = 0;
    // Record the start time
    auto start2 = chrono::high_resolution_clock::now();
    vector<float> avg_ratings2 = bplustree->searchAverageRating(index_accesses2, data_accesses2, 30000, 40000);
    // Record the end time
    auto end2 = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::microseconds>(end2 - start2);

    cout << "Number of index nodes the process accesses: " << index_accesses2 << endl;
    cout << "Number of data blocks the process accesses: " << data_accesses2 << endl;

    float sum2 = 0;
    for (float avg_rating2 : avg_ratings2) {
        sum2 += avg_rating2;
    }
    cout << "Average of “averageRating’s”: " << fixed << setprecision(3) << sum2 / avg_ratings2.size() << endl;

    cout << "Running time of the retrieval process: " << duration2.count() << " microseconds" << endl;
    scanBinaryFileExperiment4(binaryFilename);

    cout << endl << "Experiment 5" << endl;
    cout << "----------------Deletion of 1000--------------------" << endl;
    auto start_deletion = high_resolution_clock::now();
    bplustree->deleteKey(1000);
    auto stop_deletion = high_resolution_clock::now();
    auto duration_deletion = duration_cast<microseconds>(stop_deletion - start_deletion);
    cout << "-----------------After Deletion---------------------" << endl;
    bplustree->printRoot();
    cout << "Number of nodes in the updated B+Tree:" << bplustree->countNodes(bplustree->root) << endl;
    cout << "Number of levels in the updated B+Tree:" << bplustree->countLevels(bplustree->root) << endl;
    cout << "Time taken for deletion: " << duration_deletion.count() << " microseconds" << endl;
    cout << "----------Checking whether 1000 has been deleted-----------" << endl;

    return 0;
}
