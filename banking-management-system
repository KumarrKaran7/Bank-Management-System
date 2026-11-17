#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <ctime>
#include <stdexcept>
#include <limits>
#include <typeinfo>
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

// C++ equivalent of finally: destructor runs when scope exits.
class CleanupGuard {
public:
    ~CleanupGuard() noexcept {
        cout << "\n[Cleanup] Resources are being released.\n";
    }
};

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
        void display() const {
            if (records.empty()) {
                cout << "No transactions available.\n";
                return;
            }
            cout << "\n----- TRANSACTION HISTORY -----\n";
            int count = 1;
            for (const Transaction& t : records) {
                cout << count++ << ". " << left << setw(22) << t.description
                     << right << " Rs." << fixed << setprecision(2) << t.amount
                     << " " << t.type << " | " << t.timestamp << '\n';
            }
        }
    };

protected:
    int accountNumber;
    void adjustBalance(double amount) {
        this->balance += amount; // this pointer
    }

private:
    double balance;
    TransactionManager transactionManager;
    vector<Notifier*> notifiers;
    static int lastAccountNumber;

public:
    string holderName;

    // Friend functions can access private balance.
    friend bool operator>(const Account& a, const Account& b);
    friend bool operator<(const Account& a, const Account& b);
    friend bool operator==(const Account& a, const Account& b);
    friend ostream& operator<<(ostream& out, const Account& account);
    friend istream& operator>>(istream& in, Account& account);

    Account(const string& name, double initialBalance)
        : accountNumber(++lastAccountNumber), balance(initialBalance),
          holderName(name) {
        if (initialBalance < 0)
            throw InvalidAccountException("Initial balance cannot be negative.");
        transactionManager.add("Initial deposit", initialBalance, "CR");
    }

    // Static function can be called using Account::getAccountCount().
    static int getAccountCount() {
        return lastAccountNumber - 1000000000;
    }

    double getBalance() const { return balance; }
    int getAccountNumber() const { return accountNumber; }

    // Method overloading: two deposit signatures.
    virtual void deposit(double amount) {
        deposit("Cash deposit", amount);
    }

    virtual void deposit(const string& description, double amount) {
        if (amount <= 0)
            throw InvalidAmountException("Deposit amount must be positive.");
        adjustBalance(amount);
        transactionManager.add(description, amount, "CR");
        notifyAll("Deposit successful. New balance: Rs." + to_string(getBalance()));
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

    virtual void printStatement() const {
        cout << "\n----------------------------------------\n";
        cout << "Account type : " << getAccountType() << '\n';
        cout << "Account no.  : " << accountNumber << '\n';
        cout << "Holder name  : " << holderName << '\n';
        cout << "Balance      : Rs." << fixed << setprecision(2) << balance << '\n';
        cout << "----------------------------------------\n";
        transactionManager.display();
        cout << "----------------------------------------\n";
    }

    virtual ~Account() {
        cout << "Account " << accountNumber << " closed.\n";
    }
};

int Account::lastAccountNumber = 1000000000;

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
    const double MIN_BALANCE;
    const double INTEREST_RATE;
public:
    SavingsAccount(const string& name, double balance)
        : Account(name, balance), MIN_BALANCE(1000.0), INTEREST_RATE(0.04) {}

    string getAccountType() const override { return "Savings"; }

    void withdraw(const string& description, double amount) override {
        if (amount <= 0)
            throw InvalidAmountException("Withdrawal amount must be positive.");
        if (getBalance() - amount < MIN_BALANCE)
            throw MinimumBalanceException("Savings account must maintain Rs.1000 minimum balance.");
        adjustBalance(-amount);
        logTransaction(description, amount, "DR");
        notifyAll("Savings withdrawal successful. Balance: Rs." + to_string(getBalance()));
    }

    void addInterest() {
        double interest = getBalance() * INTEREST_RATE;
        adjustBalance(interest);
        logTransaction("Interest credited", interest, "CR");
        notifyAll("Interest credited: Rs." + to_string(interest));
    }
};

// ---------------- CURRENT ACCOUNT ----------------
class CurrentAccount : public Account {
    double overdraftLimit;
public:
    CurrentAccount(const string& name, double balance, double limit = 5000.0)
        : Account(name, balance), overdraftLimit(limit) {}

    string getAccountType() const override { return "Current"; }

    void withdraw(const string& description, double amount) override {
        if (amount <= 0)
            throw InvalidAmountException("Withdrawal amount must be positive.");
        if (getBalance() - amount < -overdraftLimit)
            throw InsufficientBalanceException("Withdrawal exceeds overdraft limit.");
        adjustBalance(-amount);
        logTransaction(description, amount, "DR");
        notifyAll("Current account withdrawal successful.");
    }
};

// ---------------- FIXED DEPOSIT ACCOUNT ----------------
class FixedAccount : public Account {
    int tenureMonths;
    bool matured;
public:
    FixedAccount(const string& name, double balance, int tenure)
        : Account(name, balance), tenureMonths(tenure), matured(false) {
        if (tenure <= 0)
            throw InvalidAccountException("Tenure must be greater than zero.");
    }

    string getAccountType() const override { return "Fixed Deposit"; }

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

// ---------------- INPUT VALIDATION ----------------
double readAmount(const string& prompt) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= 0)
            return value;
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
        cout << "Invalid input. Enter an integer.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void showMenu() {
    cout << "\n========== BANK MANAGEMENT SYSTEM ==========\n"
         << "1. Create Savings Account\n"
         << "2. Create Current Account\n"
         << "3. Create Fixed Deposit Account\n"
         << "4. Display All Accounts\n"
         << "5. Deposit Money\n"
         << "6. Withdraw Money\n"
         << "7. Check Balance / Statement\n"
         << "8. Add Savings Interest\n"
         << "9. Mature Fixed Deposit\n"
         << "10. Compare Two Accounts\n"
         << "11. Update Account Holder Name (>> operator)\n"
         << "12. Demonstrate OOP Concepts\n"
         << "0. Exit\n"
         << "============================================\n";
}

int main() {
    CleanupGuard cleanup;
    AuditLogger audit;
    ConsoleNotifier console;
    vector<unique_ptr<Account>> accounts;

    try {
        int choice;
        do {
            showMenu();
            choice = readInt("Enter your choice: ");

            try {
                if (choice == 1) {
                    string name;
                    cout << "Enter account holder name: ";
                    getline(cin >> ws, name);
                    double balance = readAmount("Enter opening balance: Rs.");
                    unique_ptr<Account> acc(new SavingsAccount(name, balance));
                    acc->subscribe(&console);
                    audit.log("Savings account created.");
                    cout << "Created: " << *acc << '\n';
                    accounts.push_back(move(acc));
                }
                else if (choice == 2) {
                    string name;
                    cout << "Enter account holder name: ";
                    getline(cin >> ws, name);
                    double balance = readAmount("Enter opening balance: Rs.");
                    double limit = readAmount("Enter overdraft limit: Rs.");
                    unique_ptr<Account> acc(new CurrentAccount(name, balance, limit));
                    acc->subscribe(&console);
                    audit.log("Current account created.");
                    cout << "Created: " << *acc << '\n';
                    accounts.push_back(move(acc));
                }
                else if (choice == 3) {
                    string name;
                    cout << "Enter account holder name: ";
                    getline(cin >> ws, name);
                    double balance = readAmount("Enter FD amount: Rs.");
                    int tenure = readInt("Enter tenure in months: ");
                    unique_ptr<Account> acc(new FixedAccount(name, balance, tenure));
                    acc->subscribe(&console);
                    audit.log("Fixed deposit account created.");
                    cout << "Created: " << *acc << '\n';
                    accounts.push_back(move(acc));
                }
                else if (choice == 4) {
                    if (accounts.empty()) cout << "No accounts created yet.\n";
                    for (size_t i = 0; i < accounts.size(); ++i)
                        cout << i + 1 << ". " << *accounts[i] << '\n';
                    cout << "Total accounts created: " << Account::getAccountCount() << '\n';
                }
                else if (choice == 5 || choice == 6 || choice == 7 ||
                         choice == 8 || choice == 9 || choice == 11) {
                    if (accounts.empty()) {
                        cout << "Create an account first.\n";
                        continue;
                    }
                    for (size_t i = 0; i < accounts.size(); ++i)
                        cout << i + 1 << ". " << *accounts[i] << '\n';
                    int index = readInt("Select account number from this list: ") - 1;
                    if (index < 0 || index >= static_cast<int>(accounts.size()))
                        throw InvalidAccountException("Invalid account selection.");

                    Account* selected = accounts[index].get(); // pointer to base object

                    if (choice == 5) {
                        double amount = readAmount("Enter deposit amount: Rs.");
                        selected->deposit("User cash deposit", amount);
                    }
                    else if (choice == 6) {
                        double amount = readAmount("Enter withdrawal amount: Rs.");
                        selected->withdraw("User cash withdrawal", amount);
                    }
                    else if (choice == 7) {
                        selected->showAccountCategory();
                        selected->printStatement();
                    }
                    else if (choice == 8) {
                        SavingsAccount* savings = dynamic_cast<SavingsAccount*>(selected);
                        if (!savings)
                            throw OperationNotAllowedException("Interest is available only for Savings accounts.");
                        savings->addInterest(); // pointer to derived class
                    }
                    else if (choice == 9) {
                        FixedAccount* fixed = dynamic_cast<FixedAccount*>(selected);
                        if (!fixed)
                            throw OperationNotAllowedException("Maturity option is only for Fixed Deposit accounts.");
                        fixed->mature();
                        fixed->showTenure();
                    }
                    else if (choice == 11) {
                        cin >> *selected; // friend stream extraction operator
                    }
                }
                else if (choice == 10) {
                    if (accounts.size() < 2) {
                        cout << "Create at least two accounts first.\n";
                        continue;
                    }
                    for (size_t i = 0; i < accounts.size(); ++i)
                        cout << i + 1 << ". " << *accounts[i] << '\n';
                    int first = readInt("Select first account: ") - 1;
                    int second = readInt("Select second account: ") - 1;
                    if (first < 0 || second < 0 ||
                        first >= static_cast<int>(accounts.size()) ||
                        second >= static_cast<int>(accounts.size()))
                        throw InvalidAccountException("Invalid account selection.");

                    if (*accounts[first] > *accounts[second])
                        cout << "First account has a higher balance.\n";
                    else if (*accounts[first] < *accounts[second])
                        cout << "Second account has a higher balance.\n";
                    else
                        cout << "Both accounts have equal balances.\n";
                }
                else if (choice == 12) {
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
                    cout << "Static function account count: " << Account::getAccountCount() << '\n';
                    cout << "final class AuditLogger prevents further inheritance; final method cannot be overridden.\n";
                    audit.log("OOP demonstration completed.");
                }
                else if (choice != 0) {
                    cout << "Invalid menu choice.\n";
                }
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
            catch (const BankException& e) {
                cout << "[Base BankException handler] " << e.what() << '\n';
            }
            catch (const exception& e) {
                cout << "[Standard exception handler] " << e.what() << '\n';
            }

        } while (choice != 0);
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
