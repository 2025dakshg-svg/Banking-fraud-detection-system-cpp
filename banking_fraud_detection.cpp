/*
 * ============================================================
 *   BANKING FRAUD DETECTION ENGINE
 *   ITM Skills University | B.Tech CSE 2025-29
 *   Data Structures & Algorithms with C++ | Semester II
 *   Case Study 153
 * ============================================================
 *
 *  Algorithms Used:
 *   1. Insertion Sort  - Real-time transaction insertion
 *   2. Bubble Sort     - Fraud ranking by risk score
 *   3. Selection Sort  - Top-K fraud case extraction
 *   4. Binary Search   - Fast transaction retrieval by ID
 *   5. Linear Search   - Fraud pattern detection
 * ============================================================
 */

#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <ctime>
#include <algorithm>
using namespace std;

// ============================================================
//  ANSI COLOR CODES (for terminal output styling)
// ============================================================
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define WHITE   "\033[37m"

// ============================================================
//  TRANSACTION STRUCTURE
// ============================================================
struct Transaction {
    int    id;
    string customerName;
    string accountNumber;
    double amount;
    string location;
    string channel;       // ATM / Mobile / Online / Card
    int    riskScore;     // 0 - 100
    string riskLevel;     // Low / Medium / High / Critical
    string timestamp;
    bool   flagged;       // true = alert generated
};

// ============================================================
//  GLOBAL TRANSACTION LIST
// ============================================================
Transaction transactions[200];
int transactionCount = 0;

// ============================================================
//  HELPER: Assign risk level string from numeric score
// ============================================================
string getRiskLevel(int score) {
    if (score >= 80) return "Critical";
    if (score >= 60) return "High";
    if (score >= 40) return "Medium";
    return "Low";
}

// ============================================================
//  HELPER: Color-coded risk level for display
// ============================================================
string coloredRisk(const string& level) {
    if (level == "Critical") return string(RED)    + BOLD + "CRITICAL" + RESET;
    if (level == "High")     return string(YELLOW) + BOLD + "HIGH    " + RESET;
    if (level == "Medium")   return string(CYAN)   +        "MEDIUM  " + RESET;
    return string(GREEN) + "LOW     " + RESET;
}

// ============================================================
//  HELPER: Print a single transaction row
// ============================================================
void printTransactionRow(const Transaction& t, int index) {
    cout << " " << setw(3)  << index + 1
         << " | " << setw(6)  << t.id
         << " | " << setw(14) << left  << t.customerName
         << " | " << setw(10) << right << fixed << setprecision(2) << t.amount
         << " | " << setw(10) << left  << t.location
         << " | " << setw(8)  << t.channel
         << " | " << setw(5)  << right << t.riskScore
         << " | " << coloredRisk(t.riskLevel)
         << " | " << (t.flagged ? string(RED) + "YES" + RESET : string(GREEN) + "NO " + RESET)
         << "\n";
}

// ============================================================
//  HELPER: Print table header
// ============================================================
void printTableHeader() {
    cout << BOLD << BLUE;
    cout << " ----+--------+----------------+------------+------------+----------+-------+----------+-------\n";
    cout << "  #  |  TXN ID | Customer       |     Amount | Location   | Channel  | Score |   Risk   | Alert\n";
    cout << " ----+--------+----------------+------------+------------+----------+-------+----------+-------\n";
    cout << RESET;
}

void printTableFooter() {
    cout << BLUE << " ----+--------+----------------+------------+------------+----------+-------+----------+-------\n" << RESET;
}

// ============================================================
//  HELPER: Print section banner
// ============================================================
void printBanner(const string& title) {
    cout << "\n" << BOLD << MAGENTA;
    cout << " ╔══════════════════════════════════════════════════════╗\n";
    cout << " ║  " << left << setw(52) << title << "║\n";
    cout << " ╚══════════════════════════════════════════════════════╝\n";
    cout << RESET;
}

// ============================================================
//  LOAD SAMPLE DATA (10 pre-loaded transactions)
// ============================================================
void loadSampleData() {
    // Pre-loaded sample transactions
    struct SampleData {
        int id; const char* name; const char* acc; double amount;
        const char* loc; const char* ch; int score;
    };

    SampleData samples[] = {
        {1001, "Rahul Sharma",   "ACC001", 15000.00, "Mumbai",    "Online",  72},
        {1002, "Priya Patel",    "ACC002",  2500.00, "Delhi",     "ATM",     35},
        {1003, "Amit Verma",     "ACC003", 85000.00, "London",    "Mobile",  91},
        {1004, "Sneha Iyer",     "ACC004",  1200.00, "Chennai",   "Card",    20},
        {1005, "Rohan Mehta",    "ACC005", 42000.00, "Dubai",     "Online",  65},
        {1006, "Kavya Reddy",    "ACC006",  3800.00, "Hyderabad", "Mobile",  48},
        {1007, "Arjun Singh",    "ACC007", 99500.00, "Moscow",    "ATM",     95},
        {1008, "Divya Nair",     "ACC008",  6700.00, "Pune",      "Card",    55},
        {1009, "Vikram Joshi",   "ACC009", 22000.00, "Bangalore", "Online",  60},
        {1010, "Ananya Gupta",   "ACC010",  900.00,  "Kolkata",   "Mobile",  15},
    };

    for (int i = 0; i < 10; i++) {
        Transaction t;
        t.id            = samples[i].id;
        t.customerName  = samples[i].name;
        t.accountNumber = samples[i].acc;
        t.amount        = samples[i].amount;
        t.location      = samples[i].loc;
        t.channel       = samples[i].ch;
        t.riskScore     = samples[i].score;
        t.riskLevel     = getRiskLevel(samples[i].score);
        t.timestamp     = "2026-06-10";
        t.flagged       = (samples[i].score >= 70);
        transactions[transactionCount++] = t;
    }
}

// ============================================================
//  ALGORITHM 1: INSERTION SORT
//  Inserts a new transaction into the already sorted list
//  Best case O(n) when list is nearly sorted
// ============================================================
void insertionSort() {
    for (int i = 1; i < transactionCount; i++) {
        Transaction key = transactions[i];
        int j = i - 1;
        // Sort in descending order of riskScore
        while (j >= 0 && transactions[j].riskScore < key.riskScore) {
            transactions[j + 1] = transactions[j];
            j--;
        }
        transactions[j + 1] = key;
    }
}

// Insert ONE new transaction at the correct position (live insertion)
void insertNewTransaction(Transaction newT) {
    // Place at end then shift into correct position
    transactions[transactionCount] = newT;
    transactionCount++;

    // Insertion sort step for the new element only
    int i = transactionCount - 1;
    Transaction key = transactions[i];
    int j = i - 1;
    while (j >= 0 && transactions[j].riskScore < key.riskScore) {
        transactions[j + 1] = transactions[j];
        j--;
    }
    transactions[j + 1] = key;

    cout << GREEN << BOLD << "\n [✓] Transaction inserted at correct risk position using Insertion Sort.\n" << RESET;
}

// ============================================================
//  ALGORITHM 2: BUBBLE SORT
//  Stable sort — preserves chronological order for equal scores
//  Used for full re-ranking after bulk updates
// ============================================================
void bubbleSort() {
    bool swapped;
    for (int i = 0; i < transactionCount - 1; i++) {
        swapped = false;
        for (int j = 0; j < transactionCount - i - 1; j++) {
            // Descending: higher risk score first
            if (transactions[j].riskScore < transactions[j + 1].riskScore) {
                Transaction temp   = transactions[j];
                transactions[j]    = transactions[j + 1];
                transactions[j + 1]= temp;
                swapped = true;
            }
        }
        if (!swapped) break; // Early exit optimization
    }
    cout << GREEN << BOLD << "\n [✓] Transactions re-ranked by fraud score using Bubble Sort (stable).\n" << RESET;
}

// ============================================================
//  ALGORITHM 3: SELECTION SORT
//  Extracts top-K highest risk transactions for investigators
//  Minimizes data movement — important for audit integrity
// ============================================================
void selectionSort(int k) {
    // Only sort first k elements (partial selection sort)
    int limit = min(k, transactionCount);
    for (int i = 0; i < limit; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < transactionCount; j++) {
            if (transactions[j].riskScore > transactions[maxIdx].riskScore)
                maxIdx = j;
        }
        if (maxIdx != i) {
            Transaction temp       = transactions[i];
            transactions[i]        = transactions[maxIdx];
            transactions[maxIdx]   = temp;
        }
    }

    printBanner("TOP FRAUD CASES — Selection Sort");
    cout << YELLOW << BOLD << " Showing top " << limit << " highest-risk transactions for immediate investigation:\n" << RESET;
    printTableHeader();
    for (int i = 0; i < limit; i++)
        printTransactionRow(transactions[i], i);
    printTableFooter();
    cout << GREEN << BOLD << "\n [✓] Top-" << limit << " fraud cases extracted using Selection Sort.\n" << RESET;
}

// ============================================================
//  ALGORITHM 4: BINARY SEARCH
//  Fast O(log n) retrieval by Transaction ID
//  Requires sorted-by-ID array — uses a temporary copy
// ============================================================
int binarySearch(int targetID) {
    // Create a temporary sorted-by-ID copy for binary search
    Transaction temp[200];
    for (int i = 0; i < transactionCount; i++)
        temp[i] = transactions[i];

    // Sort temp array by transaction ID (ascending)
    for (int i = 0; i < transactionCount - 1; i++) {
        for (int j = 0; j < transactionCount - i - 1; j++) {
            if (temp[j].id > temp[j + 1].id) {
                Transaction t = temp[j];
                temp[j]       = temp[j + 1];
                temp[j + 1]   = t;
            }
        }
    }

    // Binary Search
    int low = 0, high = transactionCount - 1, comparisons = 0;
    while (low <= high) {
        comparisons++;
        int mid = low + (high - low) / 2;
        if (temp[mid].id == targetID) {
            cout << GREEN << BOLD << "\n [✓] Transaction found in " << comparisons << " comparison(s) — O(log n) efficiency!\n" << RESET;
            return mid; // return index in temp array (for display)
        }
        else if (temp[mid].id < targetID)
            low = mid + 1;
        else
            high = mid - 1;
    }
    cout << RED << "\n [✗] Transaction ID " << targetID << " not found after " << comparisons << " comparisons.\n" << RESET;
    return -1;
}

// Search and display a transaction by ID
void searchByID(int id) {
    // Create temp sorted copy for binary search display
    Transaction temp[200];
    for (int i = 0; i < transactionCount; i++) temp[i] = transactions[i];
    for (int i = 0; i < transactionCount - 1; i++)
        for (int j = 0; j < transactionCount - i - 1; j++)
            if (temp[j].id > temp[j+1].id) { Transaction t=temp[j]; temp[j]=temp[j+1]; temp[j+1]=t; }

    int low = 0, high = transactionCount - 1, comparisons = 0, found = -1;
    while (low <= high) {
        comparisons++;
        int mid = low + (high - low) / 2;
        if (temp[mid].id == id) { found = mid; break; }
        else if (temp[mid].id < id) low = mid + 1;
        else high = mid - 1;
    }

    printBanner("BINARY SEARCH — Transaction Lookup");
    if (found != -1) {
        cout << GREEN << BOLD << " [✓] Found in " << comparisons << " comparison(s)!\n\n" << RESET;
        printTableHeader();
        printTransactionRow(temp[found], 0);
        printTableFooter();
    } else {
        cout << RED << BOLD << " [✗] Transaction ID " << id << " not found. (" << comparisons << " comparisons)\n" << RESET;
    }
}

// ============================================================
//  ALGORITHM 5: LINEAR SEARCH
//  Fraud pattern detection — scans full history
//  Handles unsorted, multi-criteria behavioral analysis
// ============================================================
void linearSearch(const string& customerName) {
    printBanner("LINEAR SEARCH — Fraud Pattern Detection");
    cout << CYAN << " Scanning transaction history for: " << BOLD << customerName << RESET << "\n\n";

    vector<int> found;
    double totalAmount = 0;
    int    highRiskCount = 0;

    for (int i = 0; i < transactionCount; i++) {
        if (transactions[i].customerName == customerName) {
            found.push_back(i);
            totalAmount += transactions[i].amount;
            if (transactions[i].riskScore >= 60) highRiskCount++;
        }
    }

    if (found.empty()) {
        cout << RED << " [✗] No transactions found for \"" << customerName << "\".\n" << RESET;
        return;
    }

    printTableHeader();
    for (int idx : found)
        printTransactionRow(transactions[idx], idx);
    printTableFooter();

    // Behavioral Analysis Summary
    cout << BOLD << YELLOW << "\n ──── Behavioral Analysis Summary ────\n" << RESET;
    cout << "  Total Transactions : " << found.size() << "\n";
    cout << "  Total Amount       : ₹" << fixed << setprecision(2) << totalAmount << "\n";
    cout << "  High/Critical Count: " << highRiskCount << "\n";

    // Pattern flags
    if (highRiskCount >= 2)
        cout << RED << BOLD << "  ⚠ PATTERN ALERT: Multiple high-risk transactions detected!\n" << RESET;
    if (found.size() >= 3 && totalAmount > 50000)
        cout << RED << BOLD << "  ⚠ PATTERN ALERT: High-frequency + large-amount behavior detected!\n" << RESET;

    cout << GREEN << BOLD << "\n [✓] Linear search scanned all " << transactionCount << " records.\n" << RESET;
}

// ============================================================
//  ADD NEW TRANSACTION (Menu Option)
// ============================================================
void addTransaction() {
    if (transactionCount >= 200) {
        cout << RED << " [!] Transaction list full (max 200).\n" << RESET;
        return;
    }

    printBanner("ADD NEW TRANSACTION — Insertion Sort");
    Transaction t;

    // Auto-generate next ID
    int maxID = 0;
    for (int i = 0; i < transactionCount; i++)
        if (transactions[i].id > maxID) maxID = transactions[i].id;
    t.id = maxID + 1;

    cout << "  New Transaction ID : " << BOLD << t.id << RESET << " (auto-assigned)\n\n";

    cout << "  Customer Name   : "; cin.ignore(); getline(cin, t.customerName);
    cout << "  Account Number  : "; getline(cin, t.accountNumber);
    cout << "  Amount (₹)      : "; cin >> t.amount;

    cout << "  Location        : "; cin.ignore(); getline(cin, t.location);

    cout << "  Channel (ATM/Mobile/Online/Card) : ";
    getline(cin, t.channel);

    cout << "  Risk Score (0-100): "; cin >> t.riskScore;
    if (t.riskScore < 0)   t.riskScore = 0;
    if (t.riskScore > 100) t.riskScore = 100;

    t.riskLevel = getRiskLevel(t.riskScore);
    t.timestamp = "2026-06-10";
    t.flagged   = (t.riskScore >= 70);

    // Use Insertion Sort to place it correctly
    insertNewTransaction(t);

    if (t.flagged) {
        cout << RED << BOLD;
        cout << "\n ┌─────────────────────────────────────────┐\n";
        cout << " │  🚨 ALERT: HIGH RISK TRANSACTION FLAGGED │\n";
        cout << " │  Score: " << t.riskScore << " | Level: " << t.riskLevel
             << " | ID: " << t.id << "          │\n";
        cout << " │  Fraud Investigation Team Notified!      │\n";
        cout << " └─────────────────────────────────────────┘\n" << RESET;
    }
}

// ============================================================
//  DISPLAY ALL TRANSACTIONS
// ============================================================
void displayAll() {
    printBanner("ALL TRANSACTIONS — Sorted by Risk Score");
    if (transactionCount == 0) {
        cout << YELLOW << "  No transactions loaded.\n" << RESET;
        return;
    }
    printTableHeader();
    for (int i = 0; i < transactionCount; i++)
        printTransactionRow(transactions[i], i);
    printTableFooter();
    cout << "  Total: " << transactionCount << " transaction(s)\n";
}

// ============================================================
//  PERFORMANCE ANALYSIS DISPLAY
// ============================================================
void showPerformanceAnalysis() {
    printBanner("PERFORMANCE ANALYSIS");
    cout << BOLD;
    cout << "\n  Algorithm       | Best Case  | Avg Case   | Worst Case | Use in System\n";
    cout << "  ----------------+------------+------------+------------+---------------------------\n";
    cout << RESET;
    cout << "  Insertion Sort  | O(n)       | O(n²)      | O(n²)      | Real-time tx insertion\n";
    cout << "  Bubble Sort     | O(n)       | O(n²)      | O(n²)      | Stable fraud re-ranking\n";
    cout << "  Selection Sort  | O(n²)      | O(n²)      | O(n²)      | Top-K fraud extraction\n";
    cout << "  Binary Search   | O(1)       | O(log n)   | O(log n)   | Fast ID-based lookup\n";
    cout << "  Linear Search   | O(1)       | O(n)       | O(n)       | Behavioral pattern scan\n";
    cout << "\n";
    cout << YELLOW << "  Note: Insertion Sort is preferred for real-time streams because\n";
    cout << "  the transaction list is always nearly sorted — giving O(n) behavior.\n" << RESET;
    cout << "\n  Space Complexity : O(1) auxiliary for all algorithms (in-place sorting)\n";
    cout << "  No. of Transactions in System: " << BOLD << transactionCount << RESET << "\n";
}

// ============================================================
//  ALERT DASHBOARD
// ============================================================
void showAlertDashboard() {
    printBanner("ALERT DASHBOARD — Flagged Transactions");
    int alertCount = 0;
    printTableHeader();
    for (int i = 0; i < transactionCount; i++) {
        if (transactions[i].flagged) {
            printTransactionRow(transactions[i], alertCount);
            alertCount++;
        }
    }
    printTableFooter();

    if (alertCount == 0)
        cout << GREEN << "  No flagged transactions. System is secure.\n" << RESET;
    else
        cout << RED << BOLD << "\n  ⚠  " << alertCount << " transaction(s) flagged for investigation!\n" << RESET;
}

// ============================================================
//  MAIN MENU
// ============================================================
void printMenu() {
    cout << "\n" << BOLD << BLUE;
    cout << " ╔══════════════════════════════════════════════════════╗\n";
    cout << " ║        BANKING FRAUD DETECTION ENGINE                ║\n";
    cout << " ║        ITM Skills University | DSA with C++          ║\n";
    cout << " ╠══════════════════════════════════════════════════════╣\n";
    cout << " ║  1. Display All Transactions (sorted by risk)        ║\n";
    cout << " ║  2. Add New Transaction     [Insertion Sort]         ║\n";
    cout << " ║  3. Re-rank All Transactions [Bubble Sort]           ║\n";
    cout << " ║  4. Extract Top Fraud Cases  [Selection Sort]        ║\n";
    cout << " ║  5. Search by Transaction ID [Binary Search]         ║\n";
    cout << " ║  6. Fraud Pattern Detection  [Linear Search]         ║\n";
    cout << " ║  7. Alert Dashboard                                  ║\n";
    cout << " ║  8. Performance Analysis                             ║\n";
    cout << " ║  9. Exit                                             ║\n";
    cout << " ╚══════════════════════════════════════════════════════╝\n";
    cout << RESET;
    cout << "  Enter your choice: ";
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    cout << BOLD << GREEN;
    cout << "\n ═══════════════════════════════════════════════════════\n";
    cout << "   BANKING FRAUD DETECTION ENGINE — INITIALIZING...\n";
    cout << " ═══════════════════════════════════════════════════════\n" << RESET;

    // Load sample data and apply initial insertion sort
    loadSampleData();
    insertionSort(); // Initial sort by risk score
    cout << GREEN << " [✓] " << transactionCount << " sample transactions loaded and sorted.\n" << RESET;

    int choice;
    do {
        printMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                displayAll();
                break;

            case 2:
                addTransaction();
                break;

            case 3:
                bubbleSort();
                displayAll();
                break;

            case 4: {
                int k;
                cout << "  How many top fraud cases to extract? (1-" << transactionCount << "): ";
                cin >> k;
                if (k < 1) k = 1;
                if (k > transactionCount) k = transactionCount;
                selectionSort(k);
                break;
            }

            case 5: {
                int id;
                cout << "  Enter Transaction ID to search: ";
                cin >> id;
                searchByID(id);
                break;
            }

            case 6: {
                string name;
                cout << "  Enter Customer Name to scan: ";
                cin.ignore();
                getline(cin, name);
                linearSearch(name);
                break;
            }

            case 7:
                showAlertDashboard();
                break;

            case 8:
                showPerformanceAnalysis();
                break;

            case 9:
                cout << BOLD << GREEN << "\n [✓] Fraud Detection Engine shut down safely. Goodbye!\n\n" << RESET;
                break;

            default:
                cout << RED << "  [!] Invalid choice. Please enter 1-9.\n" << RESET;
        }

    } while (choice != 9);

    return 0;
}

/*
 * ============================================================
 *  HOW TO COMPILE & RUN:
 *
 *  g++ -o fraud_detection banking_fraud_detection.cpp
 *  ./fraud_detection
 *
 *  On Windows (MinGW):
 *  g++ -o fraud_detection.exe banking_fraud_detection.cpp
 *  fraud_detection.exe
 *
 *  NOTE: ANSI color codes work on Linux/Mac terminal and
 *  Windows Terminal. If colors don't show on older CMD,
 *  run: chcp 65001  and enable VT100 in your terminal.
 * ============================================================
 */
