#include <iostream>
#include <cstring>
#include <vector>
#include <iomanip> 
#include <algorithm> 
#include <unordered_map> 

using namespace std;

struct Transaction {
    int receiverId;   
    float amount;
    char* detail;
};

class Person {
public:
    int id;
    float balance;
    vector<Transaction> all_transactions;
    vector<Transaction> total_transaction;

    Person(int id) {
        this->id = id;
        balance = 0.0;
    }

    ~Person() {
        for (auto& trans : all_transactions) {
            delete[] trans.detail; 
        }
        for (auto& trans : total_transaction) {
            delete[] trans.detail; 
        }
    }

    void addTransaction(int receiverId, float amount, const char* detail = nullptr) {
        Transaction newTransaction;
        newTransaction.receiverId = receiverId;
        newTransaction.amount = amount;
        if (detail) {
            newTransaction.detail = new char[strlen(detail) + 1];
            strcpy(newTransaction.detail, detail);
        } else {
            newTransaction.detail = nullptr; 
        }
        auto it = std::lower_bound(all_transactions.begin(), all_transactions.end(), newTransaction,
            [](const Transaction& a, const Transaction& b) {
                return a.receiverId < b.receiverId; 
            });
        all_transactions.insert(it, newTransaction);

        bool found = false;
        for (auto& totalTrans : total_transaction) {
            if (totalTrans.receiverId == receiverId) {
                totalTrans.amount += amount; 
                found = true;
                break;
            }
        }

        if (!found) {
            Transaction newTotalTransaction;
            newTotalTransaction.receiverId = receiverId;
            newTotalTransaction.amount = amount;
            newTotalTransaction.detail = nullptr; 
            total_transaction.push_back(newTotalTransaction);
        }
    }
    float amount_giving(int receiverId){
        for (auto& totalTrans : total_transaction) {
            if (totalTrans.receiverId == receiverId) {
                return totalTrans.amount;
            }
        }
        return 0;
    }
    void print_transactions_in_detail(const unordered_map<int, string>& names) {
        cout << "All transactions for ID " << id << ":\n";
        for (const auto& transaction : all_transactions) {
            cout << "  - Gave " << transaction.amount << " to ID " << transaction.receiverId;
            if (transaction.detail) {
                cout << " (" << transaction.detail << ")";
            }
            cout << " - Receiver Name: " << names.at(transaction.receiverId) << endl;
        }
    }

    void print_transactions_in_total(const unordered_map<int, string>& names) {
        cout << "Transactions for ID " << id << " (" << names.at(id) << ") give:\n";
        for (const auto& totalTrans : total_transaction) {
            cout << "  - Gave " << totalTrans.amount << " to ID " << totalTrans.receiverId 
                 << " (" << names.at(totalTrans.receiverId) << ")\n";
        }
    }
    
};

class Node {
public:
    Person* person;
    Node* next;

    Node(Person* p) : person(p), next(nullptr) {}
};

class LinkedList {
public:
    Node* head;

    LinkedList() : head(nullptr) {}

    void addPerson(Person* p) {
        Node* newNode = new Node(p);

        if (!head || p->id < head->person->id) {
            newNode->next = head;
            head = newNode;
        } else {
            Node* current = head;
            while (current->next && current->next->person->id < p->id) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    }

    Person* findPerson(int id) {
        Node* current = head;
        while (current) {
            if (current->person->id == id) {
                return current->person;
            }
            current = current->next;
        }
        return nullptr; 
    }

    void printList() {
        Node* current = head;
        while (current) {
            cout << "ID: " << current->person->id << " ";
            current = current->next;
        }
        cout << endl;
    }

    void addTransaction_LL(int giverId, int receiverId, float amount, const char* detail = nullptr) {
        Person* giver = findPerson(giverId);
        Person* receiver = findPerson(receiverId);
        if (giver && receiver) {
            giver->addTransaction(receiverId, amount, detail);
            giver->balance -= amount; 
            receiver->balance += amount; 
        } else if (giver == nullptr) {
            cout << "Giver ID " << giverId << " not found!" << endl;
        } else {
            cout << "Receiver ID " << receiverId << " not found!" << endl;
        }
    }

    void printBalances(const unordered_map<int, string>& names) {
        Node* current = head;
        while (current) {
            cout << names.at(current->person->id) << "(" << current->person->id << ") "
                 << "Balance: " << fixed << setprecision(2) << current->person->balance << endl;
            current = current->next;
        }
    }

    void print_original_table(const unordered_map<int, string>& names, int total_amount_of_person) {
        vector<vector<float>> give_and_get_matrix(total_amount_of_person, vector<float>(total_amount_of_person, 0.0));
        for (int giverId = 0; giverId < total_amount_of_person; ++giverId) {
            Person* giver = findPerson(giverId);
            if (giver) {
                for (const auto& totalTrans : giver->total_transaction) {
                    give_and_get_matrix[giverId][totalTrans.receiverId] = totalTrans.amount;
                }
            }
        }
        cout << "             | "; 
        for (int j = 0; j < total_amount_of_person; ++j) {
            cout << setw(12) << names.at(j) << " | "; 
        }
        cout << endl << string(80, '-') << endl; 
        for (int i = 0; i < total_amount_of_person; ++i) {
            cout << setw(12) << names.at(i) << " | ";  
            for (int j = 0; j < total_amount_of_person; ++j) {
                cout << setw(12) << fixed << setprecision(2) << give_and_get_matrix[i][j] << " | ";  
            }
            cout << endl << string(80, '-') << endl; 
        }
    }
    void print_calculated_table(const unordered_map<int, string>& names, int total_amount_of_person) {
        vector<vector<float>> give_and_get_matrix(total_amount_of_person, vector<float>(total_amount_of_person, 0.0));
        for (int giverId = 0; giverId < total_amount_of_person; ++giverId) {
            Person* giver = findPerson(giverId);
            if (giver) {
                for (const auto& totalTrans : giver->total_transaction) {
                    give_and_get_matrix[giverId][totalTrans.receiverId] = totalTrans.amount;
                }
            }
        }
        for (int i = 0; i < total_amount_of_person; ++i) {
            for (int j = i + 1; j < total_amount_of_person; ++j) {
                if(give_and_get_matrix[i][j] > give_and_get_matrix[j][i] ){
                    give_and_get_matrix[i][j]=give_and_get_matrix[i][j]-give_and_get_matrix[j][i];
                    give_and_get_matrix[j][i]=0;
                }
                else{
                    give_and_get_matrix[j][i]=give_and_get_matrix[j][i]-give_and_get_matrix[i][j];
                     give_and_get_matrix[i][j]=0;
                }
            }
        }
        cout << "             | "; 
        for (int j = 0; j < total_amount_of_person; ++j) {
            cout << setw(12) << names.at(j) << " | "; 
        }
        cout << endl << string(80, '-') << endl; 
        for (int i = 0; i < total_amount_of_person; ++i) {
            cout << setw(12) << names.at(i) << " | ";  
            for (int j = 0; j < total_amount_of_person; ++j) {
                cout << setw(12) << fixed << setprecision(2) << give_and_get_matrix[i][j] << " | ";  
            }
            cout << endl << string(80, '-') << endl; 
        }
    }
    void print_greedy_table(const unordered_map<int, string>& names, int total_amount_of_person) {
        vector<vector<float>> give_and_get_matrix(total_amount_of_person, vector<float>(total_amount_of_person, 0.0));
        vector<int> debtorId;
        vector<int> creditorId;
        vector<float> balance_of_all(total_amount_of_person);
        Node* current = head;
        while (current) {
            balance_of_all[current->person->id]=current->person->balance;
            if(current->person->balance>0){
                creditorId.push_back(current->person->id);
            }
            else if(current->person->balance<0){
                debtorId.push_back(current->person->id);
            }
            current = current->next;
        }
        while (!debtorId.empty() || !creditorId.empty()) {
            int debtorIndex = debtorId[0];
            int creditorIndex = creditorId[0];

            float debtorBalance = -balance_of_all[debtorIndex]; 
            float creditorBalance = balance_of_all[creditorIndex]; 

            if (debtorBalance > creditorBalance) {
                
                give_and_get_matrix[debtorIndex][creditorIndex] += creditorBalance;
                balance_of_all[debtorIndex] += creditorBalance; 
                balance_of_all[creditorIndex] = 0; 
                creditorId.erase(creditorId.begin()); 
            } else if (debtorBalance < creditorBalance) {
                give_and_get_matrix[debtorIndex][creditorIndex] += debtorBalance;
                balance_of_all[creditorIndex] -= debtorBalance; 
                balance_of_all[debtorIndex] = 0; 
                debtorId.erase(debtorId.begin()); 
            } else {
                give_and_get_matrix[debtorIndex][creditorIndex] += debtorBalance;
                balance_of_all[debtorIndex] = 0; 
                balance_of_all[creditorIndex] = 0; 
                debtorId.erase(debtorId.begin()); 
                creditorId.erase(creditorId.begin());
            }
        }
        cout << "             | "; 
        for (int j = 0; j < total_amount_of_person; ++j) {
            cout << setw(12) << names.at(j) << " | "; 
        }
        cout << endl << string(80, '-') << endl; 
        for (int i = 0; i < total_amount_of_person; ++i) {
            cout << setw(12) << names.at(i) << " | ";  
            for (int j = 0; j < total_amount_of_person; ++j) {
                cout << setw(12) << fixed << setprecision(2) << give_and_get_matrix[i][j] << " | ";  
            }
            cout << endl << string(80, '-') << endl; 
        }
    }
};


int main() {
    LinkedList people;
    unordered_map<int, string> names; 
    cout << "-----------------------Adding People-----------------------" << endl;
    int total_amount_of_person = 0;
    people.addPerson(new Person(total_amount_of_person));
    names[total_amount_of_person] = "Andy";
    total_amount_of_person++;
    people.addPerson(new Person(total_amount_of_person));
    names[total_amount_of_person] = "Ivan";
    total_amount_of_person++;
    people.addPerson(new Person(total_amount_of_person));
    names[total_amount_of_person] = "Ron";
    total_amount_of_person++;
    people.addPerson(new Person(total_amount_of_person));
    names[total_amount_of_person] = "Jason"; 
    total_amount_of_person++;
    people.printList();

    cout << "-----------------------Adding Transaction-----------------------" << endl;
    people.addTransaction_LL(1, 0, 1685);
    people.addTransaction_LL(2, 0, 1685);
    people.addTransaction_LL(3, 0, 1685);

    people.addTransaction_LL(1, 2, 863);
    people.addTransaction_LL(0, 2, 863);
    people.addTransaction_LL(3, 2, 863);

    people.addTransaction_LL(1, 3, 555.5);
    people.addTransaction_LL(2, 3, 555.5);
    people.addTransaction_LL(0, 3, 555.5);

    cout << "-----------------------Printing Transaction-----------------------" << endl;
    people.printBalances(names);
    cout << endl;
    people.findPerson(2)->print_transactions_in_detail(names);
    cout << endl;
    people.findPerson(2)->print_transactions_in_total(names);
    cout << endl;
    cout << "-----------------------Printing Original Transactions Table-----------------------" << endl;
    people.print_original_table(names,total_amount_of_person);
    cout<<endl;
    cout << "-----------------------Printing Calculated Transactions Table-----------------------" << endl;
    people.print_calculated_table(names,total_amount_of_person);
    cout<<endl;
    cout << "-----------------------Printing Greedy Transactions Table-----------------------" << endl;
    people.print_greedy_table(names,total_amount_of_person);
    
    return 0;
}