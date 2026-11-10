// KCH-PMTS  |  Module 1 : Staff Management & Dynamic Rostering
// Doctors live in a fixed array; each doctor has a linked list of nurses.
#include <iostream>
#include <iomanip>
#include <sstream>

#include "module1_staff.h"

// Q1.1 (1) - Doctor array insertion
// The new doctor goes to index doctorCount (the first empty slot).
// Time: O(n) because of the duplicate-ID check, O(1) for the insert itself.
bool addDoctorToRoster(Doctor doctorRoster[], int& doctorCount,
                       int newID, const std::string& newName,
                       const std::string& newSpecialty) {
     // Full array check: no free slot left
     if (doctorCount >= MAX_DOCTORS) {
          std::cout << "  [ERROR] Roster is full (" << MAX_DOCTORS
                    << " doctors). Cannot add Dr. " << newName << ".\n";
          return false;
     }

     // Two doctors must not share the same ID
     if (findDoctorSlot(doctorRoster, doctorCount, newID) != -1) {
          std::cout << "  [ERROR] Doctor ID " << newID << " already exists.\n";
          return false;
     }

     // Fill the free slot
     Doctor& freeSlot = doctorRoster[doctorCount];
     freeSlot.doctorID = newID;
     freeSlot.name = newName;
     freeSlot.specialization = newSpecialty;
     freeSlot.nurseHead = nullptr; // a new doctor starts with no nurses
     freeSlot.nurseCount = 0;

     doctorCount++;
     return true;
}

// Linear search through the used part of the array.
// Returns the index, or -1 when the ID is not there.  Time: O(n)
int findDoctorSlot(const Doctor doctorRoster[], int doctorCount, int wantedID) {
     for (int pos = 0; pos < doctorCount; pos++) {
          if (doctorRoster[pos].doctorID == wantedID) {
               return pos;
          }
     }
     return -1;
}

// Q1.1 (3) - Doctor array deletion
// Steps: find -> free nurses -> shift left -> clear last slot -> count--
// Time: O(n + k)  (n = doctors to shift, k = nurses to free)
bool removeDoctorFromRoster(Doctor doctorRoster[], int& doctorCount, int targetID) {
     int foundAt = findDoctorSlot(doctorRoster, doctorCount, targetID);
     if (foundAt == -1) {
          std::cout << "  [ERROR] Doctor ID " << targetID << " not found.\n";
          return false;
     }

     // 1. Free the nurse nodes first, otherwise their memory is lost (leak)
     releaseNurseTeam(doctorRoster[foundAt]);

     // 2. Shift every doctor after the gap one position to the left
     for (int pos = foundAt; pos < doctorCount - 1; pos++) {
          doctorRoster[pos] = doctorRoster[pos + 1];
     }

     // 3. The old last slot is now a duplicate: reset it so it holds
     //    no pointer to a nurse list that another slot now owns
     doctorRoster[doctorCount - 1] = Doctor{};

     // 4. One doctor less
     doctorCount--;
     return true;
}

// Q1.2 (1) - Tail insertion
// Walk to the last node, then link the new node after it.  Time: O(k)
void appendNurseToTeam(Doctor& doc, int newNurseID,
                       const std::string& nurseName, const std::string& nurseShift) {
     // Create the node on the heap (it must live after this function ends)
     Nurse* freshNurse = new Nurse;
     freshNurse->nurseID = newNurseID;
     freshNurse->name = nurseName;
     freshNurse->shift = nurseShift;
     freshNurse->next = nullptr; // it will be the new tail

     if (doc.nurseHead == nullptr) {
          // Case A: empty list -> the new nurse becomes the head
          doc.nurseHead = freshNurse;
     } else {
          // Case B: walk until we reach the node whose next is nullptr
          Nurse* tailFinder = doc.nurseHead;
          while (tailFinder->next != nullptr) {
               tailFinder = tailFinder->next;
          }
          tailFinder->next = freshNurse; // old tail now points to new node
     }

     doc.nurseCount++;
}

// Q1.2 (2) - Delete one nurse by ID
// Edge cases: empty list, nurse at head, nurse in middle/tail, not found.
// Time: O(k)
bool removeNurseFromTeam(Doctor& doc, int targetNurseID) {
     // Edge case 1: empty list
     if (doc.nurseHead == nullptr) {
          std::cout << "  [ERROR] Dr. " << doc.name << " has no nurses to remove.\n";
          return false;
     }

     // Edge case 2: the nurse is the head -> head moves to the second node
     if (doc.nurseHead->nurseID == targetNurseID) {
          Nurse* leavingNurse = doc.nurseHead;
          doc.nurseHead = leavingNurse->next;
          delete leavingNurse;
          doc.nurseCount--;
          return true;
     }

     // Edge case 3: middle or tail -> keep a pointer to the node BEFORE the target
     Nurse* behindNode = doc.nurseHead;
     Nurse* scanNode = doc.nurseHead->next;
     while (scanNode != nullptr) {
          if (scanNode->nurseID == targetNurseID) {
               behindNode->next = scanNode->next; // skip over the target
               delete scanNode;
               doc.nurseCount--;
               return true;
          }
          behindNode = scanNode;
          scanNode = scanNode->next;
     }

     // Edge case 4: reached the end without finding it
     std::cout << "  [ERROR] Nurse ID " << targetNurseID
               << " is not in Dr. " << doc.name << "'s team.\n";
     return false;
}

// Frees all nurse nodes of one doctor, one by one.  Time: O(k)
void releaseNurseTeam(Doctor& doc) {
     Nurse* scanNode = doc.nurseHead;
     while (scanNode != nullptr) {
          Nurse* nextInLine = scanNode->next; // save the link BEFORE deleting
          delete scanNode;
          scanNode = nextInLine;
     }
     doc.nurseHead = nullptr;
     doc.nurseCount = 0;
}

// Frees all nurses of all doctors (call before the program exits)
void clearWholeRoster(Doctor doctorRoster[], int& doctorCount) {
     for (int pos = 0; pos < doctorCount; pos++) {
          releaseNurseTeam(doctorRoster[pos]);
     }
     doctorCount = 0;
}

// Display helpers
void printNurseChain(const Doctor& doc) {
     std::cout << "  Dr. " << doc.name << " (" << doc.nurseCount << " nurses): HEAD";
     for (Nurse* scanNode = doc.nurseHead; scanNode != nullptr; scanNode = scanNode->next) {
          std::cout << " -> [" << scanNode->nurseID << " " << scanNode->name << "]";
     }
     std::cout << " -> NULL\n";
}

// Turns a pointer into readable text: its address, or the word "nullptr"
static std::string pointerText(const Nurse* where) {
     if (where == nullptr) {
          return "nullptr";
     }
     std::ostringstream addressOut;
     addressOut << static_cast<const void*>(where);
     return addressOut.str();
}

void printNurseChainWithAddresses(const Doctor& doc) {
     std::cout << "  Dr. " << doc.name << " | nurseHead = "
               << pointerText(doc.nurseHead) << " | nurseCount = " << doc.nurseCount << "\n";
     if (doc.nurseHead == nullptr) {
          std::cout << "      (empty list)\n";
          return;
     }
     for (Nurse* scanNode = doc.nurseHead; scanNode != nullptr; scanNode = scanNode->next) {
          std::cout << "      node @ " << pointerText(scanNode)
                    << "  { ID: " << scanNode->nurseID
                    << ", next: " << pointerText(scanNode->next) << " }\n";
     }
}

void printDoctorTable(const Doctor doctorRoster[], int doctorCount) {
     std::cout << "  +-------+------+----------------+------------------+--------+\n";
     std::cout << "  | Index | ID   | Name           | Specialization   | Nurses |\n";
     std::cout << "  +-------+------+----------------+------------------+--------+\n";
     for (int pos = 0; pos < doctorCount; pos++) {
          const Doctor& row = doctorRoster[pos];
          std::cout << "  | " << std::setw(5) << pos
                    << " | " << std::setw(4) << row.doctorID
                    << " | " << std::left << std::setw(14) << row.name
                    << " | " << std::setw(16) << row.specialization << std::right
                    << " | " << std::setw(6) << row.nurseCount << " |\n";
     }
     if (doctorCount == 0) {
          std::cout << "  |                    (roster is empty)                      |\n";
     }
     std::cout << "  +-------+------+----------------+------------------+--------+\n";
     std::cout << "  doctorCount = " << doctorCount << " / " << MAX_DOCTORS << "\n";
}
