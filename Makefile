# Makefile for C++ Course Scheduler System
# No external dependencies required!

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Target executables
TARGETS = generate_students generate_courses scheduler

# Source files
STUDENT_GEN_SRC = student_generator.cpp
COURSE_GEN_SRC = course_generator.cpp
SCHEDULER_SRC = scheduler.cpp

# Data files
DATA_FILES = students.txt courses.txt schedule.txt

.PHONY: all clean run setup install-deps

all: $(TARGETS)

# Build student generator
generate_students: $(STUDENT_GEN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS)

# Build course generator
generate_courses: $(COURSE_GEN_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS)

# Build scheduler
scheduler: $(SCHEDULER_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< $(LIBS)

# Install dependencies (macOS with Homebrew - DEFAULT)
install-deps:
	@echo "Installing dependencies for macOS..."
	@if command -v brew >/dev/null 2>&1; then \
		brew install nlohmann-json; \
	else \
		echo "Error: Homebrew not found. Please install Homebrew first:"; \
		echo "/bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""; \
		exit 1; \
	fi

# Install dependencies (Ubuntu/Debian)
install-deps-linux:
	sudo apt-get update
	sudo apt-get install -y nlohmann-json3-dev g++ make

# Setup and run the complete system
setup: install-deps all

# Run the complete scheduling process
run: all
	@echo "=== Generating Students ==="
	./generate_students
	@echo ""
	@echo "=== Generating Courses ==="
	./generate_courses
	@echo ""
	@echo "=== Running Scheduler ==="
	./scheduler
	@echo ""
	@echo "=== Process Complete ==="
	@echo "Check schedule.json for results"

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
	@if [ -f schedule.txt ]; then \
		echo "✓ Schedule generated successfully"; \
		echo "File size: $(wc -c < schedule.txt) bytes"; \
		echo "Number of students processed: $(grep -c '^[0-9]' schedule.txt)"; \
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
	@echo "  clean         - Remove executables"
	@echo "  clean-all     - Remove executables and JSON files"
	@echo "  install-deps      - No dependencies needed!"
	@echo "  install-deps-linux - No dependencies needed!"
	@echo "  help          - Show this help message"
	@echo ""
	@echo "Quick start:"
	@echo "  make setup    # Install deps and build"
	@echo "  make run      # Generate data and schedule"
