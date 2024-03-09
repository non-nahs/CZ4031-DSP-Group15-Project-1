#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "Btree.h"

const int BLOCK_SIZE = 200;

struct Record {
    char tconst[10];
    float averageRating;
    int numVotes;

    // Default constructor
    Record() : averageRating(0), numVotes(0) {
        tconst[0] = '\0'; // Initialize tconst with an empty string
    }

    Record(const std::string& id, float averageRating, int numVotes);
};

struct Block {
    char data[BLOCK_SIZE];
    int recordCount;

    bool addRecord(const Record& record);
    bool isFull();
    void clear();
};

void writeBlockToFile(const Block& block, const std::string& filename, int blockNum);
void loadTSVData(const std::string& tsvFilename, const std::string& binaryFilename, BTree* bplustree);
void showDatabaseStatistics(const std::string& binaryFilename);
void readAndDisplayBinaryFile(const std::string& binaryFilename, int numRecordsToDisplay);
void scanBinaryFileExperiment3(const std::string& binaryFilename);
void scanBinaryFileExperiment4(const std::string& binaryFilename);

#endif // DATABASE_H
