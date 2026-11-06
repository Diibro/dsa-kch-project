// ============================================================
// KCH-PMTS  |  Module 2 : Emergency Room Triage Engine
// Data structure used: array-backed MAX-HEAP (index starts at 0)
//   - the patient with the HIGHEST acuity score is always at index 0
// Group: 
//      Dushime Brother (dushimeb)
//      Mukantwari Francoise (mfrancoi)
//      Uwimbabazi Keza (ukeza)
// ============================================================
#pragma once

constexpr int MAX_PATIENTS = 100;

struct Patient {
     int patientID;
     int acuityScore; // 1 = low urgency ... 100 = critical
};

struct PriorityQueue {
     Patient patientHeap[MAX_PATIENTS];
     int patientCount = 0;
};

// When true, bubble_up / bubble_down print every swap they make.
// The demo turns it on for the traces; the tests keep it off.
extern bool triageTraceOn;

// ---------- Index mapping (0-indexed) ----------
int parent(int i);       // (i - 1) / 2
int left_child(int i);   // 2i + 1
int right_child(int i);  // 2i + 2

// ---------- Core heap algorithms (Question 2.1) ----------
void bubble_up(PriorityQueue* pq, int index);    // recursive sift-up
void bubble_down(PriorityQueue* pq, int index);  // recursive sift-down

// Puts the patient at index patientCount, then sifts it up.  O(log n)
void pq_insert(PriorityQueue* pq, Patient patientRecord);

// Removes and returns the most urgent patient (the root).  O(log n)
// If the queue is empty it returns a patient with ID -1.
Patient extract_max(PriorityQueue* pq);

// ---------- Heapsort (Question 2.2) ----------
// Sorts the heap array IN PLACE (ascending acuity) using only bubble_down.
// After it runs, patientHeap[0..n-1] is sorted and patientCount is 0.
// Returns n (how many patients were sorted).
int heap_sort_in_place(PriorityQueue* pq);

// ---------- Display helpers ----------
void printHeapArray(const PriorityQueue* pq);   // [ 95 | 22 | 45 ... ]
void printHeapTree(const PriorityQueue* pq);    // level-by-level tree view
