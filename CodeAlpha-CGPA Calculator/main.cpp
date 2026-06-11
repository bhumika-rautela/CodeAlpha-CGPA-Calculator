#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <limits>

using namespace std;

struct Course {
  string name;
  string grade;
  double credits;
};

struct Semester {
  string name;
  vector<Course> courses;
};

const map<string, vector<pair<string, double>>> DEFAULT_SCALES = {
  { "4.0", {
      {"A+", 4.0}, {"A", 4.0}, {"A-", 3.7},
      {"B+", 3.3}, {"B", 3.0}, {"B-", 2.7},
      {"C+", 2.3}, {"C", 2.0}, {"C-", 1.7},
      {"D+", 1.3}, {"D", 1.0}, {"F", 0.0}
    }
  },
  { "10.0", {
      {"O", 10.0}, {"A+", 9.0}, {"A", 8.0},
      {"B+", 7.0}, {"B", 6.0}, {"C", 5.0},
      {"P", 4.0}, {"F", 0.0}
    }
  }
};

void printColor(const string& text, const string& colorCode) {
  cout << "\033[" << colorCode << "m" << text << "\033[0m";
}

void printColorLn(const string& text, const string& colorCode) {
  printColor(text, colorCode);
  cout << "\n";
}

void clearScreen() {
  cout << "\033[2J\033[1;1H";
}

void pressEnterToContinue() {
  cout << "\nPress Enter to return to menu...";
  if (cin.eof()) exit(0);
  cin.get();
}

string getValidatedString(const string& prompt) {
  string value;
  cout << prompt;
  while (true) {
    if (getline(cin >> ws, value)) {
      if (!value.empty()) {
        return value;
      }
    } else {
      if (cin.eof()) exit(0);
    }
    cout << "Input cannot be empty. Re-enter: ";
  }
}

int getValidatedInt(const string& prompt, int minVal, int maxVal) {
  int value;
  cout << prompt;
  while (true) {
    if (cin >> value) {
      if (value >= minVal && value <= maxVal) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
      }
    } else {
      if (cin.eof()) exit(0);
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << "Invalid input. Please enter a number between " << minVal << " and " << maxVal << ": ";
  }
}

double getValidatedDouble(const string& prompt, double minVal, double maxVal) {
  double value;
  cout << prompt;
  while (true) {
    if (cin >> value) {
      if (value >= minVal && value <= maxVal) {
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
      }
    } else {
      if (cin.eof()) exit(0);
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cout << "Invalid input. Please enter a value between " << minVal << " and " << maxVal << ": ";
  }
}

double getGradePoints(const string& scale, const string& grade, const map<string, vector<pair<string, double>>>& customScales) {
  if (scale == "percent") {
    try {
      double pct = stod(grade);
      return max(0.0, min(100.0, pct));
    } catch (...) {
      return 0.0;
    }
  }

  auto defaultIt = DEFAULT_SCALES.find(scale);
  if (defaultIt != DEFAULT_SCALES.end()) {
    for (const auto& mapping : defaultIt->second) {
      if (mapping.first == grade) return mapping.second;
    }
  }

  auto customIt = customScales.find(scale);
  if (customIt != customScales.end()) {
    for (const auto& mapping : customIt->second) {
      if (mapping.first == grade) return mapping.second;
    }
  }

  return 0.0;
}

double getMaxPoints(const string& scale, const map<string, vector<pair<string, double>>>& customScales) {
  if (scale == "percent") return 100.0;
  
  double maxVal = 0.0;
  auto defaultIt = DEFAULT_SCALES.find(scale);
  if (defaultIt != DEFAULT_SCALES.end()) {
    for (const auto& mapping : defaultIt->second) {
      maxVal = max(maxVal, mapping.second);
    }
    return maxVal;
  }

  auto customIt = customScales.find(scale);
  if (customIt != customScales.end()) {
    for (const auto& mapping : customIt->second) {
      maxVal = max(maxVal, mapping.second);
    }
    return maxVal;
  }

  return 4.0;
}

bool isValidGrade(const string& scale, const string& grade, const map<string, vector<pair<string, double>>>& customScales) {
  if (scale == "percent") {
    try {
      double pct = stod(grade);
      return pct >= 0.0 && pct <= 100.0;
    } catch (...) {
      return false;
    }
  }

  auto defaultIt = DEFAULT_SCALES.find(scale);
  if (defaultIt != DEFAULT_SCALES.end()) {
    for (const auto& mapping : defaultIt->second) {
      if (mapping.first == grade) return true;
    }
    return false;
  }

  auto customIt = customScales.find(scale);
  if (customIt != customScales.end()) {
    for (const auto& mapping : customIt->second) {
      if (mapping.first == grade) return true;
    }
    return false;
  }

  return false;
}

string getValidatedGrade(const string& prompt, const string& scale, const map<string, vector<pair<string, double>>>& customScales) {
  string grade;
  while (true) {
    grade = getValidatedString(prompt);
    transform(grade.begin(), grade.end(), grade.begin(), ::toupper);
    if (isValidGrade(scale, grade, customScales)) {
      return grade;
    }
    cout << "Invalid grade symbol for scale. Re-enter.\n";
  }
}

double calculateSemesterGPA(const Semester& sem, const string& scale, const map<string, vector<pair<string, double>>>& customScales, double& totalCredits) {
  double totalPoints = 0.0;
  totalCredits = 0.0;
  for (const auto& course : sem.courses) {
    totalCredits += course.credits;
    totalPoints += (getGradePoints(scale, course.grade, customScales) * course.credits);
  }
  return totalCredits > 0.0 ? (totalPoints / totalCredits) : 0.0;
}

double calculateCGPA(const vector<Semester>& semesters, const string& scale, const map<string, vector<pair<string, double>>>& customScales, double& totalCredits) {
  double totalPoints = 0.0;
  totalCredits = 0.0;
  for (const auto& sem : semesters) {
    double semCredits = 0.0;
    calculateSemesterGPA(sem, scale, customScales, semCredits);
    totalCredits += semCredits;
    for (const auto& course : sem.courses) {
      totalPoints += (getGradePoints(scale, course.grade, customScales) * course.credits);
    }
  }
  return totalCredits > 0.0 ? (totalPoints / totalCredits) : 0.0;
}

void displayReport(const vector<Semester>& semesters, const string& scale, const map<string, vector<pair<string, double>>>& customScales) {
  clearScreen();
  printColorLn("==========================================================", "1;35");
  printColorLn("                 ACADEMIC PERFORMANCE REPORT              ", "1;36");
  printColorLn("==========================================================", "1;35");
  
  if (semesters.empty()) {
    printColorLn("No semester records found. Please add a semester.", "1;31");
    return;
  }

  double globalCredits = 0.0;
  double cgpa = calculateCGPA(semesters, scale, customScales, globalCredits);

  for (const auto& sem : semesters) {
    cout << "\n";
    printColor(sem.name, "1;33");
    cout << "\n----------------------------------------------------------\n";
    cout << left << setw(25) << "Course Name" << setw(15) << "Grade" << setw(10) << "Credits" << setw(10) << "Points" << "\n";
    cout << "----------------------------------------------------------\n";
    
    for (const auto& course : sem.courses) {
      double points = getGradePoints(scale, course.grade, customScales);
      cout << left << setw(25) << course.name 
           << setw(15) << course.grade 
           << setw(10) << fixed << setprecision(1) << course.credits 
           << setw(10) << fixed << setprecision(2) << points << "\n";
    }
    
    double semCredits = 0.0;
    double gpa = calculateSemesterGPA(sem, scale, customScales, semCredits);
    cout << "----------------------------------------------------------\n";
    cout << right << setw(40) << "Semester Credits: " << fixed << setprecision(1) << semCredits;
    cout << " | GPA: ";
    printColorLn(to_string(gpa).substr(0, 4), "1;32");
  }

  cout << "\n==========================================================\n";
  cout << "Global Academic Status Summary:\n";
  cout << "Total Credits Earned: " << fixed << setprecision(1) << globalCredits << "\n";
  cout << "Cumulative CGPA     : ";
  printColorLn(to_string(cgpa).substr(0, 4) + " / " + to_string(getMaxPoints(scale, customScales)).substr(0, 4), "1;92");
  cout << "==========================================================\n";
}

void saveToFile(const string& filename, const vector<Semester>& semesters, const string& scale, const map<string, vector<pair<string, double>>>& customScales) {
  ofstream outFile(filename);
  if (!outFile) {
    printColorLn("Error opening file for saving.", "1;31");
    return;
  }

  outFile << scale << "\n";
  outFile << customScales.size() << "\n";
  for (const auto& scalePair : customScales) {
    outFile << scalePair.first << "\n";
    outFile << scalePair.second.size() << "\n";
    for (const auto& mapping : scalePair.second) {
      outFile << mapping.first << "," << mapping.second << "\n";
    }
  }

  outFile << semesters.size() << "\n";
  for (const auto& sem : semesters) {
    outFile << sem.name << "\n";
    outFile << sem.courses.size() << "\n";
    for (const auto& course : sem.courses) {
      outFile << course.name << "," << course.grade << "," << course.credits << "\n";
    }
  }
  
  printColorLn("Records successfully saved to " + filename, "1;32");
}

void loadFromFile(const string& filename, vector<Semester>& semesters, string& scale, map<string, vector<pair<string, double>>>& customScales) {
  ifstream inFile(filename);
  if (!inFile) {
    printColorLn("File not found or cannot be opened.", "1;31");
    return;
  }

  semesters.clear();
  customScales.clear();

  string line;
  if (!getline(inFile, scale)) return;

  string numCustomStr;
  if (!getline(inFile, numCustomStr)) return;
  int numCustom = stoi(numCustomStr);
  
  for (int i = 0; i < numCustom; ++i) {
    string scaleName;
    if (!getline(inFile, scaleName)) return;
    
    string numMappingsStr;
    if (!getline(inFile, numMappingsStr)) return;
    int numMappings = stoi(numMappingsStr);
    
    vector<pair<string, double>> mappings;
    for (int j = 0; j < numMappings; ++j) {
      string mappingLine;
      if (!getline(inFile, mappingLine)) return;
      stringstream ss(mappingLine);
      string grade;
      string pointsStr;
      getline(ss, grade, ',');
      getline(ss, pointsStr, ',');
      mappings.push_back({grade, stod(pointsStr)});
    }
    customScales[scaleName] = mappings;
  }

  string numSemStr;
  if (!getline(inFile, numSemStr)) return;
  int numSem = stoi(numSemStr);

  for (int i = 0; i < numSem; ++i) {
    Semester sem;
    if (!getline(inFile, sem.name)) return;
    
    string numCoursesStr;
    if (!getline(inFile, numCoursesStr)) return;
    int numCourses = stoi(numCoursesStr);
    
    for (int j = 0; j < numCourses; ++j) {
      string courseLine;
      if (!getline(inFile, courseLine)) return;
      stringstream ss(courseLine);
      Course course;
      string creditsStr;
      getline(ss, course.name, ',');
      getline(ss, course.grade, ',');
      getline(ss, creditsStr, ',');
      course.credits = stod(creditsStr);
      sem.courses.push_back(course);
    }
    semesters.push_back(sem);
  }

  printColorLn("Records successfully loaded from " + filename, "1;32");
}

void targetPlanner(const vector<Semester>& semesters, const string& scale, const map<string, vector<pair<string, double>>>& customScales) {
  clearScreen();
  printColorLn("==========================================================", "1;35");
  printColorLn("                 TARGET CGPA GOAL PLANNER                 ", "1;36");
  printColorLn("==========================================================", "1;35");

  double currentCredits = 0.0;
  double currentCgpa = calculateCGPA(semesters, scale, customScales, currentCredits);
  double maxPoints = getMaxPoints(scale, customScales);

  cout << "Current Total Credits: " << fixed << setprecision(1) << currentCredits << "\n";
  cout << "Current CGPA: " << fixed << setprecision(2) << currentCgpa << "\n\n";

  double targetCgpa = getValidatedDouble("Enter your target CGPA: ", 0.0, maxPoints);
  double remainingCredits = getValidatedDouble("Enter remaining credits to complete: ", 0.5, 300.0);

  double totalCredits = currentCredits + remainingCredits;
  double requiredTotalPoints = targetCgpa * totalCredits;
  double currentTotalPoints = currentCgpa * currentCredits;
  double requiredGpa = (requiredTotalPoints - currentTotalPoints) / remainingCredits;

  cout << "\n----------------------------------------------------------\n";
  if (requiredGpa > maxPoints) {
    printColorLn("RESULT: IMPOSSIBLE TARGET", "1;31");
    cout << "To reach a CGPA of " << targetCgpa << ", you need a future average GPA of " 
         << fixed << setprecision(2) << requiredGpa << ".\nThis exceeds the maximum scale limit of " << maxPoints << ".\n";
  } else if (requiredGpa <= 0.0) {
    printColorLn("RESULT: TARGET SECURED", "1;32");
    cout << "You have already secured enough points to maintain your target CGPA.\nEven with a 0.0 average on future courses, you will meet your target.\n";
  } else {
    printColorLn("RESULT: TARGET ACHIEVABLE", "1;32");
    cout << "You must maintain a minimum average GPA of ";
    printColor(to_string(requiredGpa).substr(0, 4), "1;33");
    cout << " over your remaining " << remainingCredits << " credits to graduate with a CGPA of " << targetCgpa << ".\n";
  }
  cout << "==========================================================\n";
}

void configureCustomScale(string& activeScale, map<string, vector<pair<string, double>>>& customScales) {
  clearScreen();
  printColorLn("==========================================================", "1;35");
  printColorLn("                 CUSTOM SCALES CONFIGURATOR               ", "1;36");
  printColorLn("==========================================================", "1;35");
  
  string name = getValidatedString("Enter new scale name: ");
  int numGrades = getValidatedInt("Enter number of grade symbols to map: ", 1, 30);
  
  vector<pair<string, double>> mappings;
  for (int i = 0; i < numGrades; ++i) {
    string symbol = getValidatedString("Grade Symbol " + to_string(i + 1) + " (e.g. A+): ");
    transform(symbol.begin(), symbol.end(), symbol.begin(), ::toupper);
    double points = getValidatedDouble("Point value for " + symbol + ": ", 0.0, 100.0);
    mappings.push_back({symbol, points});
  }
  
  customScales[name] = mappings;
  activeScale = name;
  printColorLn("Scale '" + name + "' successfully created and activated.", "1;32");
}

int main() {
  vector<Semester> semesters;
  string activeScale = "4.0";
  map<string, vector<pair<string, double>>> customScales;
  string recordsFile = "academic_records.txt";

  ifstream checkFile(recordsFile);
  if (checkFile) {
    checkFile.close();
    loadFromFile(recordsFile, semesters, activeScale, customScales);
    cout << "Press Enter to start AeroGrade CLI...";
    if (cin.eof()) exit(0);
    cin.get();
  } else {
    semesters.push_back({
      "Semester 1", {
        {"Mathematics I", "A", 4.0},
        {"Physics Lab", "B+", 3.0},
        {"Computer Programming", "A-", 4.0}
      }
    });
  }

  while (true) {
    clearScreen();
    printColorLn("==========================================================", "1;35");
    printColorLn("               AEROGRADE CGPA CALCULATOR CLI              ", "1;36");
    printColorLn("==========================================================", "1;35");
    cout << "Active Scale: ";
    printColorLn(activeScale, "1;33");
    cout << "----------------------------------------------------------\n";
    cout << "1. Display Academic Transcript Report\n";
    cout << "2. Add a New Semester\n";
    cout << "3. Manage Course Records (Add/Remove/Modify)\n";
    cout << "4. Delete a Semester\n";
    cout << "5. Switch / Configure Grading Scale\n";
    cout << "6. Target CGPA Goal Planner (\"What-If\" Solver)\n";
    cout << "7. Save Current Records to File\n";
    cout << "8. Load Records from File\n";
    cout << "9. Exit Application\n";
    cout << "----------------------------------------------------------\n";
    
    int choice = getValidatedInt("Select option (1-9): ", 1, 9);
    
    if (choice == 1) {
      displayReport(semesters, activeScale, customScales);
      pressEnterToContinue();
    } else if (choice == 2) {
      string semName = getValidatedString("Enter new Semester Name (e.g. Semester 2): ");
      semesters.push_back({semName, {}});
      printColorLn("Semester '" + semName + "' successfully created.", "1;32");
      pressEnterToContinue();
    } else if (choice == 3) {
      if (semesters.empty()) {
        printColorLn("No semesters available. Please create one first.", "1;31");
        pressEnterToContinue();
        continue;
      }
      clearScreen();
      printColorLn("Select Semester to Manage:", "1;35");
      for (size_t i = 0; i < semesters.size(); ++i) {
        cout << i + 1 << ". " << semesters[i].name << "\n";
      }
      int semIdx = getValidatedInt("Choose semester (1-" + to_string(semesters.size()) + "): ", 1, semesters.size()) - 1;
      
      clearScreen();
      printColorLn("Manage Courses for " + semesters[semIdx].name, "1;35");
      cout << "1. Add a Course\n";
      cout << "2. Remove a Course\n";
      cout << "3. Modify Course Details\n";
      int manageChoice = getValidatedInt("Select action (1-3): ", 1, 3);
      
      if (manageChoice == 1) {
        Course c;
        c.name = getValidatedString("Enter course title: ");
        c.grade = getValidatedGrade("Enter grade symbol (or percentage value): ", activeScale, customScales);
        c.credits = getValidatedDouble("Enter course credits: ", 0.5, 30.0);
        semesters[semIdx].courses.push_back(c);
        printColorLn("Course added successfully.", "1;32");
      } else if (manageChoice == 2) {
        if (semesters[semIdx].courses.empty()) {
          printColorLn("No courses found in this semester.", "1;31");
        } else {
          for (size_t i = 0; i < semesters[semIdx].courses.size(); ++i) {
            cout << i + 1 << ". " << semesters[semIdx].courses[i].name << "\n";
          }
          int courseIdx = getValidatedInt("Choose course to remove (1-" + to_string(semesters[semIdx].courses.size()) + "): ", 1, semesters[semIdx].courses.size()) - 1;
          semesters[semIdx].courses.erase(semesters[semIdx].courses.begin() + courseIdx);
          printColorLn("Course removed successfully.", "1;32");
        }
      } else if (manageChoice == 3) {
        if (semesters[semIdx].courses.empty()) {
          printColorLn("No courses found to modify.", "1;31");
        } else {
          for (size_t i = 0; i < semesters[semIdx].courses.size(); ++i) {
            cout << i + 1 << ". " << semesters[semIdx].courses[i].name << "\n";
          }
          int courseIdx = getValidatedInt("Choose course to modify (1-" + to_string(semesters[semIdx].courses.size()) + "): ", 1, semesters[semIdx].courses.size()) - 1;
          
          cout << "\nModifying: " << semesters[semIdx].courses[courseIdx].name << "\n";
          semesters[semIdx].courses[courseIdx].name = getValidatedString("Enter new course title: ");
          semesters[semIdx].courses[courseIdx].grade = getValidatedGrade("Enter new grade symbol: ", activeScale, customScales);
          semesters[semIdx].courses[courseIdx].credits = getValidatedDouble("Enter new course credits: ", 0.5, 30.0);
          printColorLn("Course details modified successfully.", "1;32");
        }
      }
      pressEnterToContinue();
    } else if (choice == 4) {
      if (semesters.empty()) {
        printColorLn("No semesters available to delete.", "1;31");
        pressEnterToContinue();
        continue;
      }
      clearScreen();
      printColorLn("Select Semester to Delete:", "1;35");
      for (size_t i = 0; i < semesters.size(); ++i) {
        cout << i + 1 << ". " << semesters[i].name << "\n";
      }
      int semIdx = getValidatedInt("Choose semester (1-" + to_string(semesters.size()) + "): ", 1, semesters.size()) - 1;
      string name = semesters[semIdx].name;
      semesters.erase(semesters.begin() + semIdx);
      printColorLn("Semester '" + name + "' successfully deleted.", "1;32");
      pressEnterToContinue();
    } else if (choice == 5) {
      clearScreen();
      printColorLn("Configure Grading Scale:", "1;35");
      cout << "1. Use Standard 4.0 Scale (A+, A, A-, B+...)\n";
      cout << "2. Use Standard 10.0 Scale (O, A+, A, B+...)\n";
      cout << "3. Use Percentage Scale (0 - 100%)\n";
      
      int customOffset = 4;
      vector<string> customNames;
      for (const auto& scalePair : customScales) {
        cout << customOffset << ". Use Custom Scale: " << scalePair.first << "\n";
        customNames.push_back(scalePair.first);
        customOffset++;
      }
      cout << customOffset << ". Add a Custom Scale Configuration\n";
      
      int scaleChoice = getValidatedInt("Choose scale option (1-" + to_string(customOffset) + "): ", 1, customOffset);
      
      if (scaleChoice == 1) {
        activeScale = "4.0";
        for (auto& sem : semesters) {
          for (auto& course : sem.courses) {
            course.grade = "A";
          }
        }
        printColorLn("Scale switched to 4.0. Existing grades defaulted to 'A'.", "1;32");
      } else if (scaleChoice == 2) {
        activeScale = "10.0";
        for (auto& sem : semesters) {
          for (auto& course : sem.courses) {
            course.grade = "A";
          }
        }
        printColorLn("Scale switched to 10.0. Existing grades defaulted to 'A'.", "1;32");
      } else if (scaleChoice == 3) {
        activeScale = "percent";
        for (auto& sem : semesters) {
          for (auto& course : sem.courses) {
            course.grade = "85";
          }
        }
        printColorLn("Scale switched to Percentage. Existing grades defaulted to '85'.", "1;32");
      } else if (scaleChoice == customOffset) {
        configureCustomScale(activeScale, customScales);
        for (auto& sem : semesters) {
          for (auto& course : sem.courses) {
            course.grade = customScales[activeScale][0].first;
          }
        }
        printColorLn("Existing grades defaulted to '" + activeScale + "' baseline.", "1;32");
      } else {
        activeScale = customNames[scaleChoice - 4];
        for (auto& sem : semesters) {
          for (auto& course : sem.courses) {
            course.grade = customScales[activeScale][0].first;
          }
        }
        printColorLn("Scale switched to " + activeScale + ". Existing grades defaulted.", "1;32");
      }
      saveToFile(recordsFile, semesters, activeScale, customScales);
      pressEnterToContinue();
    } else if (choice == 6) {
      targetPlanner(semesters, activeScale, customScales);
      pressEnterToContinue();
    } else if (choice == 7) {
      saveToFile(recordsFile, semesters, activeScale, customScales);
      pressEnterToContinue();
    } else if (choice == 8) {
      loadFromFile(recordsFile, semesters, activeScale, customScales);
      pressEnterToContinue();
    } else if (choice == 9) {
      saveToFile(recordsFile, semesters, activeScale, customScales);
      printColorLn("Exiting AeroGrade. Goodbye!", "1;32");
      break;
    }
  }

  return 0;
}
