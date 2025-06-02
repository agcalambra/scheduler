## **Quick Start**

```bash
# Build everything and test all scenarios
make all && make test-scenarios
```

## **System Overview**

### **Components**
- **Data Generators**: Create realistic student and course data
- **Scheduler**: Your algorithm implementation (the assignment)  
- **Unit Tester**: Comprehensive testing with detailed feedback
- **4 Test Cases**: Easy → Medium → Difficult → Stretch Goal

### **File Structure**
```
course_scheduler/
├── student_generator.cpp       # Generates basic student data
├── course_generator.cpp        # Generates basic course data  
├── scenario_generator.cpp      # Generates 4 test difficulty levels
├── scheduler.cpp              # YOUR IMPLEMENTATION (main assignment)
├── unit_tester.cpp            # Comprehensive testing framework
├── Makefile                   # Build and test automation
└── README.md                  # This guide
```

## **Build Commands**

| Command | Description |
|---------|-------------|
| `make all` | Build all 5 executables (generators + scheduler + tester) |
| `make clean` | Remove compiled executables only |
| `make clean-all` | Remove executables AND all generated data files |
| `make help` | Show all available commands with descriptions |

## **Test Case Generation**

```bash
./scenario_generator           # Generate 4 test cases with increasing difficulty
```

**Creates 8 files:**
- `students_case1.txt` + `courses_case1.txt` (Easy - No conflicts, plenty seats)
- `students_case2.txt` + `courses_case2.txt` (Medium - Time conflicts, adequate seats)  
- `students_case3.txt` + `courses_case3.txt` (Difficult - Conflicts + seat competition)
- `students_case4.txt` + `courses_case4.txt` (Stretch - Max constraints + half-hour slots)

## **Running Your Scheduler**

```bash
# Basic usage
./scheduler <students_file> <courses_file>

# Test specific difficulty levels  
./scheduler students_case1.txt courses_case1.txt       # Easy case
./scheduler students_case2.txt courses_case2.txt       # Medium case
./scheduler students_case3.txt courses_case3.txt       # Difficult case
./scheduler students_case4.txt courses_case4.txt       # Stretch goal
```

**Output:** Creates `schedule_students_case*_results.txt` with your scheduling results.

## **Testing & Grading**

### **Test All Cases (Recommended)**
```bash
make test-scenarios            # Generate data + run scheduler + test all 4 cases
```

### **Test Individual Cases**
```bash
make grade-case1              # Test only Case 1 with detailed feedback
make grade-case2              # Test only Case 2 with detailed feedback  
make grade-case3              # Test only Case 3 with detailed feedback
make grade-case4              # Test only Case 4 with detailed feedback
```

### **Manual Unit Testing**
```bash
./unit_tester students_case1.txt courses_case1.txt schedule_students_case1_results.txt
```

## **Understanding Test Results**

### **Console Output**
Each test shows:
- **PASSED** or **FAILED** with points earned
- **WHY IT FAILED** - Specific error description  
- **ANALYSIS** - Root cause explanation
- **SUGGESTED FIXES** - Step-by-step solutions

### **Generated Reports**
- `simple_test_report.txt` - Overall summary with scores
- `detailed_test_report_students_case*.txt` - Comprehensive debugging guides

## **Test Cases Explained**

| Case | Students | Constraints | Expected Success Rate |
|------|----------|-------------|----------------------|
| **1 - Easy** | 50 | No conflicts, plenty seats, hour slots | 95%+ |
| **2 - Medium** | 75 | Time conflicts, adequate seats, hour slots | 80-90% |
| **3 - Difficult** | 100 | Conflicts + seat competition, hour slots | 60-75% |
| **4 - Stretch** | 120 | Max constraints + half-hour slots | 40-60% |

## **Grading Breakdown (30 Points Total)**

### **Case 1: Basic Requirements (5 points)**
- 1.1: Basic Scheduling (1 pt) - At least some students scheduled
- 1.2: Minimum Units (1 pt) - Successful students meet unit minimums  
- 1.3: Maximum Units (1 pt) - No student exceeds unit maximums
- 1.4: Minimum 3-Unit Courses (1 pt) - Successful students have ≥3 three-unit courses
- 1.5: Maximum 1-Unit Courses (1 pt) - No student has >2 one-unit courses

### **Case 2: Conflict Resolution (8 points)**
- 2.1: No Time Conflicts (2 pts) - No overlapping course times per student
- 2.2: Unavailable Times (2 pts) - Student constraints respected
- 2.3: No Duplicates (2 pts) - Same course not assigned twice  
- 2.4: Preferences (2 pts) - ≥50% of students get preferred courses

### **Case 3: Resource Competition (9 points)**
- 3.1: Course Capacity (3 pts) - No course exceeds enrollment limit
- 3.2: Competition Handling (3 pts) - ≥30% success under pressure
- 3.3: Resource Usage (3 pts) - ≥40% course seat utilization

### **Case 4: Advanced Features (8 points)**
- 4.1: Complex Time Handling (4 pts) - Correctly handles half-hour slots
- 4.2: Advanced Optimization (4 pts) - ≥25% success + ≥8 avg units under max constraints

## **Typical Workflow**

### **For Students (Assignment Submission)**
```bash
# 1. Build and test your implementation
make all
make test-scenarios

# 2. Debug any failing tests  
make grade-case1
cat simple_test_report.txt

# 3. Fix issues and retest
# (Edit scheduler.cpp based on feedback)
make clean && make all
make grade-case1

# 4. Submit when satisfied with results
```

## **Common Issues & Solutions**

### **Compilation Errors**
```bash
# Missing dependencies (shouldn't happen - system is self-contained)
make clean && make all

# Syntax errors in scheduler.cpp
# Fix your C++ code syntax
```

### **Test Failures**
- **Test 1.1 Fails**: No students scheduled → Check basic algorithm logic
- **Test 1.2 Fails**: Unit requirements not met → Validate constraint checking  
- **Test 2.1 Fails**: Time conflicts → Debug overlap detection
- **Test 3.1 Fails**: Capacity exceeded → Check enrollment limits

### **No Output Files**
```bash
# Scheduler crashes or doesn't run
./scheduler students_case1.txt courses_case1.txt  # Run manually to see errors
gdb ./scheduler                                   # Debug with GDB if needed
```

## **Algorithm Requirements**

Your `scheduler.cpp` must implement:

1. **Input Parsing**: Read students.txt and courses.txt
2. **Constraint Checking**:
   - 6-18 units per student
   - Max 2 one-unit courses, Min 3 three-unit courses  
   - No time conflicts
   - Respect unavailable times
   - Course capacity limits (10 students each)
3. **Output Generation**: Write results to schedule_*_results.txt
4. **Success Criteria**: Mark students successful only if ALL constraints met

## **Input File Formats**

### **Students File**
```
# Format: ID|Name|MinUnits|MaxUnits|PreferredCourses|UnavailableTimes
1|Alice Smith|12|15|MATH301,COMP404,PHYS401|Monday_08:00-09:00
```

### **Courses File**  
```
# Format: ID|Name|Units|MaxStudents|Instructor|Room|TimeSlots
MATH301|Advanced Calculus|3|10|Dr. Smith|Room A101|Monday_08:00-09:00:1,Wednesday_10:00-13:00:3
```

### **Output File**
```
STUDENT SCHEDULES
Format: StudentID|Name|TotalUnits|Success|AssignedCourses
1|Alice Smith|12|YES|MATH301,COMP404,PHYS401
```
