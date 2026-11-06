// ============================================================
// KCH-PMTS  |  Phase 1 automated unit tests (Modules 1 & 2)
// Each check prints [PASS] or [FAIL]. A summary is printed at the end.
// Exit code is 0 when everything passes, 1 otherwise (used by `make test`).
// ============================================================
#include <iostream>
#include <string>

#include "module1_staff.h"
#include "module2_triage.h"

static int passedChecks = 0;
static int failedChecks = 0;

// Records one test result and prints it
static void verify(bool outcome, const std::string& whatWeCheck) {
     if (outcome) {
          passedChecks++;
          std::cout << "  [PASS] " << whatWeCheck << "\n";
     } else {
          failedChecks++;
          std::cout << "  [FAIL] " << whatWeCheck << "\n";
     }
}

static void testGroup(const std::string& groupName) {
     std::cout << "\n>> " << groupName << "\n";
}

// Some tests trigger expected [ERROR] messages. These two helpers hide
// them so the test report stays clean.
static void muteOutput() { std::cout.setstate(std::ios::failbit); }
static void unmuteOutput() { std::cout.clear(); }

// Returns true if every parent is >= its children (max-heap rule)
static bool heapRuleHolds(const PriorityQueue& pq) {
     for (int pos = 1; pos < pq.patientCount; pos++) {
          if (pq.patientHeap[pos].acuityScore > pq.patientHeap[parent(pos)].acuityScore) {
               return false;
          }
     }
     return true;
}

// Builds the spec heap from [14, 22, 45, 10, 8, 30, 95]
static void buildSpecHeap(PriorityQueue& pq) {
     int specScores[] = {14, 22, 45, 10, 8, 30, 95};
     for (int step = 0; step < 7; step++) {
          Patient arriving;
          arriving.patientID = step + 1;
          arriving.acuityScore = specScores[step];
          pq_insert(&pq, arriving);
     }
}

// ============================================================
// MODULE 1 TESTS
// ============================================================
static void testDoctorArray() {
     testGroup("Module 1 | Doctor array insertion");
     Doctor roster[MAX_DOCTORS];
     int howMany = 0;

     verify(addDoctorToRoster(roster, howMany, 101, "Alex", "GP"), "insert Dr. Alex returns true");
     addDoctorToRoster(roster, howMany, 102, "Beatrice", "Peds");
     addDoctorToRoster(roster, howMany, 103, "Charles", "Surgery");
     verify(howMany == 3, "doctorCount is 3 after three inserts");
     verify(roster[0].doctorID == 101 && roster[1].doctorID == 102 && roster[2].doctorID == 103,
            "doctors stored at indexes 0,1,2 in insertion order");
     verify(roster[2].nurseHead == nullptr && roster[2].nurseCount == 0,
            "new doctor starts with an empty nurse list");

     muteOutput();
     bool duplicateAccepted = addDoctorToRoster(roster, howMany, 101, "Copy", "GP");
     unmuteOutput();
     verify(!duplicateAccepted && howMany == 3, "duplicate doctor ID is rejected");

     testGroup("Module 1 | Doctor array full condition");
     Doctor fullRoster[MAX_DOCTORS];
     int fullCount = 0;
     for (int idSeed = 1; idSeed <= MAX_DOCTORS; idSeed++) {
          addDoctorToRoster(fullRoster, fullCount, idSeed, "Doc", "GP");
     }
     verify(fullCount == MAX_DOCTORS, "roster accepts exactly MAX_DOCTORS (50) doctors");
     muteOutput();
     bool overflowAccepted = addDoctorToRoster(fullRoster, fullCount, 777, "Extra", "GP");
     unmuteOutput();
     verify(!overflowAccepted && fullCount == MAX_DOCTORS, "insert into full array returns false");

     testGroup("Module 1 | Doctor deletion and left shift");
     appendNurseToTeam(roster[1], 503, "N3", "Day"); // Beatrice gets a nurse
     verify(removeDoctorFromRoster(roster, howMany, 102), "delete middle doctor (102) returns true");
     verify(howMany == 2, "doctorCount decremented to 2");
     verify(roster[0].doctorID == 101 && roster[1].doctorID == 103,
            "Charles (103) shifted left from index 2 to index 1");
     verify(roster[2].nurseHead == nullptr && roster[2].nurseCount == 0,
            "old last slot is cleared (no dangling nurse pointer)");
     verify(findDoctorSlot(roster, howMany, 102) == -1, "deleted doctor can no longer be found");

     verify(removeDoctorFromRoster(roster, howMany, 101) && roster[0].doctorID == 103,
            "delete first doctor shifts the rest left");
     verify(removeDoctorFromRoster(roster, howMany, 103) && howMany == 0,
            "delete last remaining doctor leaves an empty roster");

     muteOutput();
     bool ghostRemoved = removeDoctorFromRoster(roster, howMany, 404);
     unmuteOutput();
     verify(!ghostRemoved, "delete a non-existent doctor returns false");

     clearWholeRoster(fullRoster, fullCount);
}

static void testNurseList() {
     testGroup("Module 1 | Nurse tail insertion");
     Doctor drTest;
     drTest.doctorID = 101;
     drTest.name = "Alex";

     appendNurseToTeam(drTest, 501, "N1", "Day");
     verify(drTest.nurseHead != nullptr && drTest.nurseHead->nurseID == 501,
            "first insert into empty list becomes the head");
     verify(drTest.nurseHead->next == nullptr, "single node's next is nullptr");

     appendNurseToTeam(drTest, 502, "N2", "Night");
     appendNurseToTeam(drTest, 503, "N3", "Day");
     verify(drTest.nurseCount == 3, "nurseCount is 3 after three inserts");
     verify(drTest.nurseHead->nurseID == 501 &&
            drTest.nurseHead->next->nurseID == 502 &&
            drTest.nurseHead->next->next->nurseID == 503,
            "list order is 501 -> 502 -> 503 (added at tail)");
     verify(drTest.nurseHead->next->next->next == nullptr, "tail node points to nullptr");

     testGroup("Module 1 | Nurse deletion edge cases");
     appendNurseToTeam(drTest, 504, "N4", "Night"); // list: 501 502 503 504

     verify(removeNurseFromTeam(drTest, 502) && drTest.nurseHead->next->nurseID == 503,
            "delete middle nurse (502): 501 now links to 503");
     verify(removeNurseFromTeam(drTest, 504) && drTest.nurseHead->next->next == nullptr,
            "delete tail nurse (504): 503 becomes tail");
     verify(removeNurseFromTeam(drTest, 501) && drTest.nurseHead->nurseID == 503,
            "delete head nurse (501): head moves to 503");
     verify(drTest.nurseCount == 1, "nurseCount decremented to 1");

     muteOutput();
     bool missingRemoved = removeNurseFromTeam(drTest, 999);
     unmuteOutput();
     verify(!missingRemoved && drTest.nurseCount == 1, "delete non-existent nurse returns false");

     verify(removeNurseFromTeam(drTest, 503) && drTest.nurseHead == nullptr,
            "delete only remaining nurse leaves head = nullptr");

     muteOutput();
     bool emptyRemoved = removeNurseFromTeam(drTest, 503);
     unmuteOutput();
     verify(!emptyRemoved && drTest.nurseCount == 0, "delete from empty list returns false");

     testGroup("Module 1 | Releasing a whole team");
     appendNurseToTeam(drTest, 601, "A", "Day");
     appendNurseToTeam(drTest, 602, "B", "Day");
     releaseNurseTeam(drTest);
     verify(drTest.nurseHead == nullptr && drTest.nurseCount == 0,
            "releaseNurseTeam frees all nodes and resets the list");
}

// ============================================================
// MODULE 2 TESTS
// ============================================================
static void testHeapIndexes() {
     testGroup("Module 2 | Index mapping formulas");
     verify(parent(1) == 0 && parent(2) == 0 && parent(5) == 2 && parent(6) == 2,
            "parent(i) = (i - 1) / 2");
     verify(left_child(0) == 1 && left_child(2) == 5, "left_child(i) = 2i + 1");
     verify(right_child(0) == 2 && right_child(2) == 6, "right_child(i) = 2i + 2");
}

static void testHeapInsertExtract() {
     testGroup("Module 2 | pq_insert with bubble_up");
     PriorityQueue triageQ;
     buildSpecHeap(triageQ);

     int expectedLayout[] = {95, 14, 45, 10, 8, 22, 30};
     bool layoutMatches = (triageQ.patientCount == 7);
     for (int pos = 0; pos < 7; pos++) {
          if (triageQ.patientHeap[pos].acuityScore != expectedLayout[pos]) {
               layoutMatches = false;
          }
     }
     verify(layoutMatches, "final array is [95, 14, 45, 10, 8, 22, 30]");
     verify(triageQ.patientHeap[0].patientID == 7, "root is patient 7 (acuity 95)");
     verify(heapRuleHolds(triageQ), "max-heap dominance holds after all inserts");

     testGroup("Module 2 | extract_max with bubble_down");
     Patient topCase = extract_max(&triageQ);
     verify(topCase.acuityScore == 95 && topCase.patientID == 7, "extract_max returns acuity 95");
     verify(triageQ.patientCount == 6, "patientCount decremented to 6");

     int afterLayout[] = {45, 14, 30, 10, 8, 22};
     bool afterMatches = true;
     for (int pos = 0; pos < 6; pos++) {
          if (triageQ.patientHeap[pos].acuityScore != afterLayout[pos]) {
               afterMatches = false;
          }
     }
     verify(afterMatches, "array after extraction is [45, 14, 30, 10, 8, 22]");
     verify(heapRuleHolds(triageQ), "max-heap dominance holds after extraction");

     bool alwaysDescending = true;
     int lastScore = 1000;
     while (triageQ.patientCount > 0) {
          Patient nextOut = extract_max(&triageQ);
          if (nextOut.acuityScore > lastScore) {
               alwaysDescending = false;
          }
          lastScore = nextOut.acuityScore;
     }
     verify(alwaysDescending, "repeated extract_max gives non-increasing acuity");

     muteOutput();
     Patient nobody = extract_max(&triageQ);
     unmuteOutput();
     verify(nobody.patientID == -1 && triageQ.patientCount == 0,
            "extract_max on empty queue returns patient ID -1");
}

static void testHeapEdgeCases() {
     testGroup("Module 2 | Edge cases");
     PriorityQueue soloQ;
     Patient lonePatient;
     lonePatient.patientID = 42;
     lonePatient.acuityScore = 60;
     pq_insert(&soloQ, lonePatient);
     Patient outAgain = extract_max(&soloQ);
     verify(outAgain.patientID == 42 && soloQ.patientCount == 0,
            "insert then extract a single patient");

     PriorityQueue packedQ;
     for (int seed = 0; seed < MAX_PATIENTS; seed++) {
          Patient filler;
          filler.patientID = seed;
          filler.acuityScore = (seed * 37) % 100 + 1; // mixed scores 1..100
          pq_insert(&packedQ, filler);
     }
     verify(packedQ.patientCount == MAX_PATIENTS && heapRuleHolds(packedQ),
            "100 mixed inserts keep max-heap dominance");
     muteOutput();
     Patient overflowCase;
     overflowCase.patientID = 999;
     overflowCase.acuityScore = 100;
     pq_insert(&packedQ, overflowCase);
     unmuteOutput();
     verify(packedQ.patientCount == MAX_PATIENTS, "insert into full queue is rejected");

     testGroup("Module 2 | Heapsort (in place)");
     PriorityQueue sortQ;
     buildSpecHeap(sortQ);
     int sortedSize = heap_sort_in_place(&sortQ);
     int ascendingExpected[] = {8, 10, 14, 22, 30, 45, 95};
     bool sortedRight = (sortedSize == 7);
     for (int pos = 0; pos < 7; pos++) {
          if (sortQ.patientHeap[pos].acuityScore != ascendingExpected[pos]) {
               sortedRight = false;
          }
     }
     verify(sortedRight, "heapsort gives [8, 10, 14, 22, 30, 45, 95]");
}

int main() {
     std::cout << "############################################################\n";
     std::cout << "#   KCH-PMTS  |  Phase 1 Unit Test Suite (Modules 1 & 2)   #\n";
     std::cout << "############################################################\n";

     testDoctorArray();
     testNurseList();
     testHeapIndexes();
     testHeapInsertExtract();
     testHeapEdgeCases();

     int totalChecks = passedChecks + failedChecks;
     std::cout << "\n============================================================\n";
     std::cout << "  RESULT: " << passedChecks << " / " << totalChecks << " checks passed";
     if (failedChecks == 0) {
          std::cout << "  -> ALL TESTS PASSED\n";
     } else {
          std::cout << "  -> " << failedChecks << " FAILED\n";
     }
     std::cout << "============================================================\n";

     return failedChecks == 0 ? 0 : 1;
}
