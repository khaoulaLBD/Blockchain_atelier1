#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <sstream>

class Blockchain; // Forward declaration for friend class

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
    friend class Blockchain; // Allow Blockchain to access private members
public:
    Block(int blockId, const std::string& prevHash, const std::string& blockData)
        : id(blockId), previousHash(prevHash), data(blockData), nonce(0) {}
    long mineBlock(int difficulty) {
        auto start = std::chrono::high_resolution_clock::now();
        std::string target(difficulty, '0');
        while (true) {
            hashValue = calculateHash();
            if (nonce % 100000 == 0) { // Debug: Show progress
                std::cout << "Bloc " << id << " trying nonce: " << nonce << "\n";
            }
            if (hashValue.substr(0, difficulty) == target) break;
            nonce++;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Bloc " << id << " mine. Hash: " << hashValue << ", Nonce: " << nonce
                  << ", Temps: " << duration.count() << " ms\n";
        return duration.count();
    }
    std::string getHash() const { return hashValue; }
    std::string getPreviousHash() const { return previousHash; }
};

class Blockchain {
private:
    std::vector<Block> chain;
public:
    Blockchain() {
        Block genesis(0, "0", "Genesis Block");
        genesis.mineBlock(1);
        chain.push_back(genesis);
    }
    long addBlock(const std::string& data, int difficulty) {
        Block newBlock(chain.size(), chain.back().getHash(), data);
        long duration = newBlock.mineBlock(difficulty);
        chain.push_back(newBlock);
        return duration;
    }
    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            if (chain[i].getHash() != chain[i].calculateHash()) {
                std::cout << "Bloc " << i << " a un hash invalide\n";
                return false;
            }
            if (chain[i].getPreviousHash() != chain[i-1].getHash()) {
                std::cout << "Bloc " << i << " a un previousHash incorrect\n";
                return false;
            }
        }
        return true;
    }
    // For tampering test
    void tamperBlock(size_t index, const std::string& newData) {
        if (index < chain.size()) {
            chain[index].data = newData;
        }
    }
};

int main() {
    std::cout << "Debut du programme\n";
    Blockchain bc;
    std::vector<long> times;
    std::vector<int> difficulties = {2, 3, 4};
    for (size_t i = 0; i < difficulties.size(); ++i) {
        times.push_back(bc.addBlock("Transaction " + std::to_string(i + 1), difficulties[i]));
    }
    std::cout << "Avant tamper: Chaine valide: " << (bc.isChainValid() ? "Oui" : "Non") << "\n";
    bc.tamperBlock(1, "Hacked Data");
    std::cout << "Apres tamper: Chaine valide: " << (bc.isChainValid() ? "Oui" : "Non") << "\n";
    std::cout << "Difficultes: ";
    for (int d : difficulties) std::cout << d << " ";
    std::cout << "\nTemps (ms): ";
    for (long t : times) std::cout << t << " ";
    std::cout << "\nFin du programme\n";
    return 0;
}
