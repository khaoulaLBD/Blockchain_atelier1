#include <iostream>
#include <vector>
#include <string>
#include <functional>  // Pour std::hash

class MerkleTree {
private:
    std::vector<std::string> leaves;  // Les transactions (feuilles)
    std::string root;  // La racine de l'arbre

    // Fonction de hachage simple : combine deux chaînes et hache
    std::string hash(const std::string& left, const std::string& right) {
        std::hash<std::string> hasher;
        return std::to_string(hasher(left + right));  // Concatène et hache
    }

    // Construit l'arbre
    void buildTree() {
        std::vector<std::string> current = leaves;  // Commence avec les feuilles

        while (current.size() > 1) {  // Tant qu'il y a plus d'un nœud
            std::vector<std::string> nextLevel;
            for (size_t i = 0; i < current.size(); i += 2) {
                if (i + 1 < current.size()) {
                    // Paire normale
                    nextLevel.push_back(hash(current[i], current[i + 1]));
                } else {
                    // Si impair, duplique le dernier
                    nextLevel.push_back(hash(current[i], current[i]));
                }
            }
            current = nextLevel;  // Monte au niveau suivant
        }
        root = current[0];  // La racine est le dernier nœud
    }

public:
    // Constructeur : prend une liste de transactions
    MerkleTree(const std::vector<std::string>& transactions) : leaves(transactions) {
        if (leaves.empty()) {
            root = "";
        } else {
            buildTree();
        }
    }

    // Retourne la racine
    std::string getRoot() const {
        return root;
    }
};

int main() {
    // Exemple d'exécution
    std::vector<std::string> transactions = {"Tx1: Alice envoie 10 à Bob", "Tx2: Bob envoie 5 à Charlie", "Tx3: Charlie envoie 2 à Alice"};

    MerkleTree tree(transactions);
    std::cout << "Racine de l'arbre de Merkle : " << tree.getRoot() << std::endl;

    // Un autre exemple avec 4 transactions (pair)
    std::vector<std::string> transactions2 = {"TxA", "TxB", "TxC", "TxD"};
    MerkleTree tree2(transactions2);
    std::cout << "Racine pour 4 tx : " << tree2.getRoot() << std::endl;

    return 0;
}