#include<iostream>
#include<string>
#include<vector>
#include<iomanip>

using namespace std;

// Converts a letter grade to grade points (on a 4.0 scale)
double gradeToPoint(const string& grade) {
    if (grade == "A+" || grade == "A")  return 4.0;
    if (grade == "A-")                  return 3.7;
    if (grade == "B+")                  return 3.3;
    if (grade == "B")                   return 3.0;
    if (grade == "B-")                  return 2.7;
    if (grade == "C+")                  return 2.3;
    if (grade == "C")                   return 2.0;
    if (grade == "C-")                  return 1.7;
    if (grade == "D+")                  return 1.3;
    if (grade == "D")                   return 1.0;
    if (grade == "F")                   return 0.0;
    return -1.0; // invalid grade marker
}

int main() {
    int numCourses;

    cout << "===== CGPA Calculator =====\n\n";
    cout << "Enter number of courses: ";
    cin >> numCourses;

    while (numCourses <= 0) {
        cout << "Please enter a positive number of courses: ";
        cin >> numCourses;
    }

    vector<string> courseNames(numCourses);
    vector<double> credits(numCourses);
    vector<double> gradePoints(numCourses);

    double totalCredits = 0.0;
    double totalQualityPoints = 0.0;

    for (int i = 0; i < numCourses; i++) {
        cout << "\n--- Course " << (i + 1) << " ---\n";

        cout << "Course name/code: ";
        cin >> courseNames[i];

        cout << "Credit hours: ";
        while (!(cin >> credits[i]) || credits[i] <= 0) {
            cout << "Enter a valid positive number for credit hours: ";
            cin.clear();
            cin.ignore(10000, '\n');
        }

        string grade;
        double points;
        while (true) {
            cout << "Grade (A+, A, A-, B+, B, B-, C+, C, C-, D+, D, F): ";
            cin >> grade;
            points = gradeToPoint(grade);
            if (points >= 0.0) break;
            cout << "Invalid grade. Please try again.\n";
        }

        gradePoints[i] = points;
        totalCredits += credits[i];
        totalQualityPoints += credits[i] * points;
    }

    double cgpa = totalQualityPoints / totalCredits;

    cout << "\n===== Summary =====\n";
    cout << left << setw(15) << "Course"
        << setw(10) << "Credits"
        << setw(10) << "Grade"
        << "Points\n";
    cout << string(45, '-') << "\n";

    for (int i = 0; i < numCourses; i++) {
        cout << left << setw(15) << courseNames[i]
            << setw(10) << credits[i]
            << setw(10) << fixed << setprecision(2) << gradePoints[i]
            << (credits[i] * gradePoints[i]) << "\n";
    }

    cout << string(45, '-') << "\n";
    cout << "Total Credit Hours: " << totalCredits << "\n";
    cout << "Total Quality Points: " << fixed << setprecision(2) << totalQualityPoints << "\n";
    cout << fixed << setprecision(2);
    cout << "\nYour CGPA is: " << cgpa << " / 4.0\n";

    return 0;
   

}



















