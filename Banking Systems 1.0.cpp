#include <iostream>
#include <fstream> // برای کار با فایل‌ها 
#include <string>
#include <vector> 
#include <sstream> // می‌تونی یک رشته رو مثل ورودی/خروجی بخونه یا توش بنویسه
#include <iomanip> // برای زیباسازی و کنترل دقیق خروجی
#include <cstdlib>
#include <limits> // کد قابل حمل بنویسی بدون اینکه مقدار ماکزیمم/مینیمم رو خودت حدس بزنی
using namespace std;

class Account {
public:
    int accNo = 0;
    string name;
    char type = 'C';
    int deposit = 0;

    string serialize() const {
        // فرمت: accNo|name|type|deposit\n
        // از '|' به عنوان جداکننده استفاده می‌کنیم تا فضای نام مشکلی ایجاد نکند.
        ostringstream oss;
        oss << accNo << '|' << name << '|' << type << '|' << deposit;
        return oss.str();
    }

    static bool deserialize(const string& line, Account& out) {
        // خط را با استفاده از جداکننده '|' پارس می‌کنیم
        // اگر خط نامعتبر بود false برمی‌گردانیم
        istringstream iss(line);
        string token;

        // accNo
        if (!getline(iss, token, '|')) return false;
        try {
            out.accNo = stoi(token);
        }
        catch (...) { return false; }

        // name
        if (!getline(iss, out.name, '|')) return false;

        // type
        if (!getline(iss, token, '|')) return false;
        if (token.empty()) return false;
        out.type = token[0];

        // deposit
        if (!getline(iss, token)) return false;
        try {
            out.deposit = stoi(token);
        }
        catch (...) { return false; }

        return true;
    }
};

const string FILENAME = "accounts.txt";

vector<Account> loadAllAccounts() {
    vector<Account> accounts; // یک وکتور خالی می‌سازد تا حساب‌هایی که از فایل خوانده می‌شوند را در آن ذخیره کند
    ifstream in(FILENAME);
    if (!in) return accounts; // فایل ممکن است وجود نداشته باشد — در این صورت لیست خالی برمی‌گردد

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        Account a;
        if (Account::deserialize(line, a)) {
            accounts.push_back(a);
        } // (در غیر این صورت خط را نادیده می‌گیریم (می‌توان لاگ گرفت
    }
    return accounts;
}

bool saveAllAccounts(const vector<Account>& accounts) {
    ofstream out(FILENAME, ios::trunc);
    if (!out) return false;
    for (const auto& a : accounts) {
        out << a.serialize() << '\n';
    }
    return true;
}

int generateAccNum(const vector<Account>& accounts) {
    int acc;
    bool ok;
    do {
        acc = 100 + rand() % 900;
        ok = true;
        for (const auto& a : accounts) if (a.accNo == acc) { ok = false; break; }
    } while (!ok);
    return acc;
}


void write_account();
void display_sp(int);
void modify_account(int);
void delete_account(int);
void display_all();
void deposit_withdraw(int, int);
void intro();


int main() {
    int num, chose;

    srand(static_cast<unsigned int>(time(0)));

    do {
        intro();
        cout << endl;
        cout << "\n\n\n\tMAIN MENU";
        cout << "\n\n\t01. NEW ACCOUNT                        <--|";
        cout << "\n\n\t02. DEPOSIT AMOUNT                     <--|";
        cout << "\n\n\t03. WITHDRAW AMOUNT                    <--|";
        cout << "\n\n\t04. BALANCE ENQUIRY                    <--|";
        cout << "\n\n\t05. HOLDER LIST                        <--|";
        cout << "\n\n\t06. CLOSE AN ACCOUNT                   <--|";
        cout << "\n\n\t07. MODIFY AN ACCOUNT                  <--|";
        cout << "\n\n\t08. EXIT                               <--|";
        cout << "\n\n\n\tSelect Your Option (1-8)";
        cin >> chose;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            chose = 0;
        }
        cout << endl;


        switch (chose) {

        case 1:
            write_account();
            break;

        case 2:
            cout << "\n\n\tEnter The account Number : "; cin >> num;
            deposit_withdraw(num, 1);
            break;

        case 3:
            cout << "\n\n\tEnter The account Number : "; cin >> num;
            deposit_withdraw(num, 2);
            break;

        case 4:
            cout << "\n\n\tEnter The account Number : "; cin >> num;
            display_sp(num);
            break;

        case 5:
            display_all();
            break;

        case 6:
            cout << "\n\n\tEnter The account Number : "; cin >> num;
            delete_account(num);
            break;

        case 7:
            cout << "\n\n\tEnter The account Number : "; cin >> num;
            modify_account(num);
            break;

        case 8:
            cout << "\n\nThanks for using bank management system";
            break;

        default:
            cout << "Select Your Option (1-8)";
            break;
        }

        cout << "\n\n--------------------------------------------" << endl;
        if (chose != 8) {
            system("Pause");
            system("CLS");
        }

    } while (chose != 8);

    system("CLS");
    return 0;
}


void write_account() {
    auto accounts = loadAllAccounts();

    int acc_number = generateAccNum(accounts);
    cout << "\nGenerated Account Number: " << acc_number << "\n";

    Account newAcc;
    newAcc.accNo = acc_number;

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter The Name of account Holder: ";
    getline(cin, newAcc.name);

    cout << "Enter Type of The account (C/S): ";
    cin >> newAcc.type;
    newAcc.type = static_cast<char>(toupper(newAcc.type));

    cout << "Enter The amount: ";
    while (!(cin >> newAcc.deposit)) {
        cout << "Invalid number. Enter The amount: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    accounts.push_back(newAcc);
    if (saveAllAccounts(accounts)) {
        cout << "\nAccount created successfully!\n";
    }
    else {
        cout << "\nError saving account file!\n";
    }
}

void display_sp(int n) {
    auto accounts = loadAllAccounts();
    bool found = false;
    for (const auto& a : accounts) {
        if (a.accNo == n) {
            cout << "\nAccount Number : " << a.accNo << "\n";
            cout << "Account Holder Name : " << a.name << "\n";
            cout << "Type of The account : " << a.type << "\n";
            cout << "Balance amount : " << a.deposit << "\n";
            found = true;
            break;
        }
    }
    if (!found) cout << "\nAccount number does not exist...\n";
}

void display_all() {
    auto accounts = loadAllAccounts();
    if (accounts.empty()) {
        cout << "No accounts available.\n";
        return;
    }
    cout << "\n\n\t\tACCOUNT HOLDER LIST\n\n";
    cout << "==========================================================\n";
    cout << left << setw(12) << "ACCOUNT" << setw(20) << "NAME" << setw(8) << "TYPE" << setw(10) << "BALANCE" << "\n";
    cout << "==========================================================\n";
    for (const auto& a : accounts) {
        cout << left << setw(12) << a.accNo << setw(20) << a.name << setw(8) << a.type << setw(10) << a.deposit << "\n";
    }
}

void modify_account(int n) {
    auto accounts = loadAllAccounts();
    bool found = false;
    for (auto& a : accounts) {
        if (a.accNo == n) {
            cout << "Existing record:\n\n";
            cout << "Name: " << a.name << "\nType: " << a.type << "\nBalance: " << a.deposit << "\n\n";

            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Enter The Name of account Holder: ";
            getline(cin, a.name);

            cout << "Enter Type of The account (C/S): ";
            cin >> a.type;
            a.type = static_cast<char>(toupper(a.type));

            cout << "Enter The amount: ";
            while (!(cin >> a.deposit)) {
                cout << "Invalid number. Enter The amount: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

            found = true;
            break;
        }
    }
    if (!found) {
        cout << "Record not Found!\n";
        return;
    }
    if (saveAllAccounts(accounts)) cout << "Record Updated\n";
    else cout << "Error saving file\n";
}

void delete_account(int n) {
    auto accounts = loadAllAccounts();
    auto it = remove_if(accounts.begin(), accounts.end(), [&](const Account& a) { return a.accNo == n; });
    if (it == accounts.end()) {
        cout << "Record Not Found.\n";
        return;
    }
    accounts.erase(it, accounts.end());
    if (saveAllAccounts(accounts)) cout << "Record Deleted.\n";
    else cout << "Error saving file\n";
}

void deposit_withdraw(int n, int option) {
    auto accounts = loadAllAccounts();
    bool found = false;
    for (auto& a : accounts) {
        if (a.accNo == n) {
            cout << "Account found: " << a.name << " Balance: " << a.deposit << "\n";
            int amt;
            if (option == 1) {
                cout << "Enter The amount to be deposited: ";
                while (!(cin >> amt)) {
                    cout << "Invalid number. Enter amount: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                a.deposit += amt;
            }
            else {
                cout << "Enter The amount to be withdraw: ";
                while (!(cin >> amt)) {
                    cout << "Invalid number. Enter amount: ";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
                if (amt > a.deposit) {
                    cout << "Insufficient balance!\n";
                    return;
                }
                a.deposit -= amt;
            }
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "Record Not Found!\n";
        return;
    }
    if (saveAllAccounts(accounts)) cout << "Record Updated\n";
    else cout << "Error saving file\n";
}
void intro() {
    cout << "\n\t\t BANK MANAGEMENT SYSTEM - Created by Amirmohammad\n";
}
