#include <iostream>
#include <fstream>
#include "memory.cpp"

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


        std::cout << "Block " << recordsDisplayed + 1 << ": " << std::endl;
        // for (int i = 0; i < block.recordCount && recordsDisplayed < numRecordsToDisplay; ++i) {
        //     // Assuming the starting point of each Record is correctly calculated
        //     Record* record = reinterpret_cast<Record*>(block.data + i * sizeof(Record));
        //     std::cout << "Record " << recordsDisplayed + 1 << ": " << std::endl;
        //     std::cout << "tconst: " << record->tconst << std::endl;
        //     std::cout << "Average Rating: " << record->averageRating << std::endl;
        //     std::cout << "Number of Votes: " << record->numVotes << std::endl;
        //     std::cout << "------------------------------------" << std::endl;
        //     ++recordsDisplayed;
        // }
    }

    binaryFile.close();
}

int main() {
    const std::string tsvFilename = "data.tsv"; 
    const std::string binaryFilename = "disk_storage_18byes_record.bin";
    int numRecordsToDisplay = 10;

    // loadTSVData(tsvFilename, binaryFilename);

    // std::cout << "Data loading complete." << std::endl;

    // showDatabaseStatistics(binaryFilename);

    readAndDisplayBinaryFile(binaryFilename, numRecordsToDisplay);
    return 0;
}