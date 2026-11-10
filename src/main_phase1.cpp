// KCH-PMTS  |  Phase 1 driver (Modules 1 & 2)
// Runs the exact scenarios from the project specification and prints
// every step so the output can be used directly as report traces.
// Group: Dushime Brother (dushimeb), Mukantwari Francoise (mfrancoi),
//        Uwimbabazi Keza (ukeza)
#include <iostream>
#include <string>

#include "module1_staff.h"
#include "module2_triage.h"

// Prints a big section title
static void showSection(const std::string& heading) {
     std::cout << "\n============================================================\n";
     std::cout << "  " << heading << "\n";
     std::cout << "============================================================\n";
}

// Prints a smaller step title
static void showStep(const std::string& label) {
     std::cout << "\n--- " << label << " ---\n";
}

// MODULE 1 demo
static void runStaffDemo() {
     Doctor hospitalRoster[MAX_DOCTORS];
     int rosterSize = 0;

     showSection("MODULE 1 - Q1.1 : Doctor Roster Array");

     showStep("Insert Dr. Alex (101), Dr. Beatrice (102), Dr. Charles (103)");
     addDoctorToRoster(hospitalRoster, rosterSize, 101, "Alex", "General Medicine");
     addDoctorToRoster(hospitalRoster, rosterSize, 102, "Beatrice", "Pediatrics");
     addDoctorToRoster(hospitalRoster, rosterSize, 103, "Charles", "Surgery");
     printDoctorTable(hospitalRoster, rosterSize);

     showStep("Edge case: insert a duplicate ID (101)");
     addDoctorToRoster(hospitalRoster, rosterSize, 101, "Duplicate", "None");

     showSection("MODULE 1 - Q1.2 : Nurse Linked Lists (pointer trace)");
     Doctor& drAlex = hospitalRoster[0];
     Doctor& drBeatrice = hospitalRoster[1];

     showStep("Start: both lists empty");
     printNurseChainWithAddresses(drAlex);
     printNurseChainWithAddresses(drBeatrice);

     showStep("Step 1: tail-insert N1 (501) for Dr. Alex");
     appendNurseToTeam(drAlex, 501, "N1", "Day");
     printNurseChainWithAddresses(drAlex);

     showStep("Step 2: tail-insert N2 (502) for Dr. Alex");
     appendNurseToTeam(drAlex, 502, "N2", "Night");
     printNurseChainWithAddresses(drAlex);

     showStep("Step 3: tail-insert N3 (503) for Dr. Beatrice");
     appendNurseToTeam(drBeatrice, 503, "N3", "Day");
     printNurseChainWithAddresses(drBeatrice);

     showStep("Step 4: delete N1 (501) from Dr. Alex (head deletion)");
     removeNurseFromTeam(drAlex, 501);
     printNurseChainWithAddresses(drAlex);

     showStep("Summary of all teams");
     for (int pos = 0; pos < rosterSize; pos++) {
          printNurseChain(hospitalRoster[pos]);
     }

     showStep("Edge case: delete a nurse that does not exist (999)");
     removeNurseFromTeam(drAlex, 999);

     showStep("Edge case: delete from an empty list (Dr. Charles)");
     removeNurseFromTeam(hospitalRoster[2], 503);

     showSection("MODULE 1 - Q1.1 : Doctor Deletion with left shift");
     showStep("Before: delete Dr. Beatrice (102), who still has nurse N3");
     printDoctorTable(hospitalRoster, rosterSize);
     removeDoctorFromRoster(hospitalRoster, rosterSize, 102);
     showStep("After: N3 freed, Dr. Charles shifted from index 2 to index 1");
     printDoctorTable(hospitalRoster, rosterSize);

     showStep("Edge case: delete a doctor that does not exist (555)");
     removeDoctorFromRoster(hospitalRoster, rosterSize, 555);

     showStep("Edge case: fill the roster to " + std::to_string(MAX_DOCTORS) + " and add one more");
     int nextFreeID = 200;
     while (rosterSize < MAX_DOCTORS) {
          addDoctorToRoster(hospitalRoster, rosterSize, nextFreeID, "Filler", "General");
          nextFreeID++;
     }
     std::cout << "  doctorCount = " << rosterSize << "\n";
     addDoctorToRoster(hospitalRoster, rosterSize, 999, "Extra", "General");

     // Free every remaining nurse node before leaving (no memory leaks)
     clearWholeRoster(hospitalRoster, rosterSize);
     std::cout << "\n  All nurse memory released. doctorCount = " << rosterSize << "\n";
}

// MODULE 2 demo
static void runTriageDemo() {
     PriorityQueue erQueue;
     triageTraceOn = true; // show each swap

     showSection("MODULE 2 - Q2.2 : Sequential Max-Heap Insertion Trace");
     std::cout << "  Acuity scores in arrival order: [14, 22, 45, 10, 8, 30, 95]\n";

     int arrivalScores[] = {14, 22, 45, 10, 8, 30, 95};
     int arrivalTotal = 7;
     for (int step = 0; step < arrivalTotal; step++) {
          Patient arriving;
          arriving.patientID = step + 1; // patientID = arrival order
          arriving.acuityScore = arrivalScores[step];

          showStep("Insert #" + std::to_string(step + 1) + ": patient " +
                   std::to_string(arriving.patientID) + " (acuity " +
                   std::to_string(arriving.acuityScore) + ")");
          pq_insert(&erQueue, arriving);
          printHeapArray(&erQueue);
          printHeapTree(&erQueue);
     }

     showSection("MODULE 2 - Q2.2 : One extract_max (sift-down trace)");
     std::cout << "  Root " << erQueue.patientHeap[0].acuityScore
               << " is removed; last leaf "
               << erQueue.patientHeap[erQueue.patientCount - 1].acuityScore
               << " moves to index 0, then sifts down:\n";
     Patient served = extract_max(&erQueue);
     std::cout << "  Extracted: patient " << served.patientID
               << " (acuity " << served.acuityScore << ")\n";
     showStep("Heap after extraction");
     printHeapArray(&erQueue);
     printHeapTree(&erQueue);

     showSection("MODULE 2 - Heapsort (in place, on a copy of the heap)");
     triageTraceOn = false;
     PriorityQueue sortCopy = erQueue; // keep the real queue untouched
     int sortedTotal = heap_sort_in_place(&sortCopy);
     std::cout << "  Sorted array (ascending acuity): ";
     for (int pos = 0; pos < sortedTotal; pos++) {
          std::cout << sortCopy.patientHeap[pos].acuityScore << " ";
     }
     std::cout << "\n";

     showSection("MODULE 2 - Serving the remaining patients by urgency");
     int servedOrder = 1;
     while (erQueue.patientCount > 0) {
          Patient nextUp = extract_max(&erQueue);
          std::cout << "  " << servedOrder << ". patient " << nextUp.patientID
                    << "  acuity " << nextUp.acuityScore << "\n";
          servedOrder++;
     }

     showStep("Edge case: extract from an empty queue");
     extract_max(&erQueue);
}

int main() {
     std::cout << "############################################################\n";
     std::cout << "#  Kigali Central Hospital - PMTS  |  Phase 1 Demo         #\n";
     std::cout << "#  Modules: 1 (Staff Rostering)  &  2 (ER Triage Heap)     #\n";
     std::cout << "############################################################\n";

     runStaffDemo();
     runTriageDemo();

     std::cout << "\n=================== END OF PHASE 1 DEMO ====================\n";
     return 0;
}
