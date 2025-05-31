# Makefile for C++ Course Scheduler System
# No external dependencies required!

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Target executables
TARGETS = generate_students generate_courses scheduler scenario_generator

# Source files
STUDENT_GEN_SRC = student_generator.cpp
COURSE_GEN_SRC = course_generator.cpp
SCHEDULER_SRC = scheduler.cpp
SCENARIO_GEN_SRC = scenario_generator.cpp

# Data files
DATA_FILES = students.txt courses.txt schedule.txt

.PHONY: all clean run setup install-deps test-scenarios

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

# No dependencies to install!
install-deps:
	@echo "No external dependencies required!"
	@echo "This version uses simple text files instead of JSON."

# Setup and run the complete system
setup: all

# Run the complete scheduling process
run: all
	@echo "=== Generating Students ==="
	./generate_students
	@echo ""
	@echo "=== Generating Courses ==="
	./generate_courses
	@echo ""
	@echo "=== Running Scheduler ==="
	./scheduler students.txt courses.txt
	@echo ""
	@echo "=== Process Complete ==="
	@echo "Check schedule_students_results.txt for results"

# Generate and test all 4 scenarios
test-scenarios: scenario_generator
	@echo "=== Generating All Test Scenarios ==="
	./scenario_generator
	@echo ""
	@echo "=== Testing Case 1 (Easy) ==="
	./scheduler students_case1.txt courses_case1.txt
	@echo ""
	@echo "=== Testing Case 2 (Medium) ==="
	./scheduler students_case2.txt courses_case2.txt
	@echo ""
	@echo "=== Testing Case 3 (Difficult) ==="
	./scheduler students_case3.txt courses_case3.txt
	@echo ""
	@echo "=== Testing Case 4 (Stretch Goal) ==="
	./scheduler students_case4.txt courses_case4.txt
	@echo ""
	@echo "=== All Scenarios Complete ==="

# Clean build artifacts
clean:
	rm -f $(TARGETS)
	rm -f *.o

# Clean all generated files
clean-all: clean
	rm -f $(DATA_FILES)

# Quick test run (generates data and schedules)
test: run
	@echo "=== Testing Results ==="
	@if [ -f schedule_students_results.txt ]; then \
		echo "✓ Schedule generated successfully"; \
		echo "File size: $$(wc -c < schedule_students_results.txt) bytes"; \
		echo "Number of students processed: $$(grep -c '^[0-9]' schedule_students_results.txt)"; \
	else \
		echo "✗ Schedule generation failed"; \
	fi

# Help target
help:
	@echo "Available targets:"
	@echo "  all           - Build all executables"
	@echo "  setup         - Install dependencies and build"
	@echo "  run           - Generate data and run scheduler"
	@echo "  test          - Run system and verify output"
	@echo "  test-scenarios- Generate and test all 4 difficulty cases"
	@echo "  clean         - Remove executables"
	@echo "  clean-all     - Remove executables and data files"
	@echo "  install-deps  - No dependencies needed!"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Quick start:"
	@echo "  make all              # Build everything"
	@echo "  make test-scenarios   # Test all 4 cases"
