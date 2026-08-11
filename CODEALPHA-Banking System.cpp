#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <cctype>

using namespace std;

const string DATA_FILE = "bank_data.txt";

// ---------------- Transaction Class ----------------
class Transaction {
public:
    string type;      // Deposit, Withdrawal, Transfer-Out, Transfer-In
    double amount;

    Transaction(string t, double amt) {
        type = t;
        amount = amt;
    }

    void display() const {
        cout << "  " << left << setw(12) << type
            << " Rs. " << fixed << setprecision(2) << amount << "\n";
    }
};

// ---------------- Account Class ----------------
class Account {
public:
    int accountNumber;
    double balance;
    vector<Transaction> history;

    Account(int accNo, double initialBalance) {
        accountNumber = accNo;
        balance = initialBalance;
    }

    void showBalance() const {
        cout << "Account #" << accountNumber << " | Balance: Rs. "
            << fixed << setprecision(2) << balance << "\n";
    }

    void showHistory() const {
        cout << "\n--- Transaction History (Account #" << accountNumber << ") ---\n";
        if (history.empty()) {
            cout << "  No transactions yet.\n";
            return;
        }
        for (const auto& t : history) {
            t.display();
        }
    }
};

// ---------------- Customer Class ----------------
class Customer {
public:
    int customerId;
    string name;
    Account account;

    Customer(int id, string customerName, int accNo, double initialBalance)
        : account(accNo, initialBalance) {
        customerId = id;
        name = customerName;
    }

    void displayInfo() const {
        cout << "\n===== Account Details =====\n";
        cout << "Customer ID : " << customerId << "\n";
        cout << "Name        : " << name << "\n";
        account.showBalance();
        account.showHistory();
    }
};

// =====================================================================
//                        FILE HANDLING FUNCTIONS
// =====================================================================

// ---------------- Read ALL customers fresh from file ----------------
// Called every time an action needs current data (not at program start).
// File format (per customer block, human-readable headings):
//   Customer ID: 1
//   Name: Sajid
//   Account Number: 1001
//   Balance: 79997.00
//   Transactions:
//     Deposit: 100000.00
//     Withdrawal: 3.00
//     Transfer-Out: 20000.00
//   ---END---
vector<Customer> readAllFromFile() {
    vector<Customer> customers;
    ifstream inFile(DATA_FILE);
    if (!inFile) {
        return customers; // file doesn't exist yet -> empty list
    }

    string line;
    int tempId = 0, tempAcc = 0;
    string tempName;

    while (getline(inFile, line)) {
        if (line.rfind("Customer ID: ", 0) == 0) {
            tempId = stoi(line.substr(13));
        }
        else if (line.rfind("Name: ", 0) == 0) {
            tempName = line.substr(6);
        }
        else if (line.rfind("Account Number: ", 0) == 0) {
            tempAcc = stoi(line.substr(16));
        }
        else if (line.rfind("Balance: ", 0) == 0) {
            double tempBal = stod(line.substr(9));
            // All 4 fields collected -> create the customer now
            customers.push_back(Customer(tempId, tempName, tempAcc, tempBal));
        }
        else if (line.rfind("  ", 0) == 0 && !customers.empty()) {
            // Indented line = a transaction, format: "  Type: Amount"
            size_t sep = line.find(": ");
            if (sep != string::npos) {
                string type = line.substr(2, sep - 2);
                double amount = stod(line.substr(sep + 2));
                customers.back().account.history.push_back(Transaction(type, amount));
            }
        }
        // "Transactions:", "---END---" and blank lines need no action
    }

    inFile.close();
    return customers;
}

// ---------------- Write the given list of customers back to file ----------------
void writeAllToFile(const vector<Customer>& customers) {
    ofstream outFile(DATA_FILE);
    if (!outFile) {
        cout << "Warning: Could not update data file.\n";
        return;
    }

    for (const auto& c : customers) {
        outFile << "Customer ID: " << c.customerId << "\n";
        outFile << "Name: " << c.name << "\n";
        outFile << "Account Number: " << c.account.accountNumber << "\n";
        outFile << "Balance: " << fixed << setprecision(2) << c.account.balance << "\n";
        outFile << "Transactions:\n";

        for (const auto& t : c.account.history) {
            outFile << "  " << t.type << ": " << fixed << setprecision(2) << t.amount << "\n";
        }

        outFile << "---END---\n\n";
    }

    outFile.close();
}

// ---------------- Helper: find a customer by account number in a list ----------------
Customer* findByAccount(vector<Customer>& customers, int accNo) {
    for (auto& c : customers) {
        if (c.account.accountNumber == accNo) return &c;
    }
    return nullptr;
}

// =====================================================================
//                    SAFE INPUT HELPERS (prevent infinite loops)
// =====================================================================
// If the user types letters/symbols where a number is expected, cin
// fails and gets "stuck" repeating the same prompt forever. These
// helpers detect that, clear the error, discard the bad input, and
// ask again -- instead of looping infinitely.

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail()) {
            cin.clear();                                         // reset error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // discard bad input
            cout << "Error: Please enter a valid whole number.\n";
        }
        else {
            return value;
        }
    }
}

double readDouble(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Error: Please enter a valid number.\n";
        }
        else {
            return value;
        }
    }
}

// ---------------- Helper: check that a name contains only letters/spaces ----------------
bool isValidName(const string& name) {
    if (name.empty()) return false;

    bool hasLetter = false;
    for (char ch : name) {
        if (isalpha(static_cast<unsigned char>(ch))) {
            hasLetter = true;
        }
        else if (!isspace(static_cast<unsigned char>(ch))) {
            return false; // found a digit or symbol -> invalid
        }
    }
    return hasLetter; // must contain at least one actual letter
}

// ---------------- Read a name from the user, rejecting numbers/symbols ----------------
string readName(const string& prompt) {
    string name;
    while (true) {
        cout << prompt;
        getline(cin, name);

        if (!isValidName(name)) {
            cout << "Error: Name must contain only letters (no numbers or symbols).\n";
        }
        else {
            return name;
        }
    }
}

// =====================================================================
//                        BANKING OPERATIONS
// =====================================================================

// ---------------- Create customer & account ----------------
void createCustomer() {
    // Read current data fresh from file, so we know the next available IDs
    vector<Customer> customers = readAllFromFile();

    int nextCustomerId = 1;
    int nextAccountNumber = 1001;
    for (const auto& c : customers) {
        if (c.customerId >= nextCustomerId) nextCustomerId = c.customerId + 1;
        if (c.account.accountNumber >= nextAccountNumber) nextAccountNumber = c.account.accountNumber + 1;
    }

    string name;
    double initialDeposit;

    cin.ignore();
    name = readName("Enter customer name: ");
    initialDeposit = readDouble("Enter initial deposit: ");

    if (initialDeposit < 0) {
        cout << "Error: Initial deposit cannot be negative.\n";
        return;
    }

    customers.push_back(Customer(nextCustomerId, name, nextAccountNumber, initialDeposit));
    cout << "Customer created! Customer ID: " << nextCustomerId
        << ", Account Number: " << nextAccountNumber << "\n";

    writeAllToFile(customers); // update file immediately
}

// ---------------- Deposit ----------------
void depositMoney() {
    vector<Customer> customers = readAllFromFile();

    int accNo = readInt("Enter account number: ");

    Customer* c = findByAccount(customers, accNo);
    if (!c) {
        cout << "Error: Account not found.\n";
        return;
    }

    double amount = readDouble("Enter deposit amount: ");

    if (amount <= 0) {
        cout << "Error: Deposit amount must be positive.\n";
        return;
    }

    c->account.balance += amount;
    c->account.history.push_back(Transaction("Deposit", amount));
    cout << "Success: Rs. " << fixed << setprecision(2) << amount << " deposited.\n";

    writeAllToFile(customers); // update file immediately
}

// ---------------- Withdraw ----------------
void withdrawMoney() {
    vector<Customer> customers = readAllFromFile();

    int accNo = readInt("Enter account number: ");

    Customer* c = findByAccount(customers, accNo);
    if (!c) {
        cout << "Error: Account not found.\n";
        return;
    }

    double amount = readDouble("Enter withdrawal amount: ");

    if (amount <= 0) {
        cout << "Error: Withdrawal amount must be positive.\n";
        return;
    }
    if (amount > c->account.balance) {
        cout << "Error: Insufficient balance.\n";
        return;
    }

    c->account.balance -= amount;
    c->account.history.push_back(Transaction("Withdrawal", amount));
    cout << "Success: Rs. " << fixed << setprecision(2) << amount << " withdrawn.\n";

    writeAllToFile(customers); // update file immediately
}

// ---------------- Fund transfer between accounts ----------------
void transferFunds() {
    vector<Customer> customers = readAllFromFile();

    int fromAcc = readInt("Enter sender account number: ");
    int toAcc = readInt("Enter receiver account number: ");
    double amount = readDouble("Enter amount to transfer: ");

    Customer* sender = findByAccount(customers, fromAcc);
    Customer* receiver = findByAccount(customers, toAcc);

    if (!sender || !receiver) {
        cout << "Error: One or both accounts not found.\n";
        return;
    }
    if (fromAcc == toAcc) {
        cout << "Error: Cannot transfer to the same account.\n";
        return;
    }
    if (amount <= 0) {
        cout << "Error: Transfer amount must be positive.\n";
        return;
    }
    if (amount > sender->account.balance) {
        cout << "Error: Insufficient balance.\n";
        return;
    }

    sender->account.balance -= amount;
    receiver->account.balance += amount;
    sender->account.history.push_back(Transaction("Transfer-Out", amount));
    receiver->account.history.push_back(Transaction("Transfer-In", amount));

    cout << "Success: Rs. " << fixed << setprecision(2) << amount
        << " transferred from #" << fromAcc << " to #" << toAcc << ".\n";

    writeAllToFile(customers); // update file immediately
}

// ---------------- View account info & transactions ----------------
void viewAccount() {
    vector<Customer> customers = readAllFromFile(); // fresh read, only when checking

    int accNo = readInt("Enter account number: ");

    Customer* c = findByAccount(customers, accNo);
    if (!c) {
        cout << "Error: Account not found.\n";
        return;
    }

    c->displayInfo();
}

// =====================================================================
//                              MAIN MENU
// =====================================================================
int main() {
    int choice;

    // NOTE: no data is loaded here at startup on purpose.
    // Every menu action reads fresh from the file only when it needs to.

    do {
        cout << "\n===== Banking System =====\n";
        cout << "1. Create Customer & Account\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Transfer Funds\n";
        cout << "5. View Account Info & Transactions\n";
        cout << "6. Exit\n";
        choice = readInt("Enter choice: ");

        switch (choice) {
        case 1: createCustomer(); break;
        case 2: depositMoney();   break;
        case 3: withdrawMoney();  break;
        case 4: transferFunds();  break;
        case 5: viewAccount();    break;
        case 6:
            cout << "Exiting program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 6);

    return 0;
}