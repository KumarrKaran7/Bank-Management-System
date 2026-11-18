#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <ctime>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <sstream>
#include <cctype>
using namespace std;

// ---------------- USER-DEFINED EXCEPTIONS ----------------
class BankException : public exception {
    string message;
public:
    explicit BankException(const string& msg) : message(msg) {}
    const char* what() const noexcept override { return message.c_str(); }
};

class InvalidAmountException : public BankException {
public:
    using BankException::BankException;
};
class InsufficientBalanceException : public BankException {
public:
    using BankException::BankException;
};
class MinimumBalanceException : public BankException {
public:
    using BankException::BankException;
};
class AccountLockedException : public BankException {
public:
    using BankException::BankException;
};
class OperationNotAllowedException : public BankException {
public:
    using BankException::BankException;
};
class InvalidAccountException : public BankException {
public:
    using BankException::BankException;
};
class InvalidPinException : public BankException {
public:
    using BankException::BankException;
};

// Input stream band ho jaye (Ctrl+D / Ctrl+Z) to program ko saaf tareeke se rokne ke liye.
struct InputClosed {};

// C++ equivalent of finally: destructor runs when scope exits.
class CleanupGuard {
public:
    ~CleanupGuard() noexcept {
        cout << "\n[Cleanup] Resources are being released.\n";
    }
};

// ---------------- SMALL HELPER FUNCTIONS ----------------
// Paise ko 2 decimal ke saath string me badalta hai (notifications ke liye).
string money(double value) {
    ostringstream os;
    os << fixed << setprecision(2) << value;
    return os.str();
}

// PIN ko seedha file me nahi likhte; uska hash save hota hai (djb2 algorithm).
unsigned long long hashPin(const string& pin) {
    unsigned long long h = 5381;
    for (char c : pin)
        h = h * 33 + static_cast<unsigned char>(c);
    return h;
}

// File format me '|' separator hai, isliye user input se '|' hata dete hain.
string cleanField(string text) {
    for (char& c : text)
        if (c == '|') c = ' ';
    return text;
}

// ---------------- NESTED CLASS SUPPORT DATA ----------------
struct Transaction {
    string description;
    double amount;
    string type;
    string timestamp;

    Transaction(const string& desc, double amt, const string& txnType)
        : description(desc), amount(amt), type(txnType) {
        time_t now = time(nullptr);
        timestamp = ctime(&now);
        if (!timestamp.empty() && timestamp.back() == '\n')
            timestamp.pop_back();
    }

    // File se load karne ke liye: purana timestamp wapas set hota hai.
    Transaction(const string& desc, double amt, const string& txnType, const string& ts)
        : description(desc), amount(amt), type(txnType), timestamp(ts) {}
};

// ---------------- NOTIFICATION HIERARCHY ----------------
class Notifier {
public:
    virtual void notify(const string& message) = 0;
    virtual ~Notifier() {}
};

class ConsoleNotifier : public Notifier {
public:
    void notify(const string& message) override {
        cout << "\n[CONSOLE ALERT] " << message << '\n';
    }
};

class SMSNotifier : public Notifier {
    string phone;
public:
    explicit SMSNotifier(const string& number) : phone(number) {}
    void notify(const string& message) override {
        cout << "\n[SMS -> " << phone << "] " << message << '\n';
    }
};

class EmailNotifier : public Notifier {
    string email;
public:
    explicit EmailNotifier(const string& address) : email(address) {}
    void notify(const string& message) override {
        cout << "\n[EMAIL -> " << email << "] " << message << '\n';
    }
};

// ---------------- TYPE CONVERSION CLASSES ----------------
// Basic-to-class: double -> Amount
class Amount {
    double value;
public:
    Amount(double v = 0) : value(v) {}
    double getValue() const { return value; }

    // Class-to-basic: Amount -> double
    operator double() const { return value; }
};

// Class-to-class: Amount -> RoundedAmount
class RoundedAmount {
    int value;
public:
    RoundedAmount(const Amount& a) : value(static_cast<int>(a.getValue() + 0.5)) {}
    void show() const { cout << "Rounded amount: Rs." << value << '\n'; }
};

// ---------------- TEMPLATE EXAMPLES ----------------
template <class T>
class Box {
    T value;
public:
    explicit Box(const T& v) : value(v) {}
    void display() const { cout << "Template Box value: " << value << '\n'; }
};

template <class T>
void showValue(const T& value) {
    cout << "Template function value: " << value << '\n';
}

// ---------------- OBJECT SLICING DEMONSTRATION ----------------
class CustomerRecord {
public:
    string name;
    explicit CustomerRecord(const string& n = "Unknown") : name(n) {}
    virtual void display() const {
        cout << "Customer: " << name << '\n';
    }
    virtual ~CustomerRecord() {}
};

class PremiumCustomerRecord : public CustomerRecord {
public:
    int rewardPoints;
    PremiumCustomerRecord(const string& n, int points)
        : CustomerRecord(n), rewardPoints(points) {}
    void display() const override {
        cout << "Premium customer: " << name
             << ", reward points: " << rewardPoints << '\n';
    }
};

// ---------------- VIRTUAL BASE CLASS DEMONSTRATION ----------------
class Identity {
protected:
    string identityName;
public:
    explicit Identity(const string& n = "") : identityName(n) {}
};

class BankCustomer : virtual public Identity {
public:
    BankCustomer(const string& n = "") : Identity(n) {}
};

class BankUser : virtual public Identity {
public:
    BankUser(const string& n = "") : Identity(n) {}
};

class VerifiedClient : public BankCustomer, public BankUser {
public:
    explicit VerifiedClient(const string& n)
        : Identity(n), BankCustomer(n), BankUser(n) {}
    void showIdentity() const {
        cout << "Virtual base class identity: " << identityName << '\n';
    }
};

// ---------------- FINAL CLASS ----------------
class AuditLogger final {
public:
    void log(const string& message) const {
        cout << "[AUDIT] " << message << '\n';
    }
};

// ---------------- ABSTRACT ACCOUNT CLASS ----------------
class Account {
public:
    // Nested class: transaction history is managed inside Account.
    class TransactionManager {
        vector<Transaction> records;
    public:
        void add(const string& desc, double amount, const string& type) {
            records.emplace_back(desc, amount, type);
        }
        // File se load hui transaction add karne ke liye.
        void add(const Transaction& t) { records.push_back(t); }

        // File me transactions save karne ke liye.
        void save(ostream& out) const {
            for (const Transaction& t : records)
                out << "TXN|" << cleanField(t.description) << '|' << t.amount << '|'
                    << t.type << '|' << t.timestamp << '\n';
        }

        // Screen (cout) ya file (ofstream), dono par history likh sakta hai.
        void display(ostream& out) const {
            if (records.empty()) {
                out << "No transactions available.\n";
                return;
            }
            out << "\n----- TRANSACTION HISTORY -----\n";
            int count = 1;
            for (const Transaction& t : records) {
                out << count++ << ". " << left << setw(26) << t.description
                    << right << " Rs." << fixed << setprecision(2) << t.amount
                    << " " << t.type << " | " << t.timestamp << '\n';
            }
        }
    };

    // Sirf file se account restore karte waqt use hota hai.
    struct RestoreTag {};

protected:
    int accountNumber;
    void adjustBalance(double amount) {
        this->balance += amount; // this pointer
    }

    // Restore constructor: initial deposit transaction add nahi karta
    // aur naya account number nahi banata.
    Account(RestoreTag, int accNo, const string& name, double bal, unsigned long long pinH)
        : accountNumber(accNo), balance(bal), pinHash(pinH), holderName(name) {
        ++activeAccounts;
    }

private:
    double balance;
    unsigned long long pinHash;
    TransactionManager transactionManager;
    vector<Notifier*> notifiers;
    string notifyType = "CONSOLE";
    string notifyContact;
    static int lastAccountNumber;
    static int activeAccounts;

    // Account number tabhi badhta hai jab saari validation pass ho jaye.
    static int takeNextNumber(double initialBalance) {
        if (initialBalance < 0)
            throw InvalidAccountException("Initial balance cannot be negative.");
        return ++lastAccountNumber;
    }

public:
    string holderName;

    // Friend functions can access private balance.
    friend bool operator>(const Account& a, const Account& b);
    friend bool operator<(const Account& a, const Account& b);
    friend bool operator==(const Account& a, const Account& b);
    friend ostream& operator<<(ostream& out, const Account& account);
    friend istream& operator>>(istream& in, Account& account);

    Account(const string& name, double initialBalance, const string& pin)
        : accountNumber(takeNextNumber(initialBalance)), balance(initialBalance),
          pinHash(hashPin(pin)), holderName(name) {
        transactionManager.add("Initial deposit", initialBalance, "CR");
        ++activeAccounts;
    }

    // Static function can be called using Account::getAccountCount().
    // Ye abhi chal rahe (active) accounts ginta hai.
    static int getAccountCount() { return activeAccounts; }

    static int getLastAccountNumber() { return lastAccountNumber; }
    static void setLastAccountNumber(int n) { lastAccountNumber = n; }

    void restoreTransaction(const Transaction& t) { transactionManager.add(t); }

    void setNotifyInfo(const string& type, const string& contact) {
        notifyType = type;
        notifyContact = contact;
    }

    bool verifyPin(const string& pin) const { return hashPin(pin) == pinHash; }

    void changePin(const string& newPin) {
        if (verifyPin(newPin))
            throw OperationNotAllowedException("New PIN must be different from the old PIN.");
        pinHash = hashPin(newPin);
        notifyAll("Your account PIN was changed successfully.");
    }

    // Derived classes apna extra data (limit, tenure, etc.) yahan se save karti hain.
    virtual string getExtraData() const { return ""; }

    // Poora account file me likhta hai.
    void saveTo(ostream& out) const {
        out << getAccountType() << '|' << accountNumber << '|' << cleanField(holderName)
            << '|' << balance << '|' << pinHash << '|' << notifyType << '|'
            << cleanField(notifyContact) << '|' << getExtraData() << '\n';
        transactionManager.save(out);
        out << "END\n";
    }

    double getBalance() const { return balance; }
    int getAccountNumber() const { return accountNumber; }

    // Kya is account me paise jama ho sakte hain / kya account band ho sakta hai.
    virtual bool acceptsDeposits() const { return true; }
    virtual bool canClose() const { return true; }

    // Method overloading: two deposit signatures.
    virtual void deposit(double amount) {
        deposit("Cash deposit", amount);
    }

    virtual void deposit(const string& description, double amount) {
        if (amount <= 0)
            throw InvalidAmountException("Deposit amount must be positive.");
        adjustBalance(amount);
        transactionManager.add(description, amount, "CR");
        notifyAll("Deposit successful. New balance: Rs." + money(getBalance()));
    }

    virtual void withdraw(double amount) {
        withdraw("Cash withdrawal", amount);
    }

    virtual void withdraw(const string& description, double amount) = 0;

    void logTransaction(const string& desc, double amount, const string& type) {
        transactionManager.add(desc, amount, type);
    }

    void subscribe(Notifier* notifier) {
        if (notifier == nullptr)
            throw InvalidAccountException("Notifier pointer cannot be null.");
        notifiers.push_back(notifier);
    }

    void notifyAll(const string& message) {
        for (Notifier* notifier : notifiers)
            notifier->notify(message);
    }

    virtual string getAccountType() const = 0;

    // final function: derived classes cannot override this function.
    virtual void showAccountCategory() const final {
        cout << "Bank account category: " << getAccountType() << '\n';
    }

    // Statement kisi bhi stream me likh sakte hain (screen ya text file).
    void writeStatement(ostream& out) const {
        out << "\n----------------------------------------\n";
        out << "Account type : " << getAccountType() << '\n';
        out << "Account no.  : " << accountNumber << '\n';
        out << "Holder name  : " << holderName << '\n';
        out << "Balance      : Rs." << fixed << setprecision(2) << balance << '\n';
        out << "----------------------------------------\n";
        transactionManager.display(out);
        out << "----------------------------------------\n";
    }

    virtual void printStatement() const {
        writeStatement(cout);
    }

    // Statement ko "statement_<accountNo>.txt" file me export karta hai.
    string exportStatement() const {
        string fileName = "statement_" + to_string(accountNumber) + ".txt";
        ofstream out(fileName);
        if (!out)
            throw BankException("Could not create the statement file.");
        time_t now = time(nullptr);
        string generated = ctime(&now);
        if (!generated.empty() && generated.back() == '\n')
            generated.pop_back();
        out << "BANK ACCOUNT STATEMENT\n";
        out << "Generated on : " << generated << '\n';
        writeStatement(out);
        if (!out)
            throw BankException("Error while writing the statement file.");
        return fileName;
    }

    virtual ~Account() {
        --activeAccounts;
        cout << "Account " << accountNumber << " closed.\n";
    }
};

int Account::lastAccountNumber = 1000000000;
int Account::activeAccounts = 0;

// ---------------- FRIEND OPERATOR DEFINITIONS ----------------
bool operator>(const Account& a, const Account& b) {
    return a.balance > b.balance;
}
bool operator<(const Account& a, const Account& b) {
    return a.balance < b.balance;
}
bool operator==(const Account& a, const Account& b) {
    return a.balance == b.balance;
}

// Stream operators overloaded using friend functions.
ostream& operator<<(ostream& out, const Account& account) {
    out << "Account No: " << account.accountNumber
        << " | " << account.getAccountType()
        << " | Holder: " << account.holderName
        << " | Balance: Rs." << fixed << setprecision(2) << account.balance;
    return out;
}
istream& operator>>(istream& in, Account& account) {
    cout << "Enter new account holder name: ";
    getline(in >> ws, account.holderName);
    return in;
}

// ---------------- SAVINGS ACCOUNT ----------------
class SavingsAccount : public Account {
    static constexpr double MIN_BALANCE = 1000.0;
    static constexpr double INTEREST_RATE = 0.04;

    // Object banne se pehle hi minimum opening balance check hota hai.
    static double checkOpening(double balance) {
        if (balance < MIN_BALANCE)
            throw MinimumBalanceException("Savings account needs at least Rs.1000 to open.");
        return balance;
    }
public:
    using Account::withdraw;

    SavingsAccount(const string& name, double balance, const string& pin)
        : Account(name, checkOpening(balance), pin) {}

    // File se restore karne ke liye.
    SavingsAccount(RestoreTag t, int no, const string& name, double bal, unsigned long long pinH)
        : Account(t, no, name, bal, pinH) {}

    string getAccountType() const override { return "Savings"; }

    void withdraw(const string& description, double amount) override {
        if (amount <= 0)
            throw InvalidAmountException("Withdrawal amount must be positive.");
        if (getBalance() - amount < MIN_BALANCE)
            throw MinimumBalanceException("Savings account must maintain Rs.1000 minimum balance.");
        adjustBalance(-amount);
        logTransaction(description, amount, "DR");
        notifyAll("Savings withdrawal successful. Balance: Rs." + money(getBalance()));
    }

    void addInterest() {
        double interest = getBalance() * INTEREST_RATE;
        adjustBalance(interest);
        logTransaction("Interest credited", interest, "CR");
        notifyAll("Interest credited: Rs." + money(interest));
    }
};

// ---------------- CURRENT ACCOUNT ----------------
class CurrentAccount : public Account {
    double overdraftLimit;
public:
    using Account::withdraw;

    CurrentAccount(const string& name, double balance, const string& pin, double limit = 5000.0)
        : Account(name, balance, pin), overdraftLimit(limit) {}

    // File se restore karne ke liye.
    CurrentAccount(RestoreTag t, int no, const string& name, double bal,
                   unsigned long long pinH, double limit)
        : Account(t, no, name, bal, pinH), overdraftLimit(limit) {}

    string getAccountType() const override { return "Current"; }

    string getExtraData() const override { return to_string(overdraftLimit); }

    void withdraw(const string& description, double amount) override {
        if (amount <= 0)
            throw InvalidAmountException("Withdrawal amount must be positive.");
        if (getBalance() - amount < -overdraftLimit)
            throw InsufficientBalanceException("Withdrawal exceeds overdraft limit.");
        adjustBalance(-amount);
        logTransaction(description, amount, "DR");
        notifyAll("Current account withdrawal successful. Balance: Rs." + money(getBalance()));
    }
};

// ---------------- FIXED DEPOSIT ACCOUNT ----------------
class FixedAccount : public Account {
    int tenureMonths;
    bool matured;

    // Object banne se pehle hi tenure check hota hai (account number waste nahi hota).
    static double checkTenure(double balance, int tenure) {
        if (tenure <= 0)
            throw InvalidAccountException("Tenure must be greater than zero.");
        return balance;
    }
public:
    using Account::deposit;

    FixedAccount(const string& name, double balance, int tenure, const string& pin)
        : Account(name, checkTenure(balance, tenure), pin),
          tenureMonths(tenure), matured(false) {}

    // File se restore karne ke liye.
    FixedAccount(RestoreTag t, int no, const string& name, double bal,
                 unsigned long long pinH, int tenure, bool isMatured)
        : Account(t, no, name, bal, pinH), tenureMonths(tenure), matured(isMatured) {}

    string getAccountType() const override { return "Fixed Deposit"; }

    string getExtraData() const override {
        return to_string(tenureMonths) + "|" + (matured ? "1" : "0");
    }

    bool acceptsDeposits() const override { return false; }
    bool canClose() const override { return matured; }

    void mature() {
        matured = true;
        notifyAll("Fixed deposit has matured and is now withdrawable.");
    }

    void deposit(const string&, double) override {
        throw OperationNotAllowedException("Fixed deposit does not accept additional deposits.");
    }

    void withdraw(const string& description, double amount) override {
        if (!matured)
            throw AccountLockedException("Fixed deposit is locked until maturity.");
        if (amount <= 0)
            throw InvalidAmountException("Withdrawal amount must be positive.");
        if (amount > getBalance())
            throw InsufficientBalanceException("Insufficient fixed deposit balance.");
        adjustBalance(-amount);
        logTransaction(description, amount, "DR");
        notifyAll("Fixed deposit withdrawal successful.");
    }

    void showTenure() const {
        cout << "FD tenure: " << tenureMonths << " months\n";
    }
};

// ---------------- INPUT HELPERS ----------------
double readAmount(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= 0)
            return value;
        if (cin.eof()) throw InputClosed();
        cout << "Invalid input. Enter a valid non-negative number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value)
            return value;
        if (cin.eof()) throw InputClosed();
        cout << "Invalid input. Enter an integer.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string readLine(const string& prompt) {
    cout << prompt;
    string text;
    if (!getline(cin >> ws, text)) throw InputClosed();
    return text;
}

// Naya PIN set karte waqt: exactly 4 digits chahiye.
string readNewPin() {
    while (true) {
        cout << "Set a 4-digit PIN: ";
        string pin;
        if (!(cin >> pin)) throw InputClosed();
        bool ok = (pin.size() == 4);
        for (char c : pin)
            if (!isdigit(static_cast<unsigned char>(c))) ok = false;
        if (ok) return pin;
        cout << "PIN must be exactly 4 digits.\n";
    }
}

// ---------------- NOTIFIER SETUP ----------------
// Account ke liye Console / SMS / Email notifier chunta hai.
void askNotifier(string& type, string& contact) {
    int n = readInt("Notification type (1 = Console, 2 = SMS, 3 = Email): ");
    if (n == 2) {
        type = "SMS";
        contact = readLine("Enter phone number: ");
    } else if (n == 3) {
        type = "EMAIL";
        contact = readLine("Enter email address: ");
    } else {
        type = "CONSOLE";
        contact = "";
    }
}

// Notifier ka object pool me rehta hai; account sirf uska pointer rakhta hai.
void attachNotifier(Account* acc, const string& type, const string& contact,
                    vector<unique_ptr<Notifier>>& pool, ConsoleNotifier& console) {
    if (type == "SMS") {
        pool.emplace_back(new SMSNotifier(contact));
        acc->subscribe(pool.back().get());
        acc->setNotifyInfo("SMS", contact);
    } else if (type == "EMAIL") {
        pool.emplace_back(new EmailNotifier(contact));
        acc->subscribe(pool.back().get());
        acc->setNotifyInfo("EMAIL", contact);
    } else {
        acc->subscribe(&console);
        acc->setNotifyInfo("CONSOLE", "");
    }
}

// ---------------- FILE SAVE / LOAD ----------------
const string DATA_FILE = "bank_data.txt";
const string DEFAULT_ADMIN_PASSWORD = "admin123";   // pehli baar ke liye, baad me badal sakte hain

vector<string> splitLine(const string& line) {
    vector<string> parts;
    string item;
    stringstream ss(line);
    while (getline(ss, item, '|'))
        parts.push_back(item);
    return parts;
}

string fieldAt(const vector<string>& f, size_t i) {
    return i < f.size() ? f[i] : "";
}

void saveAccounts(const vector<unique_ptr<Account>>& accounts, unsigned long long adminHash) {
    ofstream out(DATA_FILE);
    if (!out) {
        cout << "Warning: data file save nahi ho payi.\n";
        return;
    }
    out << "BANKDATA|2|" << Account::getLastAccountNumber() << '|' << adminHash << '\n';
    out << fixed << setprecision(6);
    for (const auto& acc : accounts)
        acc->saveTo(out);
    if (!out)
        cout << "Warning: data file likhte waqt error aaya.\n";
}

void loadAccounts(vector<unique_ptr<Account>>& accounts,
                  vector<unique_ptr<Notifier>>& pool, ConsoleNotifier& console,
                  unsigned long long& adminHash) {
    ifstream in(DATA_FILE);
    if (!in) return;  // pehli baar run: file abhi exist nahi karti

    string line;
    if (!getline(in, line)) return;
    vector<string> header = splitLine(line);
    if (header.size() < 3 || header[0] != "BANKDATA" || header[1] != "2") {
        cout << "Warning: " << DATA_FILE
             << " purane/unknown format me hai, isliye ignore ki gayi.\n";
        return;
    }

    Account* current = nullptr;
    int lastNo = 1000000000;

    try {
        lastNo = stoi(header[2]);
        if (header.size() >= 4)               // admin password ka hash (purani file me nahi hota)
            adminHash = stoull(header[3]);
        while (getline(in, line)) {
            if (line.empty()) continue;
            vector<string> f = splitLine(line);

            if (f[0] == "TXN") {
                if (current && f.size() >= 5)
                    current->restoreTransaction(Transaction(f[1], stod(f[2]), f[3], f[4]));
            }
            else if (f[0] == "END") {
                current = nullptr;
            }
            else if (f.size() >= 6) {
                int no = stoi(f[1]);
                double bal = stod(f[3]);
                unsigned long long pinH = stoull(f[4]);
                unique_ptr<Account> acc;

                if (f[0] == "Savings")
                    acc.reset(new SavingsAccount(Account::RestoreTag{}, no, f[2], bal, pinH));
                else if (f[0] == "Current" && f.size() >= 8)
                    acc.reset(new CurrentAccount(Account::RestoreTag{}, no, f[2], bal, pinH,
                                                 stod(f[7])));
                else if (f[0] == "Fixed Deposit" && f.size() >= 9)
                    acc.reset(new FixedAccount(Account::RestoreTag{}, no, f[2], bal, pinH,
                                               stoi(f[7]), f[8] == "1"));
                else
                    continue;

                attachNotifier(acc.get(), f[5], fieldAt(f, 6), pool, console);
                current = acc.get();
                if (no > lastNo) lastNo = no;
                accounts.push_back(move(acc));
            }
        }
    }
    catch (const exception& e) {
        cout << "Warning: data file me problem hai: " << e.what() << '\n';
    }

    // Naye accounts ka number purane numbers se aage se chalega.
    Account::setLastAccountNumber(lastNo);
}

// ---------------- ACCOUNT LOOKUP / LOGIN ----------------
Account* findAccount(const vector<unique_ptr<Account>>& accounts, int accNo) {
    for (const auto& a : accounts)
        if (a->getAccountNumber() == accNo)
            return a.get();
    return nullptr;
}

// Selection ke liye list: balance nahi dikhate (privacy).
void listAccountsBrief(const vector<unique_ptr<Account>>& accounts) {
    for (const auto& a : accounts)
        cout << "  " << a->getAccountNumber() << " | " << a->getAccountType()
             << " | " << a->holderName << '\n';
}

Account* selectAccount(const vector<unique_ptr<Account>>& accounts,
                       const string& prompt, bool showList = true) {
    if (accounts.empty())
        throw InvalidAccountException("No accounts exist. Create an account first.");
    if (showList) {
        cout << "Available accounts:\n";
        listAccountsBrief(accounts);
    }
    int number = readInt(prompt);
    Account* acc = findAccount(accounts, number);
    if (!acc)
        throw InvalidAccountException("No account found with that account number.");
    return acc;
}

// 3 galat PIN ke baad operation cancel.
void verifyPinOrThrow(const Account* acc) {
    for (int attempt = 1; attempt <= 3; ++attempt) {
        string pin;
        cout << "Enter PIN for account " << acc->getAccountNumber() << ": ";
        if (!(cin >> pin)) throw InputClosed();
        if (acc->verifyPin(pin))
            return;
        cout << "Wrong PIN. Attempts left: " << 3 - attempt << '\n';
    }
    throw InvalidPinException("Too many wrong PIN attempts. Operation cancelled.");
}

// Admin password 3 galat attempts ke baad operation cancel.
void verifyAdminOrThrow(unsigned long long adminHash) {
    for (int attempt = 1; attempt <= 3; ++attempt) {
        string password;
        cout << "Enter admin password: ";
        if (!(cin >> password)) throw InputClosed();
        if (hashPin(password) == adminHash)
            return;
        cout << "Wrong password. Attempts left: " << 3 - attempt << '\n';
    }
    throw InvalidPinException("Too many wrong admin password attempts. Operation cancelled.");
}

// Naya admin password: kam se kam 4 characters, bina space ke.
string readNewAdminPassword() {
    while (true) {
        cout << "Enter new admin password (min 4 characters, no spaces): ";
        string password;
        if (!(cin >> password)) throw InputClosed();
        if (password.size() >= 4)
            return password;
        cout << "Password is too short.\n";
    }
}

void addAccount(vector<unique_ptr<Account>>& accounts, unique_ptr<Account> acc,
                const string& ntype, const string& ncontact,
                vector<unique_ptr<Notifier>>& pool, ConsoleNotifier& console,
                const AuditLogger& audit, const string& auditMessage) {
    attachNotifier(acc.get(), ntype, ncontact, pool, console);
    audit.log(auditMessage);
    cout << "Created: " << *acc << '\n';
    accounts.push_back(move(acc));
}

void showMenu() {
    cout << "\n========== BANK MANAGEMENT SYSTEM ==========\n"
         << " 1. Create Savings Account\n"
         << " 2. Create Current Account\n"
         << " 3. Create Fixed Deposit Account\n"
         << " 4. Display All Accounts (admin password)\n"
         << " 5. Deposit Money\n"
         << " 6. Withdraw Money (PIN)\n"
         << " 7. Transfer Money (PIN)\n"
         << " 8. Check Balance / Statement (PIN)\n"
         << " 9. Add Savings Interest\n"
         << "10. Mature Fixed Deposit\n"
         << "11. Compare Two Accounts\n"
         << "12. Update Account Holder Name (PIN)\n"
         << "13. Close Account (PIN)\n"
         << "14. Demonstrate OOP Concepts\n"
         << "15. Change Account PIN (PIN)\n"
         << "16. Export Statement to Text File (PIN)\n"
         << "17. Change Admin Password (admin password)\n"
         << " 0. Exit\n"
         << "============================================\n";
}

int main() {
    CleanupGuard cleanup;
    AuditLogger audit;
    ConsoleNotifier console;
    vector<unique_ptr<Notifier>> notifierPool;   // notifiers ke owner
    vector<unique_ptr<Account>> accounts;        // accounts ke owner
    unsigned long long adminHash = hashPin(DEFAULT_ADMIN_PASSWORD);  // file se overwrite ho sakta hai

    // Program start hote hi purana saved data file se load karo.
    loadAccounts(accounts, notifierPool, console, adminHash);
    if (!accounts.empty())
        cout << accounts.size() << " saved account(s) loaded from " << DATA_FILE << ".\n";

    try {
        int choice;
        do {
            showMenu();
            choice = readInt("Enter your choice: ");
            bool changed = false;   // true hone par data file me save hota hai

            try {
                if (choice == 1) {
                    string name = readLine("Enter account holder name: ");
                    double balance = readAmount("Enter opening balance (min Rs.1000): Rs.");
                    string pin = readNewPin();
                    string ntype, ncontact;
                    askNotifier(ntype, ncontact);
                    unique_ptr<Account> acc(new SavingsAccount(name, balance, pin));
                    addAccount(accounts, move(acc), ntype, ncontact, notifierPool, console,
                               audit, "Savings account created.");
                    changed = true;
                }
                else if (choice == 2) {
                    string name = readLine("Enter account holder name: ");
                    double balance = readAmount("Enter opening balance: Rs.");
                    double limit = readAmount("Enter overdraft limit: Rs.");
                    string pin = readNewPin();
                    string ntype, ncontact;
                    askNotifier(ntype, ncontact);
                    unique_ptr<Account> acc(new CurrentAccount(name, balance, pin, limit));
                    addAccount(accounts, move(acc), ntype, ncontact, notifierPool, console,
                               audit, "Current account created.");
                    changed = true;
                }
                else if (choice == 3) {
                    string name = readLine("Enter account holder name: ");
                    double balance = readAmount("Enter FD amount: Rs.");
                    int tenure = readInt("Enter tenure in months: ");
                    string pin = readNewPin();
                    string ntype, ncontact;
                    askNotifier(ntype, ncontact);
                    unique_ptr<Account> acc(new FixedAccount(name, balance, tenure, pin));
                    addAccount(accounts, move(acc), ntype, ncontact, notifierPool, console,
                               audit, "Fixed deposit account created.");
                    changed = true;
                }
                else if (choice == 4) {
                    verifyAdminOrThrow(adminHash);
                    if (accounts.empty()) cout << "No accounts created yet.\n";
                    for (size_t i = 0; i < accounts.size(); ++i)
                        cout << i + 1 << ". " << *accounts[i] << '\n';
                    cout << "Active accounts: " << Account::getAccountCount() << '\n';
                }
                else if (choice == 5) {
                    Account* selected = selectAccount(accounts, "Enter account number: ");
                    double amount = readAmount("Enter deposit amount: Rs.");
                    selected->deposit("User cash deposit", amount);   // deposit ke liye PIN nahi
                    changed = true;
                }
                else if (choice == 6) {
                    Account* selected = selectAccount(accounts, "Enter account number: ");
                    verifyPinOrThrow(selected);
                    double amount = readAmount("Enter withdrawal amount: Rs.");
                    selected->withdraw("User cash withdrawal", amount);
                    changed = true;
                }
                else if (choice == 7) {
                    Account* from = selectAccount(accounts, "Enter SOURCE account number: ");
                    verifyPinOrThrow(from);
                    Account* to = selectAccount(accounts, "Enter DESTINATION account number: ", false);
                    if (from == to)
                        throw OperationNotAllowedException("Cannot transfer to the same account.");
                    if (!to->acceptsDeposits())
                        throw OperationNotAllowedException("Destination account does not accept deposits.");
                    double amount = readAmount("Enter transfer amount: Rs.");
                    // Pehle withdraw (fail ho sakta hai), phir deposit (ab fail nahi hoga).
                    from->withdraw("Transfer to " + to_string(to->getAccountNumber()), amount);
                    to->deposit("Transfer from " + to_string(from->getAccountNumber()), amount);
                    cout << "Transfer of Rs." << money(amount) << " successful.\n";
                    changed = true;
                }
                else if (choice == 8) {
                    Account* selected = selectAccount(accounts, "Enter account number: ");
                    verifyPinOrThrow(selected);
                    selected->showAccountCategory();
                    selected->printStatement();
                }
                else if (choice == 9) {
                    Account* selected = selectAccount(accounts, "Enter account number: ");
                    SavingsAccount* savings = dynamic_cast<SavingsAccount*>(selected);
                    if (!savings)
                        throw OperationNotAllowedException("Interest is available only for Savings accounts.");
                    savings->addInterest(); // pointer to derived class
                    changed = true;
                }
                else if (choice == 10) {
                    Account* selected = selectAccount(accounts, "Enter account number: ");
                    FixedAccount* fixed = dynamic_cast<FixedAccount*>(selected);
                    if (!fixed)
                        throw OperationNotAllowedException("Maturity option is only for Fixed Deposit accounts.");
                    fixed->mature();
                    fixed->showTenure();
                    changed = true;
                }
                else if (choice == 11) {
                    if (accounts.size() < 2)
                        throw InvalidAccountException("Create at least two accounts first.");
                    Account* first = selectAccount(accounts, "Enter first account number: ");
                    Account* second = selectAccount(accounts, "Enter second account number: ", false);
                    if (*first > *second)
                        cout << "First account has a higher balance.\n";
                    else if (*first < *second)
                        cout << "Second account has a higher balance.\n";
                    else
                        cout << "Both accounts have equal balances.\n";
                }
                else if (choice == 12) {
                    Account* selected = selectAccount(accounts, "Enter account number: ");
                    verifyPinOrThrow(selected);
                    cin >> *selected; // friend stream extraction operator
                    changed = true;
                }
                else if (choice == 13) {
                    Account* selected = selectAccount(accounts, "Enter account number to close: ");
                    verifyPinOrThrow(selected);
                    if (!selected->canClose())
                        throw AccountLockedException("Fixed deposit cannot be closed before maturity.");
                    if (selected->getBalance() < 0)
                        throw OperationNotAllowedException("Clear the overdraft before closing this account.");
                    cout << "Rs." << money(selected->getBalance())
                         << " will be paid out in cash. Confirm closing? (y/n): ";
                    char answer;
                    if (!(cin >> answer)) throw InputClosed();
                    if (answer == 'y' || answer == 'Y') {
                        for (size_t i = 0; i < accounts.size(); ++i) {
                            if (accounts[i].get() == selected) {
                                accounts.erase(accounts.begin() + i);  // destructor chalega
                                break;
                            }
                        }
                        audit.log("Account closed.");
                        changed = true;
                    } else {
                        cout << "Account closing cancelled.\n";
                    }
                }
                else if (choice == 14) {
                    cout << "\n--- OOP DEMONSTRATIONS ---\n";

                    // Basic-to-class, class-to-basic and class-to-class casting.
                    Amount a = 1250.75;
                    double basicValue = a;
                    RoundedAmount rounded = a;
                    cout << "Basic-to-class value: Rs." << a.getValue() << '\n';
                    cout << "Class-to-basic value: " << basicValue << '\n';
                    rounded.show();

                    // Class template and function template.
                    Box<string> textBox("Banking Project");
                    textBox.display();
                    showValue<int>(2026);

                    // Object slicing: derived object copied into base object by value.
                    PremiumCustomerRecord premium("Student Customer", 150);
                    CustomerRecord sliced = premium;
                    cout << "Derived object: ";
                    premium.display();
                    cout << "After object slicing: ";
                    sliced.display();

                    // Virtual base class avoids duplicate Identity base.
                    VerifiedClient client("Verified Bank Client");
                    client.showIdentity();

                    // this pointer is used in Account::adjustBalance().
                    cout << "The this pointer refers to the current object inside member functions.\n";

                    // Runtime polymorphism through base pointer.
                    if (!accounts.empty()) {
                        Account* basePointer = accounts[0].get();
                        cout << "Base pointer calls derived implementation: "
                             << basePointer->getAccountType() << '\n';
                    }

                    // Method overloading.
                    cout << "Deposit is overloaded: deposit(amount) and deposit(description, amount).\n";
                    cout << "Stream operators << and >> are overloaded as friend functions.\n";
                    cout << "Static function active account count: " << Account::getAccountCount() << '\n';
                    cout << "final class AuditLogger prevents further inheritance; final method cannot be overridden.\n";
                    cout << "Notifier hierarchy (Console/SMS/Email) shows runtime polymorphism.\n";
                    cout << "File handling: data is saved in " << DATA_FILE << " and loaded at startup.\n";
                    cout << "Same writeStatement(ostream&) code prints on screen (cout) and writes to a file (ofstream).\n";
                    audit.log("OOP demonstration completed.");
                }
                else if (choice == 15) {
                    Account* selected = selectAccount(accounts, "Enter account number: ");
                    verifyPinOrThrow(selected);
                    string newPin = readNewPin();
                    selected->changePin(newPin);
                    cout << "PIN changed successfully.\n";
                    audit.log("PIN changed.");
                    changed = true;
                }
                else if (choice == 16) {
                    Account* selected = selectAccount(accounts, "Enter account number: ");
                    verifyPinOrThrow(selected);
                    string fileName = selected->exportStatement();
                    cout << "Statement saved to " << fileName << '\n';
                    audit.log("Statement exported.");
                }
                else if (choice == 17) {
                    verifyAdminOrThrow(adminHash);
                    adminHash = hashPin(readNewAdminPassword());
                    cout << "Admin password changed successfully.\n";
                    audit.log("Admin password changed.");
                    changed = true;
                }
                else if (choice != 0) {
                    cout << "Invalid menu choice.\n";
                }

                // Jis operation se data badla, use turant file me save karo.
                if (changed)
                    saveAccounts(accounts, adminHash);
            }
            // Derived exceptions must be caught before their base class.
            catch (const InvalidAmountException& e) {
                cout << "[InvalidAmountException] " << e.what() << '\n';
            }
            catch (const MinimumBalanceException& e) {
                cout << "[MinimumBalanceException] " << e.what() << '\n';
            }
            catch (const InsufficientBalanceException& e) {
                cout << "[InsufficientBalanceException] " << e.what() << '\n';
            }
            catch (const AccountLockedException& e) {
                cout << "[AccountLockedException] " << e.what() << '\n';
            }
            catch (const OperationNotAllowedException& e) {
                cout << "[OperationNotAllowedException] " << e.what() << '\n';
            }
            catch (const InvalidAccountException& e) {
                cout << "[InvalidAccountException] " << e.what() << '\n';
            }
            catch (const InvalidPinException& e) {
                cout << "[InvalidPinException] " << e.what() << '\n';
            }
            catch (const BankException& e) {
                cout << "[Base BankException handler] " << e.what() << '\n';
            }
            catch (const exception& e) {
                cout << "[Standard exception handler] " << e.what() << '\n';
            }

        } while (choice != 0);
    }
    catch (const InputClosed&) {
        cout << "\nInput closed. Exiting.\n";
    }
    catch (const BankException& e) {
        cout << "[Main BankException handler] " << e.what() << '\n';
    }
    catch (const exception& e) {
        cout << "[Main standard exception handler] " << e.what() << '\n';
    }

    cout << "\nThank you for using Bank Management System.\n";
    return 0;
}
