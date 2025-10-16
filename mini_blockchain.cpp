#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <random>
#include <sstream>

// ---------- Transaction ----------
class Transaction {
public:
    std::string id, sender, receiver;
    double amount;
    Transaction(const std::string& i, const std::string& s, const std::string& r, double a)
        : id(i), sender(s), receiver(r), amount(a) {}
    std::string toString() const {
        std::stringstream ss;
        ss << id << ":" << sender << "->" << receiver << ":" << amount;
        return ss.str();
    }
};

// ---------- Merkle Tree ----------
class MerkleTree {
private:
    std::vector<std::string> leaves;
    std::string root;
    std::string hash(const std::string& left, const std::string& right) {
        std::hash<std::string> hasher;
        return std::to_string(hasher(left + right));
    }
    void buildTree() {
        std::vector<std::string> current = leaves;
        while (current.size() > 1) {
            std::vector<std::string> nextLevel;
            for (size_t i = 0; i < current.size(); i += 2) {
                if (i + 1 < current.size()) nextLevel.push_back(hash(current[i], current[i + 1]));
                else nextLevel.push_back(hash(current[i], current[i]));
            }
            current = nextLevel;
        }
        root = current[0];
    }
public:
    MerkleTree(const std::vector<Transaction>& txs) {
        for (const auto& tx : txs) leaves.push_back(tx.toString());
        if (!leaves.empty()) buildTree();
    }
    std::string getRoot() const { return root; }
};

// ---------- Validator ----------
struct Validator { std::string name; double stake; };

// ---------- Block ----------
class Block {
private:
    int id;
    std::string previousHash, merkleRoot, hashValue;
    int nonce;
    std::vector<Transaction> transactions;

    std::string calculateHash() const {
        std::hash<std::string> hasher;
        std::stringstream ss;
        ss << id << previousHash << merkleRoot << nonce;
        return std::to_string(hasher(ss.str()));
    }

public:
    Block(int blockId, const std::string& prevHash, const std::vector<Transaction>& txs)
        : id(blockId), previousHash(prevHash), nonce(0), transactions(txs) {
        MerkleTree tree(txs);
        merkleRoot = tree.getRoot();
    }

    void mineBlock() {
        auto start = std::chrono::high_resolution_clock::now();
        // PoW simplifié pour démonstration
        while (true) {
            hashValue = calculateHash();
            if (hashValue.back() == '0') break; // simple condition
            nonce++;
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Bloc " << id << " mine (PoW), Temps: " << duration.count() 
                  << " ms, Hash: " << hashValue << "\n";
    }

    void validate(const std::string& validator) {
        auto start = std::chrono::high_resolution_clock::now();
        hashValue = calculateHash();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Bloc " << id << " valide par " << validator 
                  << " (PoS), Temps: " << duration.count() 
                  << " ms, Hash: " << hashValue << "\n";
    }

    std::string getHash() const { return hashValue; }
    std::string getPreviousHash() const { return previousHash; }
    std::string getMerkleRoot() const { return merkleRoot; }
    bool isValid() const { return calculateHash() == hashValue; }
};

// ---------- Blockchain ----------
class Blockchain {
private:
    std::vector<Block> chain;
    std::vector<Validator> validators;

    std::string selectValidator() {
        double totalStake = 0;
        for (const auto& v : validators) totalStake += v.stake;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, totalStake);
        double random = dis(gen);
        double cumulative = 0;

        for (const auto& v : validators) {
            cumulative += v.stake;
            if (random <= cumulative) return v.name;
        }
        return validators.back().name;
    }

public:
    Blockchain(const std::vector<Validator>& vals) : validators(vals) {
        std::vector<Transaction> genesisTx = {Transaction("0", "Genesis", "Genesis", 0)};
        Block genesis(0, "0", genesisTx);
        genesis.mineBlock();
        chain.push_back(genesis);
    }

    void addBlockPoW(const std::vector<Transaction>& txs) {
        Block newBlock(chain.size(), chain.back().getHash(), txs);
        newBlock.mineBlock();
        chain.push_back(newBlock);
    }

    void addBlockPoS(const std::vector<Transaction>& txs) {
        Block newBlock(chain.size(), chain.back().getHash(), txs);
        std::string val = selectValidator();
        newBlock.validate(val);
        chain.push_back(newBlock);
    }

    bool verifyChain() {
        for (size_t i = 1; i < chain.size(); ++i)
            if (!chain[i].isValid() || chain[i-1].getHash() != chain[i].getPreviousHash())
                return false;
        return true;
    }

    // Getter pour accéder aux blocs pour affichage
    const std::vector<Block>& getChain() const { return chain; }
};

// ---------- Main ----------
int main() {
    std::vector<Validator> validators = {{"Alice", 50}, {"Bob", 30}, {"Charlie", 20}};
    Blockchain bc(validators);

    std::vector<Transaction> txs1 = {Transaction("1", "Alice", "Bob", 10)};
    bc.addBlockPoW(txs1);

    std::vector<Transaction> txs2 = {Transaction("2", "Bob", "Charlie", 5)};
    bc.addBlockPoS(txs2);

    std::cout << "\nChaîne valide ? " << (bc.verifyChain() ? "Oui" : "Non") << "\n";

    std::cout << "\n--- Blockchain ---\n";
    const auto& chain = bc.getChain();
    for (size_t i = 0; i < chain.size(); ++i) {
        std::cout << "Block " << i 
                  << ", Hash: " << chain[i].getHash() 
                  << ", PrevHash: " << chain[i].getPreviousHash() 
                  << ", MerkleRoot: " << chain[i].getMerkleRoot() << "\n";
    }

    return 0;
}
