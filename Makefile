# Makefile for C++ Course Scheduler System with Unit Testing
# No external dependencies required!

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Target executables
TARGETS = generate_students generate_courses scheduler scenario_generator unit_tester

# Source files
STUDENT_GEN_SRC = student_generator.cpp
COURSE_GEN_SRC = course_generator.cpp
SCHEDULER_SRC = scheduler.cpp
SCENARIO_GEN_SRC = scenario_generator.cpp
UNIT_TESTER_SRC = unit_tester.cpp

# Data files
DATA_FILES = students.txt courses.txt schedule.txt

.PHONY: all clean run setup test-scenarios grade

all: $(TARGETS)

# Build student generator
generate_students: $(STUDENT_GEN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Build course generator
generate_courses: $(COURSE_GEN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Build scheduler
scheduler: $(SCHEDULER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Build scenario generator
scenario_generator: $(SCENARIO_GEN_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Build unit tester
unit_tester: $(UNIT_TESTER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $<

# Generate and test all 4 scenarios with unit tests
test-scenarios: scenario_generator scheduler unit_tester
	@echo "=== Generating All Test Scenarios ==="
	./scenario_generator
	@echo ""
	@echo "=== Testing Case 1 (Easy) ==="
	./scheduler students_case1.txt courses_case1.txt
	./unit_tester students_case1.txt courses_case1.txt schedule_students_case1_results.txt
	@echo ""
	@echo "=== Testing Case 2 (Medium) ==="
	./scheduler students_case2.txt courses_case2.txt
	./unit_tester students_case2.txt courses_case2.txt schedule_students_case2_results.txt
	@echo ""
	@echo "=== Testing Case 3 (Difficult) ==="
	./scheduler students_case3.txt courses_case3.txt
	./unit_tester students_case3.txt courses_case3.txt schedule_students_case3_results.txt
	@echo ""
	@echo "=== Testing Case 4 (Stretch Goal) ==="
	./scheduler students_case4.txt courses_case4.txt
	./unit_tester students_case4.txt courses_case4.txt schedule_students_case4_results.txt
	@echo ""
	@echo "=== All Scenarios Complete ==="
	@echo "Check test_report_students_case*.txt for detailed results"

# Grade a specific case
grade-case1: scheduler unit_tester
	./scheduler students_case1.txt courses_case1.txt
	./unit_tester students_case1.txt courses_case1.txt schedule_students_case1_results.txt

grade-case2: scheduler unit_tester
	./scheduler students_case2.txt courses_case2.txt
	./unit_tester students_case2.txt courses_case2.txt schedule_students_case2_results.txt

grade-case3: scheduler unit_tester
	./scheduler students_case3.txt courses_case3.txt
	./unit_tester students_case3.txt courses_case3.txt schedule_students_case3_results.txt

grade-case4: scheduler unit_tester
	./scheduler students_case4.txt courses_case4.txt
	./unit_tester students_case4.txt courses_case4.txt schedule_students_case4_results.txt

# Clean build artifacts
clean:
	rm -f $(TARGETS)
	rm -f *.o

# Clean all generated files
clean-all: clean
	rm -f $(DATA_FILES)
	rm -f students_case*.txt courses_case*.txt schedule_*.txt test_report_*.txt

# Help target
help:
	@echo "Available targets:"
	@echo "  all              - Build all executables"
	@echo "  test-scenarios   - Generate and test all 4 difficulty cases"
	@echo "  grade-case1      - Test only Case 1 (Easy)"
	@echo "  grade-case2      - Test only Case 2 (Medium)"
	@echo "  grade-case3      - Test only Case 3 (Difficult)"
	@echo "  grade-case4      - Test only Case 4 (Stretch Goal)"
	@echo "  clean            - Remove executables"
	@echo "  clean-all        - Remove executables and all generated files"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Quick start for grading:"
	@echo "  make all                # Build everything"
	@echo "  make test-scenarios     # Generate and test all cases"
	@echo "  make grade-case1        # Test only easy case"
