#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <sstream>

class Block {
private:
    int id;
    std::string previousHash;
    std::string data;
    int nonce;
    std::string hashValue;

    std::string calculateHash() const {
        std::hash<std::string> hasher;
        std::stringstream ss;
        ss << id << previousHash << data << nonce;
        return std::to_string(hasher(ss.str()));
    }

public:
    Block(int blockId, const std::string& prevHash, const std::string& blockData)
        : id(blockId), previousHash(prevHash), data(blockData), nonce(0) {}

    void mineBlock(int difficulty) {
        auto start = std::chrono::high_resolution_clock::now();
        std::string target(difficulty, '0'); // Cible avec 'difficulty' zéros
        while (true) {
            hashValue = calculateHash();
            if (hashValue.substr(0, difficulty) == target) break;
            nonce++;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Bloc " << id << " mine. Hash: " << hashValue << ", Nonce: " << nonce
                  << ", Temps: " << duration.count() << " ms\n";
    }

    std::string getHash() const { return hashValue; }
};

class Blockchain {
private:
    std::vector<Block> chain;

public:
    Blockchain() {
        Block genesis(0, "0", "Genesis Block");
        genesis.mineBlock(1); // Difficulté basse pour genesis
        chain.push_back(genesis);
    }

    void addBlock(const std::string& data, int difficulty) {
        Block newBlock(chain.size(), chain.back().getHash(), data);
        newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
    }
};

int main() {
    Blockchain bc;
    bc.addBlock("Transaction 1", 2); // Difficulté 2
    bc.addBlock("Transaction 2", 3); // Difficulté 3
    bc.addBlock("Transaction 3", 4); // Difficulté 4
    return 0;
}