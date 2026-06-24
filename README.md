# AeroGrade — C++ GPA & CGPA Calculator Command Line Interface

[![License: MIT](https://img.shields.io/badge/License-MIT-purple.svg)](LICENSE)
[![Version](https://img.shields.io/badge/Version-1.0.0-indigo.svg)](package.json)
[![Security Policy](https://img.shields.io/badge/Security-Supported-emerald.svg)](SECURITY.md)

AeroGrade is a premium, feature-rich C++ command-line application designed for managing student course transcripts, computing semester GPAs, tracking overall cumulative CGPA, and performing target goal planning.

---

## 🚀 Key Features

* **Multi-Semester Architecture**: Manage and organize academic records grouped by semesters dynamically.
* **Flexible Grading System**: Built-in support for:
  * **Standard 4.0 Scale** (A+, A, B...)
  * **Standard 10.0 Scale** (O, A+, A...)
  * **Percentage Scale** (0-100%)
* **Custom Scale Configurator**: Define custom scales with unique grade letters and specific GPA point mappings dynamically at runtime.
* **Target CGPA Goal Planner**: Performs What-If analysis to calculate the required future average GPA based on target CGPA and remaining credits.
* **File Serialization**: Save academic configurations to `academic_records.txt` and load them on startup, preserving semesters, scales, and courses.
* **Robust Input Validation**: Implements clear stream state checks to handle user input errors gracefully without crash loops.
* **Interactive CLI Aesthetics**: Formatted output tables using ASCII borders and ANSI text color highlighting (green, yellow, red, cyan) for a polished appearance.

---

## 🛠️ Tech Stack

* **Language**: C++17
* **Compilers**: GCC / Clang (compatible on macOS, Linux, and Windows)
* **Libraries**: Standard C++ Library (`<iostream>`, `<vector>`, `<map>`, `<fstream>`, `<iomanip>`)

---

## 📁 File Structure

```text
├── main.cpp                # Core C++ application source code
├── calculator              # Compiled executable binary (macOS)
├── academic_records.txt    # Saved data file (generated on save)
├── LICENSE                 # MIT License agreement
├── SECURITY.md             # Security reporting policy
├── README.md               # Project documentation (MarkDown)
└── read.md                 # Compatibility documentation copy
```

---

## 💻 Compilation and Execution

To compile and run the application manually from the terminal:

### Compilation:
```bash
g++ -std=c++17 main.cpp -o calculator
```

### Execution:
```bash
./calculator
```

---

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## 👤 Developer

Developed with ❤️ by **Bhumika Rautela**.
