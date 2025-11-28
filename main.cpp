#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>

using namespace std;

const int MAX_USERNAME_LEN = 20;
const int MAX_PASSWORD_LEN = 20;

struct User {
    string username;
    string passwordHash;
    string role; // "admin" or "user"
};

vector<User> users;
bool isLoggedIn = false;
User currentUser;

// --------- Simple password hash (for project demo only) ----------
string simpleHash(const string &password) {
    unsigned long long hash = 0;
    for (char c : password) {
        hash = hash * 131 + (unsigned char)c;
        hash %= 1000000007;
    }
    return to_string(hash);
}

// --------- Logging ----------
void logEvent(const string &event) {
    ofstream fout("logs.txt", ios::app);
    if (!fout) return;

    time_t now = time(nullptr);
    fout << ctime(&now) << " : " << event << endl;
    fout.close();
}

// --------- Load users ----------
void loadUsers() {
    users.clear();
    ifstream fin("users.txt");
    if (!fin) return;

    string u, p, r;
    while (fin >> u >> p >> r) {
        users.push_back({u, p, r});
    }
    fin.close();
}

// --------- Save users ----------
void saveUsers() {
    ofstream fout("users.txt");
    for (auto &u : users) {
        fout << u.username << " " << u.passwordHash << " " << u.role << endl;
    }
    fout.close();
}

// --------- Find user ----------
int findUserIndex(const string &username) {
    for (size_t  i = 0; i < users.size(); i++) {
        if (users[i].username == username)
            return i;
    }
    return -1;
}

// --------- Register ----------
void registerUser() {
    string username, password;
    int roleChoice;

    cout << "\n--- Register ---\n";
    cout << "Enter username: ";
    cin >> username;

    if (username.length() > MAX_USERNAME_LEN) {
        cout << "Username too long!\n";
        return;
    }

    if (findUserIndex(username) != -1) {
        cout << "User already exists!\n";
        return;
    }

    cout << "Enter password: ";
    cin >> password;

    if (password.length() > MAX_PASSWORD_LEN) {
        cout << "Password too long!\n";
        return;
    }

    cout << "Choose role (1 = Admin, 2 = User): ";
    cin >> roleChoice;

    string role = (roleChoice == 1) ? "admin" : "user";

    User u;
    u.username = username;
    u.passwordHash = simpleHash(password);
    u.role = role;

    users.push_back(u);
    saveUsers();

    cout << "User Registered Successfully!\n";
    logEvent("New user registered: " + username);
}

// --------- Generate OTP ----------
int generateOTP() {
    return rand() % 9000 + 1000;
}

// --------- Login ----------
void loginUser() {
    string username, password;
    cout << "\n--- Login ---\n";
    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    int idx = findUserIndex(username);
    if (idx == -1) {
        cout << "User not found!\n";
        logEvent("Login failed: user not found - " + username);
        return;
    }

    if (users[idx].passwordHash != simpleHash(password)) {
        cout << "Wrong password!\n";
        logEvent("Login failed: wrong password - " + username);
        return;
    }

    int otp = generateOTP();
    cout << "Your OTP is: " << otp << endl;

    int userOtp;
    cout << "Enter OTP: ";
    cin >> userOtp;

    if (otp != userOtp) {
        cout << "Wrong OTP!\n";
        logEvent("Login failed: wrong OTP - " + username);
        return;
    }

    isLoggedIn = true;
    currentUser = users[idx];
    cout << "Login Successful! Welcome " << currentUser.username << endl;
    logEvent("Login successful: " + username);
}

// --------- Logout ----------
void logoutUser() {
    if (!isLoggedIn) {
        cout << "You are not logged in!\n";
        return;
    }

    logEvent("User logged out: " + currentUser.username);
    isLoggedIn = false;
    cout << "Logged out successfully!\n";
}

// --------- View Resource ----------
void viewResource() {
    if (!isLoggedIn) {
        cout << "Please login first!\n";
        return;
    }

    cout << "Viewing protected OS resource...\n";
    logEvent("Resource viewed by: " + currentUser.username);
}

// --------- Modify Resource (Admin Only) ----------
void modifyResource() {
    if (!isLoggedIn) {
        cout << "Please login first!\n";
        return;
    }

    if (currentUser.role != "admin") {
        cout << "Access Denied! Admin only.\n";
        logEvent("Access denied (modify) for: " + currentUser.username);
        return;
    }

    cout << "Admin modified system resource.\n";
    logEvent("Resource modified by admin: " + currentUser.username);
}

// --------- View Logs (Admin Only) ----------
void viewLogs() {
    if (!isLoggedIn || currentUser.role != "admin") {
        cout << "Admin access only!\n";
        return;
    }

    ifstream fin("logs.txt");
    string line;
    while (getline(fin, line))
        cout << line << endl;

    fin.close();
}

// --------- Menu ----------
void menu() {
    cout << "\n====== Secure Authentication Module ======\n";
    cout << "1. Register\n";
    cout << "2. Login\n";
    cout << "3. View Protected Resource\n";
    cout << "4. Modify Resource (Admin Only)\n";
    cout << "5. View Logs (Admin Only)\n";
    cout << "6. Logout\n";
    cout << "7. Exit\n";
    cout << "Enter choice: ";
}

// --------- MAIN ----------
int main() {
    srand(time(0));
    loadUsers();

    int choice;
    while (true) {
        menu();
        cin >> choice;

        switch (choice) {
        case 1: registerUser(); break;
        case 2: loginUser(); break;
        case 3: viewResource(); break;
        case 4: modifyResource(); break;
        case 5: viewLogs(); break;
        case 6: logoutUser(); break;
        case 7: return 0;
        default: cout << "Invalid choice!\n";
        }
    }
}
