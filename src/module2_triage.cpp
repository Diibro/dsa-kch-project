// KCH-PMTS  |  Module 2 : Emergency Room Triage Engine (Max-Heap)
// Rule kept at all times ("max-heap dominance"):
//   every parent's acuityScore >= its children's acuityScore
#include <iostream>
#include <iomanip>
#include "module2_triage.h"

bool triageTraceOn = false;

// Index mapping for a 0-indexed array heap
int parent(int i) { return (i - 1) / 2; }
int left_child(int i) { return 2 * i + 1; }
int right_child(int i) { return 2 * i + 2; }

// Small helper: exchange two patients inside the heap array
static void swapPatients(PriorityQueue* pq, int firstPos, int secondPos) {
     Patient holder = pq->patientHeap[firstPos];
     pq->patientHeap[firstPos] = pq->patientHeap[secondPos];
     pq->patientHeap[secondPos] = holder;
}

// bubble_up (sift-up), recursive
// While the node is more urgent than its parent, swap them and continue
// from the parent's position.  Stops at the root (index 0).
void bubble_up(PriorityQueue* pq, int index) {
     if (index == 0) {
          return; // reached the root: nothing above it
     }

     int above = parent(index);
     if (pq->patientHeap[index].acuityScore > pq->patientHeap[above].acuityScore) {
          if (triageTraceOn) {
               std::cout << "      sift-up: swap " << pq->patientHeap[index].acuityScore
                         << " (idx " << index << ") <-> "
                         << pq->patientHeap[above].acuityScore
                         << " (idx " << above << ")\n";
          }
          swapPatients(pq, index, above);
          bubble_up(pq, above); // keep climbing
     }
}

// bubble_down (sift-down), recursive
// Compare the node with its two children, pick the largest of the three.
// If a child is larger, swap with it and continue from that child.
void bubble_down(PriorityQueue* pq, int index) {
     int leftPos = left_child(index);
     int rightPos = right_child(index);
     int biggestPos = index; // assume the parent is the largest

     if (leftPos < pq->patientCount &&
         pq->patientHeap[leftPos].acuityScore > pq->patientHeap[biggestPos].acuityScore) {
          biggestPos = leftPos;
     }
     if (rightPos < pq->patientCount &&
         pq->patientHeap[rightPos].acuityScore > pq->patientHeap[biggestPos].acuityScore) {
          biggestPos = rightPos;
     }

     if (biggestPos != index) {
          if (triageTraceOn) {
               std::cout << "      sift-down: swap " << pq->patientHeap[index].acuityScore
                         << " (idx " << index << ") <-> "
                         << pq->patientHeap[biggestPos].acuityScore
                         << " (idx " << biggestPos << ")\n";
          }
          swapPatients(pq, index, biggestPos);
          bubble_down(pq, biggestPos); // keep sinking
     }
}

// Q2.1 (1) - Insertion
void pq_insert(PriorityQueue* pq, Patient patientRecord) {
     if (pq->patientCount >= MAX_PATIENTS) {
          std::cout << "  [ERROR] Triage queue is full. Patient "
                    << patientRecord.patientID << " not added.\n";
          return;
     }

     // Acuity must be in the clinical range 1 (low) to 100 (critical)
     if (patientRecord.acuityScore < 1 || patientRecord.acuityScore > 100) {
          std::cout << "  [ERROR] Invalid acuity " << patientRecord.acuityScore
                    << " for patient " << patientRecord.patientID
                    << " (must be 1-100). Not added.\n";
          return;
     }

     int landingPos = pq->patientCount;          // first free cell at the bottom
     pq->patientHeap[landingPos] = patientRecord;
     pq->patientCount++;
     bubble_up(pq, landingPos);                  // restore the heap rule
}

// Q2.1 (2) - Extraction of the most urgent patient
Patient extract_max(PriorityQueue* pq) {
     if (pq->patientCount == 0) {
          std::cout << "  [ERROR] Triage queue is empty. Nothing to extract.\n";
          Patient noPatient;
          noPatient.patientID = -1;
          noPatient.acuityScore = 0;
          return noPatient;
     }

     Patient mostUrgent = pq->patientHeap[0];                     // 1. save the root
     pq->patientHeap[0] = pq->patientHeap[pq->patientCount - 1];  // 2. last leaf -> root
     pq->patientCount--;                                          // 3. shrink
     bubble_down(pq, 0);                                          // 4. fix from the top
     return mostUrgent;
}

// Heapsort, in place
// Repeat: swap root (max) with the last active cell, shrink the heap by
// one, sift the new root down. The max values pile up at the back.
// Time: O(n log n)   Extra space: O(log n) call stack (recursive bubble_down); O(1) if iterative
int heap_sort_in_place(PriorityQueue* pq) {
     int totalSorted = pq->patientCount;
     while (pq->patientCount > 1) {
          swapPatients(pq, 0, pq->patientCount - 1);
          pq->patientCount--;
          bubble_down(pq, 0);
     }
     pq->patientCount = 0; // the array is now a sorted list, not a heap
     return totalSorted;
}

// Display helpers
void printHeapArray(const PriorityQueue* pq) {
     std::cout << "  index : ";
     for (int pos = 0; pos < pq->patientCount; pos++) {
          std::cout << std::setw(4) << pos << " ";
     }
     std::cout << "\n  acuity: ";
     for (int pos = 0; pos < pq->patientCount; pos++) {
          std::cout << std::setw(4) << pq->patientHeap[pos].acuityScore << " ";
     }
     std::cout << "\n  ptID  : ";
     for (int pos = 0; pos < pq->patientCount; pos++) {
          std::cout << std::setw(4) << pq->patientHeap[pos].patientID << " ";
     }
     std::cout << "\n";
}

void printHeapTree(const PriorityQueue* pq) {
     if (pq->patientCount == 0) {
          std::cout << "  (empty heap)\n";
          return;
     }
     // Level L holds indexes from (2^L - 1) up to (2^(L+1) - 2)
     int levelStart = 0;
     int levelWidth = 1;
     int levelNumber = 0;
     while (levelStart < pq->patientCount) {
          std::cout << "  Level " << levelNumber << ": ";
          for (int pos = levelStart; pos < levelStart + levelWidth && pos < pq->patientCount; pos++) {
               std::cout << pq->patientHeap[pos].acuityScore;
               if (pos > 0) {
                    std::cout << "(parent " << pq->patientHeap[parent(pos)].acuityScore << ")";
               }
               std::cout << "   ";
          }
          std::cout << "\n";
          levelStart += levelWidth;
          levelWidth *= 2;
          levelNumber++;
     }
}
