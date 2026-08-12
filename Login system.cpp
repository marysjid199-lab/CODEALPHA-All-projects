#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

const string USER_FILE = "users.txt";

// ---------- Helper: check if a username already exists ----------
// File format (per user, 2 lines + blank line separator):
// Name: <username>
// Password: <password>
//
bool usernameExists(const string& username) {
    ifstream inFile(USER_FILE);
    string line;

    while (getline(inFile, line)) {
        // Look for lines starting with "Name: "
        if (line.rfind("Name: ", 0) == 0) {
            string storedUser = line.substr(6); // text after "Name: "
            if (storedUser == username) {
                inFile.close();
                return true;
            }
        }
    }
    inFile.close();
    return false;
}

// ---------- Helper: basic input validation ----------
bool isValidInput(const string& username, const string& password) {
    if (username.empty() || password.empty()) {
        cout << "Error: Username and password cannot be empty.\n";
        return false;
    }
    if (username.find(',') != string::npos || password.find(',') != string::npos) {
        cout << "Error: Username/password cannot contain commas.\n";
        return false;
    }
    if (password.length() < 4) {
        cout << "Error: Password must be at least 4 characters long.\n";
        return false;
    }
    return true;
}

// ---------- Registration function ----------
void registerUser() {
    string username, password;

    cout << "\n--- Registration ---\n";
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    if (!isValidInput(username, password)) {
        cout << "Registration failed.\n";
        return;
    }

    if (usernameExists(username)) {
        cout << "Error: Username already exists. Please choose another.\n";
        return;
    }

    // Store credentials (Name and Password on separate labeled lines)
    ofstream outFile(USER_FILE, ios::app);
    if (!outFile) {
        cout << "Error: Could not open user file for writing.\n";
        return;
    }

    outFile << "Name: " << username << "\n";
    outFile << "Password: " << password << "\n";
    outFile << "\n"; // blank line to separate records
    outFile.close();

    cout << "Registration successful! You can now log in.\n";
}

// ---------- Login function ----------
void loginUser() {
    string username, password;

    cout << "\n--- Login ---\n";
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    cin >> password;

    ifstream inFile(USER_FILE);
    if (!inFile) {
        cout << "Error: No registered users found. Please register first.\n";
        return;
    }

    string line, storedUser, storedPass;
    bool found = false;

    while (getline(inFile, line)) {
        if (line.rfind("Name: ", 0) == 0) {
            storedUser = line.substr(6);          // text after "Name: "
            getline(inFile, line);                 // next line should be Password
            if (line.rfind("Password: ", 0) == 0) {
                storedPass = line.substr(10);       // text after "Password: "
            }

            if (storedUser == username && storedPass == password) {
                found = true;
                break;
            }
        }
    }
    inFile.close();

    if (found) {
        cout << "Login successful! Welcome, " << username << ".\n";
    }
    else {
        cout << "Error: Invalid username or password.\n";
    }
}

// ---------- Main menu ----------
int main() {
    int choice;

    do {
        cout << "\n===== Login & Registration System =====\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            registerUser();
            break;
        case 2:
            loginUser();
            break;
        case 3:
            cout << "Exiting program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 3);

    return 0;
}
