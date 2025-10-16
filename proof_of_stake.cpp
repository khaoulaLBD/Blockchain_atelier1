#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>

struct Validator {
    std::string name;
    double stake;
};

class Block {
private:
    int id;
    std::string data;
    std::string validator;

public:
    Block(int blockId, const std::string& blockData) : id(blockId), data(blockData) {}

    void validate(const std::string& val) {
        validator = val;
    }

    std::string getValidator() const { return validator; }
};

class PoSBlockchain {
private:
    std::vector<Validator> validators;
    std::vector<Block> chain;

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
    PoSBlockchain(const std::vector<Validator>& vals) : validators(vals) {
        chain.emplace_back(0, "Genesis Block");
    }

    void addBlock(const std::string& data) {
        auto start = std::chrono::high_resolution_clock::now();
        Block newBlock(chain.size(), data);
        std::string val = selectValidator();
        newBlock.validate(val);
        chain.push_back(newBlock);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Bloc " << chain.size() - 1 << " valide par " << val
                  << ", Temps: " << duration.count() << " ms\n";
    }
};

int main() {
    std::vector<Validator> validators = {{"Alice", 50}, {"Bob", 30}, {"Charlie", 20}};
    PoSBlockchain posBc(validators);

    posBc.addBlock("Transaction PoS 1"); // Exemple PoS
    // Comparaison PoW (simplifiée)
    Block powBlock(1, "Transaction PoW 1");
    auto start = std::chrono::high_resolution_clock::now();
    powBlock.mineBlock(3); // Difficulté 3
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Bloc PoW Temps: " << duration.count() << " ms\n";

    return 0;
}