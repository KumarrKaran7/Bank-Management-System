# Use Case Specifications — Bank Management System

## Actors
- **Bank Operator / Customer (Primary Actor):** Interacts with the console menu to create accounts, execute transactions, inspect statements, and trigger lifecycle actions.
- **Notification Service (Secondary Actor):** Receives event triggers from `Account` to dispatch Console, SMS, or Email alerts.

---

## UC-01: Open Bank Account
- **Actor:** Bank Operator / Customer
- **Preconditions:** Application is running; valid holder name and non-negative initial deposit are provided.
- **Main Flow:**
  1. User selects Option `1` (Savings), `2` (Current), or `3` (Fixed Deposit).
  2. System prompts for holder name, opening balance, and type-specific parameters (overdraft limit or tenure).
  3. System auto-generates a unique 10-digit account number (`1000000001+`), records `"Initial deposit"` (`CR`), attaches `ConsoleNotifier`, and logs an audit entry.
- **Exception Flows:**
  - *E1 (Negative Initial Balance / Zero Tenure):* Throws `InvalidAccountException` and aborts creation.

---

## UC-02: Deposit Funds
- **Actor:** Bank Operator / Customer
- **Preconditions:** At least one account exists.
- **Main Flow:**
  1. User selects Option `5` and chooses a target account.
  2. User enters a positive deposit amount.
  3. System adds the amount to `balance`, logs a `CR` entry in `TransactionManager`, and notifies subscribed `Notifier` instances.
- **Exception Flows:**
  - *E1 (Amount <= 0):* Throws `InvalidAmountException`.
  - *E2 (Target is Fixed Deposit):* `FixedAccount::deposit()` throws `OperationNotAllowedException`.

---

## UC-03: Withdraw Funds
- **Actor:** Bank Operator / Customer
- **Preconditions:** At least one account exists.
- **Main Flow:**
  1. User selects Option `6` and chooses a target account.
  2. System dispatches polymorphically to the derived `withdraw()` implementation.
  3. System deducts the amount, logs a `DR` transaction, and triggers `notifyAll()`.
- **Exception Flows:**
  - *E1 (Savings balance drops below Rs. 1,000):* Throws `MinimumBalanceException`.
  - *E2 (Current withdrawal exceeds balance + overdraft):* Throws `InsufficientBalanceException`.
  - *E3 (Fixed Deposit not yet matured):* Throws `AccountLockedException`.

---

## UC-04: View Statement & Transaction History
- **Actor:** Bank Operator / Customer
- **Preconditions:** At least one account exists.
- **Main Flow:**
  1. User selects Option `7` and picks an account.
  2. System invokes `showAccountCategory()` (`final` method) and `printStatement()`.
  3. System displays account metadata and iterates through `TransactionManager` records with timestamps.

---

## UC-05: Credit Savings Interest / Mature Fixed Deposit
- **Actor:** Bank Operator / Customer
- **Preconditions:** Target account exists.
- **Main Flow:**
  1. User selects Option `8` (Add Savings Interest) or Option `9` (Mature Fixed Deposit).
  2. System performs RTTI check using `dynamic_cast<SavingsAccount*>` or `dynamic_cast<FixedAccount*>`.
  3. System credits 4% interest or unlocks the FD (`matured = true`) and notifies observers.
- **Exception Flows:**
  - *E1 (Wrong account type selected):* `dynamic_cast` returns `nullptr`, throwing `OperationNotAllowedException`.

---

## UC-06: Compare Accounts & Update Holder Name
- **Actor:** Bank Operator / Customer
- **Preconditions:** At least two accounts exist for comparison (Option `10`); one account exists for renaming (Option `11`).
- **Main Flow:**
  1. Option `10` compares balances using overloaded friend operators `>`, `<`, and `==`.
  2. Option `11` updates `holderName` using overloaded stream extraction `cin >> *selected`.
