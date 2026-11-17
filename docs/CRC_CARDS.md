# Class-Responsibility-Collaborator (CRC) Cards

### 1. `Account` (Abstract Base Class)
| Responsibilities | Collaborators |
| :--- | :--- |
| Maintain core account state (`accountNumber`, `balance`, `holderName`) | `TransactionManager` |
| Enforce positive deposit amounts and record credit entries | `Notifier` |
| Define pure virtual interface for `withdraw()` and `getAccountType()` | `SavingsAccount`, `CurrentAccount`, `FixedAccount` |
| Manage observer subscriptions and broadcast notifications | `BankException` |
| Print formatted account statements and support relational/stream operators | |

---

### 2. `SavingsAccount` (Inherits from `Account`)
| Responsibilities | Collaborators |
| :--- | :--- |
| Enforce minimum balance constraint (`Rs. 1000.0`) on withdrawals | `Account` |
| Compute and credit annual interest (`4%`) to balance | `MinimumBalanceException` |
| Log debit/credit transactions and trigger alerts | `InvalidAmountException` |

---

### 3. `CurrentAccount` (Inherits from `Account`)
| Responsibilities | Collaborators |
| :--- | :--- |
| Allow withdrawals up to configured `overdraftLimit` (default `Rs. 5000.0`) | `Account` |
| Reject withdrawals exceeding available balance + overdraft | `InsufficientBalanceException` |
| Log debit transactions and notify account observers | `InvalidAmountException` |

---

### 4. `FixedAccount` (Inherits from `Account`)
| Responsibilities | Collaborators |
| :--- | :--- |
| Store deposit `tenureMonths` and track `matured` status | `Account` |
| Block additional deposits after account creation | `OperationNotAllowedException` |
| Lock withdrawals until `mature()` is executed | `AccountLockedException` |

---

### 5. `Account::TransactionManager` (Nested Class)
| Responsibilities | Collaborators |
| :--- | :--- |
| Store chronological `Transaction` records (`CR` / `DR`) | `Account` |
| Format and display itemized transaction history tables | `Transaction` |

---

### 6. `Notifier` (`ConsoleNotifier`, `SMSNotifier`, `EmailNotifier`)
| Responsibilities | Collaborators |
| :--- | :--- |
| Define polymorphic `notify(message)` contract | `Account` |
| Deliver transaction alerts via Console, SMS, or Email channels | |

---

### 7. `BankException` Hierarchy
| Responsibilities | Collaborators |
| :--- | :--- |
| Represent domain-specific banking errors (`InvalidAmount`, `MinimumBalance`, `InsufficientBalance`, `AccountLocked`, `OperationNotAllowed`, `InvalidAccount`) | `Account`, `SavingsAccount`, `CurrentAccount`, `FixedAccount`, `main()` |

---

### 8. `AuditLogger` (`final` Class)
| Responsibilities | Collaborators |
| :--- | :--- |
| Emit tamper-proof `[AUDIT]` log entries for account creation and system events | `main()` |
