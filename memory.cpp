#include "memory.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <chrono>
// using namespace std;

// const int BLOCK_SIZE = 200; // Block size in bytes

#pragma pack(push, 1) // Disable padding bytes

// Key data structure for B+ tree
Record::Record(const std::string& id, float averageRating, int numVotes)
    : averageRating(averageRating), numVotes(numVotes) {
    strncpy(this->tconst, id.c_str(), sizeof(this->tconst) - 1);
    this->tconst[sizeof(this->tconst) - 1] = '\0'; // Ensure null-termination
}
#pragma pack(pop) // Re-enable padding

const int RECORD_SIZE = sizeof(Record); // The size of a record
const int MAX_RECORDS_PER_BLOCK = BLOCK_SIZE / RECORD_SIZE;

bool Block::addRecord(const Record& record) {
    if (recordCount < MAX_RECORDS_PER_BLOCK) {
        int offset = recordCount * sizeof(Record);
        memcpy(data + offset, &record, sizeof(Record));
        recordCount++;
        return true;
    }
    return false;
}

bool Block::isFull() {
    return recordCount >= MAX_RECORDS_PER_BLOCK;
}

void Block::clear() {
    memset(data, 0, BLOCK_SIZE);
    recordCount = 0;
}

void writeBlockToFile(const Block& block, const std::string& filename, int blockNum) {
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        file.open(filename, std::ios::out | std::ios::binary); // Create file if it doesn't exist
    }
    file.seekp(blockNum * BLOCK_SIZE, std::ios::beg);
    file.write(block.data, BLOCK_SIZE);
    file.close();
}

void loadTSVData(const std::string& tsvFilename, const std::string& binaryFilename, BTree* bplustree) {
    std::ifstream tsvFile(tsvFilename);
    std::string line, token;
    Block block;
    int blockNum = 0;

    // Skip header line
    std::getline(tsvFile, line);

    while (std::getline(tsvFile, line)) {
        std::istringstream lineStream(line);
        std::getline(lineStream, token, '\t'); // tconst
        std::string tconst = token;
        std::getline(lineStream, token, '\t'); // averageRating
        float averageRating = std::stof(token);
        std::getline(lineStream, token, '\t'); // numVotes
        int numVotes = std::stoi(token);

        Record* record = new Record(tconst, averageRating, numVotes);
        if (!block.addRecord(*record)) {
            writeBlockToFile(block, binaryFilename, blockNum++);
            block.clear();
            block.addRecord(*record); // Add record to the new block
        }
        // insert into b+tree
        bplustree->insert(record->numVotes, record);
    }

    // Write any remaining records to file
    if (block.recordCount > 0) {
        writeBlockToFile(block, binaryFilename, blockNum);
    }

    std::cout << "Data written to file: " << binaryFilename << std::endl;

    tsvFile.close();
}

// Function to display the database statistics
void showDatabaseStatistics(const std::string& binaryFilename) {
    // Open the binary file for reading
    std::ifstream file(binaryFilename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Could not open the file for reading." << std::endl;
        return;
    }

    // Get the size of the file
    std::streamsize fileSize = file.tellg();
    file.close();

    // Calculate the number of blocks and number of records
    int numberOfBlocks = fileSize / BLOCK_SIZE;
    int numberOfRecords = numberOfBlocks * MAX_RECORDS_PER_BLOCK;

    // Check for any partial block at the end
    if (fileSize % BLOCK_SIZE != 0) {
        numberOfBlocks++; // There's an additional partial block
        std::ifstream file(binaryFilename, std::ios::binary);
        file.seekg(numberOfBlocks - 1 * BLOCK_SIZE); // Go to the last block
        Block block;
        file.read(block.data, BLOCK_SIZE); // Read the last block
        file.close();
        // In the last block, not all positions might be filled, so count actual records
        numberOfRecords = (numberOfBlocks - 1) * MAX_RECORDS_PER_BLOCK + block.recordCount;
    }

    // Displaying the statistics
    std::cout << "Experiment 1" << std::endl;
    std::cout << "Database Statistics:" << std::endl;
    std::cout << "Number of records: " << numberOfRecords << std::endl;
    std::cout << "Size of a record: " << RECORD_SIZE << " bytes" << std::endl;
    std::cout << "Number of records stored in a block: " << MAX_RECORDS_PER_BLOCK << std::endl;
    std::cout << "Number of blocks for storing the data: " << numberOfBlocks << std::endl;
}
// <<<<<<< Updated upstream

void readAndDisplayBinaryFile(const std::string& binaryFilename, int numRecordsToDisplay) {
    std::ifstream binaryFile(binaryFilename, std::ios::binary);
    if (!binaryFile.is_open()) {
        std::cerr << "Could not open the file for reading." << std::endl;
        return;
    }

    int recordsDisplayed = 0;

    while (binaryFile && recordsDisplayed < numRecordsToDisplay) {
        Block block;
        binaryFile.read(reinterpret_cast<char*>(&block), sizeof(Block));

        for (int i = 0; i < block.recordCount && recordsDisplayed < numRecordsToDisplay; ++i) {
            // Assuming the starting point of each Record is correctly calculated
            Record* record = reinterpret_cast<Record*>(block.data + i * sizeof(Record));
            std::cout << "Experiment 1" << std::endl;
            std::cout << "Record " << recordsDisplayed + 1 << ": " << std::endl;
            std::cout << "tconst: " << record->tconst << std::endl;
            std::cout << "Average Rating: " << record->averageRating << std::endl;
            std::cout << "Number of Votes: " << record->numVotes << std::endl;
            std::cout << "------------------------------------" << std::endl;
            ++recordsDisplayed;
        }
    }

    binaryFile.close();
}

void scanBinaryFileExperiment3(const std::string& binaryFilename) {
    std::ifstream binaryFile(binaryFilename, std::ios::binary);
    if (!binaryFile.is_open()) {
        std::cerr << "Could not open the binary file for reading." << std::endl;
        return;
    }

    int blockCount = 0, recordCount = 0;
    char block[BLOCK_SIZE];

    // Start measuring time
    auto startTime = std::chrono::high_resolution_clock::now();

    // Read and count blocks
    while (binaryFile.read(block, BLOCK_SIZE) || binaryFile.gcount() != 0) {
        ++blockCount;

        // Process each record in the block
        for (int offset = 0; offset < BLOCK_SIZE; offset += sizeof(Record)) {
            Record record;
            memcpy(&record, block + offset, sizeof(Record));

            if (record.numVotes == 500) {
                // std::cout << "Record with 500 votes found: " << record.tconst << std::endl;
                recordCount++;
            }
        }
    }

    // Stop measuring time
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> runningTime = endTime - startTime;

    binaryFile.close();

    std::cout << "Experiment 3 Brute force linear scan results" << std::endl;
    std::cout << "Number of blocks accessed: " << blockCount << std::endl;
    std::cout << "Number of records = 500 accessed: " << recordCount << std::endl;
    std::cout << "Running time: " << runningTime.count() << " milliseconds" << std::endl;
}

void scanBinaryFileExperiment4(const std::string& binaryFilename) {
    std::ifstream binaryFile(binaryFilename, std::ios::binary);
    if (!binaryFile.is_open()) {
        std::cerr << "Could not open the binary file for reading." << std::endl;
        return;
    }

    int blockCount = 0, recordCount = 0;
    char block[BLOCK_SIZE];

    // Start measuring time
    auto startTime = std::chrono::high_resolution_clock::now();

    // Read and count blocks
    while (binaryFile.read(block, BLOCK_SIZE) || binaryFile.gcount() != 0) {
        ++blockCount;

        // Process each record in the block
        for (int offset = 0; offset < BLOCK_SIZE; offset += sizeof(Record)) {
            Record record;
            memcpy(&record, block + offset, sizeof(Record));

            if (record.numVotes >= 30000 && record.numVotes <= 40000) {
                // std::cout << "Record between 30k and 40k votes found: " << record.tconst << ", Vote count: " << record.numVotes << std::endl;
                recordCount++;
            }
        }
    }

    // Stop measuring time
    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> runningTime = endTime - startTime;

    binaryFile.close();

    std::cout << "Experiment 4 Brute force linear scan results" << std::endl;
    std::cout << "Number of blocks accessed: " << blockCount << std::endl;
    std::cout << "Number of records accessed: " << recordCount << std::endl;
    std::cout << "Running time: " << runningTime.count() << " milliseconds" << std::endl;
}

// int main() {
    // const std::string binaryFilename = "disk_storage_18byes_record.bin"; // Replace name with size of byte record
//     scanBinaryFileExperiment3(binaryFilename);
//     return 0;
// }

// int main() {
//     const std::string tsvFilename = "data.tsv"; 
//     const std::string binaryFilename = "disk_storage_18byes_record.bin"; // Replace name with size of byte record
//     int numRecordsToDisplay = 10;

//     loadTSVData(tsvFilename, binaryFilename, nullptr);

//     std::cout << "Data loading complete." << std::endl;

//     showDatabaseStatistics(binaryFilename);

//     readAndDisplayBinaryFile(binaryFilename, numRecordsToDisplay);
//     return 0;
// }
// =======
// >>>>>>> Stashed changes
