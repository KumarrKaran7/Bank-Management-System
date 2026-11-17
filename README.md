# Bank Management System (C++)

A console-based **Bank Management System** built in modern C++ for **CS201**, demonstrating core and advanced **Object-Oriented Programming (OOP)** principles, polymorphic account hierarchies, custom exception handling, operator overloading, and observer-based transaction notifications[cite: 2, 3].

---

## Key Features

- **Multi-Account Architecture:** Supports **Savings** (4% interest, Rs. 1,000 minimum balance), **Current** (configurable overdraft limit), and **Fixed Deposit** (tenure lock until maturity) accounts.
- **Real-Time Transaction Ledger:** Tracks credits (`CR`), debits (`DR`), descriptions, and system timestamps per account via a nested `TransactionManager`.
- **Observer Notification System:** Extensible `Notifier` interface (`ConsoleNotifier`, `SMSNotifier`, `EmailNotifier`) triggered automatically on account activity.
- **Custom Exception Hierarchy:** Dedicated domain exceptions (`InsufficientBalanceException`, `MinimumBalanceException`, `AccountLockedException`, `OperationNotAllowedException`, `InvalidAmountException`, `InvalidAccountException`) inheriting from `BankException`.
- **Interactive OOP Sandbox (Menu Option 12):** Built-in demonstration of type conversions, templates, object slicing, virtual base classes, `final` classes/methods, and runtime polymorphism.

---

## Class Architecture

```mermaid
classDiagram
    class Account {
        <<abstract>>
        #int accountNumber
        -double balance
        +string holderName
        +deposit(amount)
        +withdraw(desc, amount)*
        +printStatement()
        +getAccountType()*
    }
    class SavingsAccount {
        -double MIN_BALANCE
        -double INTEREST_RATE
        +addInterest()
    }
    class CurrentAccount {
        -double overdraftLimit
    }
    class FixedAccount {
        -int tenureMonths
        -bool matured
        +mature()
    }
    class Notifier {
        <<interface>>
        +notify(message)*
    }

    Account <|-- SavingsAccount
    Account <|-- CurrentAccount
    Account <|-- FixedAccount
    Account o-- Notifier : notifies
    Account *-- TransactionManager : nested class
```

---

## OOP Concepts Implemented

| OOP Concept | Implementation in Codebase |
| :--- | :--- |
| **Abstraction & Pure Virtual Functions** | `Account` (`withdraw`, `getAccountType`) and `Notifier` (`notify`) |
| **Inheritance & Runtime Polymorphism** | `SavingsAccount`, `CurrentAccount`, `FixedAccount` managed via `vector<unique_ptr<Account>>` |
| **Virtual Base Class (Diamond Problem)** | `BankCustomer` and `BankUser` virtually inherit from `Identity` into `VerifiedClient` |
| **Encapsulation & Nested Classes** | Private `balance` and nested `Account::TransactionManager` storing `Transaction` records |
| **RAII / Deterministic Cleanup** | `CleanupGuard` destructor releases resources automatically on scope exit |
| **Custom Exception Hierarchy** | `BankException` base class with 6 specialized derived exception classes |
| **Operator Overloading (`friend`)** | Relational (`>`, `<`, `==`) for balance comparison and stream (`<<`, `>>`) operators |
| **Function Overloading** | `deposit(double)` and `deposit(const string&, double)` |
| **Type Conversions** | `Amount` (basic-to-class, class-to-basic) and `RoundedAmount` (class-to-class) |
| **Templates** | Class template `Box<T>` and function template `showValue<T>()` |
| **`final` Specifier** | `AuditLogger final` class and `Account::showAccountCategory() const final` |
| **RTTI (`dynamic_cast`)** | Safe downcasting from `Account*` to `SavingsAccount*` and `FixedAccount*` |
| **Object Slicing Demonstration** | `PremiumCustomerRecord` assigned by value to `CustomerRecord` in Option 12 |

---
## Program Output & Screenshots

### 1. Main Menu & Account Creation
![Main Menu and Account Creation](screenshots/menu.png)

### 2. Account Statement & Notifications
![Account Statement and Transaction History](screenshots/statement.png)

### 3. OOP Concepts Demonstration
![OOP Concepts Output](screenshots/oop-demo.png)

## Build & Run Instructions

### Prerequisites
- A C++14 or C++17 compatible compiler (`g++`, `clang++`, or MSVC)

### Using `g++` (Linux / macOS / Windows MinGW)
```bash
g++ -std=c++17 -Wall -Wextra src/main.cpp -o bank_system
./bank_system
```

### Using `Makefile`
```bash
make run
```

---

## Team Members & Contributions

| Role | Name | Roll Number | GitHub | Module / Responsibility |
| :--- | :--- | :--- | :--- | :--- |
| **Team Leader** | Vashvik Vyas | `20251501172` | — | Core `Account` Hierarchy & Polymorphism |
| **Member** | Karan Kumar Saroj | `20251502039` | [@KumarrKaran7](https://github.com/KumarrKaran7) | Repository Setup, Build Configuration (`Makefile`) & CLI Integration |
| **Member** | Sarthak Rawat | `20251501155` | [@sarthakdev01](https://github.com/sarthakdev01) | Custom Exception Hierarchy, RTTI & Input Validation |
| **Member** | Ravaliya Raj Mahendrabhai | `20251501142` | — | `TransactionManager` Ledger & `Notifier` Observer System |
| **Member** | Vikas Chand Bairwa | `20251501175` | — | Operator Overloading, Templates & Type Conversions |
