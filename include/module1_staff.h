//   KCH-PMTS  |  Module 1 : Staff Management & Dynamic Rostering
//   Data structures used:
//    - a fixed ARRAY of doctors (max 50)
//    - each doctor owns a SINGLY LINKED LIST of nurses
//   Group7: 
//      Dushime Brother (dushimeb), 
//      Mukantwari Francoise (mfrancoi),
//      Uwimbabazi Keza (ukeza)

#pragma once
#include <string>

// Maximum number of doctors the hospital roster can hold
const int MAX_DOCTORS = 50;

// One nurse = one node in a doctor's linked list
struct Nurse {
     int nurseID;
     std::string name;
     std::string shift;      // e.g. "Day", "Night"
     Nurse* next = nullptr;  // link to the next nurse (nullptr = end of list)
};

// One doctor = one slot in the doctor array
struct Doctor {
     int doctorID;
     std::string name;
     std::string specialization;
     Nurse* nurseHead = nullptr; // first nurse of this doctor's team
     int nurseCount = 0;         // how many nurses are in the team
};

// Doctor array operations (Question 1.1)

// Adds a doctor at the next free index. Returns false if the array is full
// (or if the ID is already used).
bool addDoctorToRoster(
     Doctor doctorRoster[], 
     int& doctorCount, 
     int newID, 
     const std::string& newName, 
     const std::string& newSpecialty
);

// Returns the array index of the doctor with this ID, or -1 if not found.
int findDoctorSlot(const Doctor doctorRoster[], int doctorCount, int wantedID);

// Removes a doctor: frees all their nurses, shifts the rest of the array
// one step left, and decreases doctorCount. Returns false if not found.
bool removeDoctorFromRoster(Doctor doctorRoster[], int& doctorCount, int targetID);

// Nurse linked list operations (Question 1.2)

// Creates a new nurse node and attaches it at the END (tail) of the list.
void appendNurseToTeam(Doctor& doc, int newNurseID,
                       const std::string& nurseName, const std::string& nurseShift);

// Finds a nurse by ID, unlinks the node, frees it. Returns false if not found.
bool removeNurseFromTeam(Doctor& doc, int targetNurseID);

// Frees every nurse node of one doctor (used by doctor deletion).
void releaseNurseTeam(Doctor& doc);

// Frees every nurse of every doctor and empties the roster (end of program).
void clearWholeRoster(Doctor doctorRoster[], int& doctorCount);

// Display helpers

// Prints: HEAD -> [501 Alice] -> [502 Bella] -> NULL
void printNurseChain(const Doctor& doc);

// Same as above but also shows each node's memory address and its next pointer
void printNurseChainWithAddresses(const Doctor& doc);

// Prints the whole doctor array as a table (index, ID, name, nurses)
void printDoctorTable(const Doctor doctorRoster[], int doctorCount);
