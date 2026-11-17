# Noun-Verb Analysis — Bank Management System

## 1. Problem Statement
A console-based Bank Management System allows customers and bank operators to create and manage **Savings**, **Current**, and **Fixed Deposit** accounts. Users can deposit funds, withdraw funds subject to account-specific rules (minimum balance, overdraft limit, or maturity lock), calculate savings interest, mature fixed deposits, compare account balances, update account holder details, view timestamped transaction statements, and receive multi-channel alerts (`Console`, `SMS`, `Email`) while recording audit logs and handling domain exceptions safely.

---

## 2. Noun Extraction (Candidate Classes & Attributes)

| Noun / Phrase | Category | Decision & Mapping in C++ Codebase |
| :--- | :--- | :--- |
| **Bank Management System** | System Boundary | Main CLI controller (`main()`, `showMenu()`) |
| **Account** | Abstract Base Class | `Account` (encapsulates `accountNumber`, `balance`, `holderName`) |
| **Savings Account** | Concrete Subclass | `SavingsAccount` (`MIN_BALANCE = 1000.0`, `INTEREST_RATE = 0.04`) |
| **Current Account** | Concrete Subclass | `CurrentAccount` (`overdraftLimit`) |
| **Fixed Deposit Account** | Concrete Subclass | `FixedAccount` (`tenureMonths`, `matured`) |
| **Transaction** | Data Entity / Struct | `Transaction` (`description`, `amount`, `type`, `timestamp`) |
| **Transaction Manager** | Nested Helper Class | `Account::TransactionManager` (maintains `vector<Transaction>`) |
| **Notifier / Alert** | Abstract Interface | `Notifier` (`ConsoleNotifier`, `SMSNotifier`, `EmailNotifier`) |
| **Audit Logger** | Utility (`final` Class) | `AuditLogger` (records system events via `log()`) |
| **Bank Exception** | Exception Hierarchy | `BankException` and 6 derived domain exceptions |
| **Verified Client** | Multiple-Inheritance Entity | `VerifiedClient` inheriting virtually from `Identity` |
| **Amount / RoundedAmount** | Value / Conversion Types | `Amount`, `RoundedAmount` (custom type conversions) |
| **Cleanup Guard** | RAII Resource Manager | `CleanupGuard` (deterministic scope-exit cleanup) |
| **Account Number / Balance / Holder Name** | Attributes | Member variables of `Account` |

---

## 3. Verb Extraction (Candidate Methods & Responsibilities)

| Verb / Action Phrase | Assigned Class | C++ Method / Operator |
| :--- | :--- | :--- |
| Create Savings / Current / FD account | `SavingsAccount`, `CurrentAccount`, `FixedAccount` | Constructors + `Account::Account()` |
| Deposit money | `Account`, `FixedAccount` | `deposit(double)`, `deposit(string, double)` |
| Withdraw money | `Account`, `SavingsAccount`, `CurrentAccount`, `FixedAccount` | Pure virtual `withdraw(string, double)` |
| Add / credit interest | `SavingsAccount` | `addInterest()` |
| Mature fixed deposit | `FixedAccount` | `mature()`, `showTenure()` |
| Print statement / display history | `Account`, `TransactionManager` | `printStatement()`, `TransactionManager::display()` |
| Record / log transaction | `Account`, `TransactionManager` | `logTransaction()`, `TransactionManager::add()` |
| Subscribe & notify user | `Account`, `Notifier` | `subscribe(Notifier*)`, `notifyAll()`, `notify()` |
| Compare two accounts | `Account` | Overloaded `operator>`, `operator<`, `operator==` |
| Display / update account details | `Account` | Overloaded `operator<<`, `operator>>` |
| Count total created accounts | `Account` | Static `Account::getAccountCount()` |
| Log system audit trail | `AuditLogger` | `AuditLogger::log()` |
