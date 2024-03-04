#include <iostream>

// disk_size = 500MB
// block_size = 200B

#define DISK_SIZE 500*1024*1024
#define BLOCK_SIZE 200

class Disk;
class Block;

class Block {
public:
    Block() {
        std::cout << "Block constructor" << std::endl;
    }
    ~Block() {
        std::cout << "Block destructor" << std::endl;
    }

    void read() {
        std::cout << "Block read" << std::endl;
    }

    void write() {
        std::cout << "Block write" << std::endl;
    }

    void format() {
        std::cout << "Block format" << std::endl;
    }

    void print() {
        std::cout << "Block print" << std::endl;
    }

    void set_data(char* data) {
        std::cout << "Block set_data" << std::endl;

        for (int i = 0; i < BLOCK_SIZE; i++) {
            records[i] = data[i];
        }
    }

private:
    char records[BLOCK_SIZE];

};

class Disk {
public:
    Disk() {
        std::cout << "Disk constructor" << std::endl;
    }
    ~Disk() {
        std::cout << "Disk destructor" << std::endl;
    }

    void read() {
        std::cout << "Disk read" << std::endl;

        for (int i = 0; i < DISK_SIZE / BLOCK_SIZE; i++) {
            blocks[i]->read();
        }
    }

    void format() {
        std::cout << "Disk format" << std::endl;

        for (int i = 0; i < DISK_SIZE / BLOCK_SIZE; i++) {
            blocks[i]->format();
        }

        std::cout << "Disk formatted" << std::endl;
    }

private:
    Block *blocks[DISK_SIZE / BLOCK_SIZE];
};


int main() {
    Disk disk;
    std::cout << "main" << std::endl;

    disk.format();

    return 0;
}