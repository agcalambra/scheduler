#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <stdexcept>

using namespace std;

// Simple and safe unit tester
class SimpleUnitTester {
private:
    struct TestResult {
        string test_name;
        bool passed;
        string error_message;
        string detailed_feedback;
        string suggested_fixes;
        int points_possible;
        int points_earned;
    };

    struct TimeSlot {
        string day;
        string start_time;
        string end_time;
        int duration_hours;

        TimeSlot() : duration_hours(0) {}

        string toString() const {
            if (day.empty() || start_time.empty() || end_time.empty()) {
                return "";
            }
            return day + "_" + start_time + "-" + end_time;
        }
    };

    struct Course {
        string id;
        string name;
        int units;
        int max_students;
        vector<TimeSlot> time_slots;
        string instructor;
        string room;

        Course() : units(0), max_students(0) {}
    };

    struct Student {
        int id;
        string name;
        int min_units;
        int max_units;
        vector<string> preferred_courses;
        vector<string> unavailable_times;

        Student() : id(0), min_units(0), max_units(0) {}
    };

    struct ScheduleResult {
        int student_id;
        string student_name;
        int total_units;
        bool success;
        vector<string> assigned_courses;

        ScheduleResult() : student_id(0), total_units(0), success(false) {}
    };

    vector<Student> students;
    vector<Course> courses;
    vector<ScheduleResult> results;
    unordered_map<string, int> course_index_map;
    vector<TestResult> test_results;

    bool data_loaded = false;
    bool results_loaded = false;

public:
    vector<string> safeSplit(const string& str, char delimiter) {
        vector<string> tokens;
        if (str.empty() || str.length() > 5000) return tokens;

        stringstream ss(str);
        string token;
        int count = 0;

        while (getline(ss, token, delimiter) && count < 100) {
            if (!token.empty() && token.length() < 500) {
                tokens.push_back(token);
            }
            count++;
        }
        return tokens;
    }

    int safeStringToInt(const string& str, int default_value = 0) {
        if (str.empty() || str.length() > 10) return default_value;
        try {
            return stoi(str);
        } catch (...) {
            return default_value;
        }
    }

    TimeSlot parseTimeSlot(const string& slot_str) {
        TimeSlot slot;
        if (slot_str.empty() || slot_str.length() > 50) return slot;

        try {
            size_t colon_pos = slot_str.find(':');
            if (colon_pos == string::npos) return slot;

            string time_part = slot_str.substr(0, colon_pos);
            string duration_str = slot_str.substr(colon_pos + 1);
            slot.duration_hours = safeStringToInt(duration_str, 1);

            size_t underscore_pos = time_part.find('_');
            if (underscore_pos == string::npos) return slot;

            slot.day = time_part.substr(0, underscore_pos);
            string time_range = time_part.substr(underscore_pos + 1);

            size_t dash_pos = time_range.find('-');
            if (dash_pos == string::npos) return slot;

            slot.start_time = time_range.substr(0, dash_pos);
            slot.end_time = time_range.substr(dash_pos + 1);

        } catch (...) {
            // Return empty slot on any error
        }
        return slot;
    }

    bool loadData(const string& students_file, const string& courses_file) {
        cout << "Loading input data..." << endl;

        // Load students
        ifstream sfile(students_file);
        if (!sfile.is_open()) {
            cout << "Error: Cannot open " << students_file << endl;
            return false;
        }

        string line;
        int student_count = 0;
        while (getline(sfile, line) && student_count < 1000) {
            if (line.empty() || line[0] == '#') continue;

            vector<string> parts = safeSplit(line, '|');
            if (parts.size() != 6) continue;

            try {
                Student student;
                student.id = safeStringToInt(parts[0]);
                if (student.id <= 0) continue;

                student.name = parts[1];
                student.min_units = safeStringToInt(parts[2]);
                student.max_units = safeStringToInt(parts[3]);

                if (student.min_units < 0 || student.max_units < 0 ||
                    student.min_units > student.max_units) continue;

                if (!parts[4].empty()) {
                    student.preferred_courses = safeSplit(parts[4], ',');
                }
                if (!parts[5].empty()) {
                    student.unavailable_times = safeSplit(parts[5], ',');
                }

                students.push_back(student);
                student_count++;
            } catch (...) {
                continue;
            }
        }
        sfile.close();

        // Load courses
        ifstream cfile(courses_file);
        if (!cfile.is_open()) {
            cout << "Error: Cannot open " << courses_file << endl;
            return false;
        }

        int course_count = 0;
        while (getline(cfile, line) && course_count < 100) {
            if (line.empty() || line[0] == '#') continue;

            vector<string> parts = safeSplit(line, '|');
            if (parts.size() != 7) continue;

            try {
                Course course;
                course.id = parts[0];
                if (course.id.empty()) continue;

                course.name = parts[1];
                course.units = safeStringToInt(parts[2]);
                course.max_students = safeStringToInt(parts[3]);

                if (course.units <= 0 || course.max_students <= 0) continue;

                course.instructor = parts[4];
                course.room = parts[5];

                if (!parts[6].empty()) {
                    vector<string> slot_strings = safeSplit(parts[6], ',');
                    for (const auto& slot_str : slot_strings) {
                        if (!slot_str.empty()) {
                            TimeSlot slot = parseTimeSlot(slot_str);
                            if (!slot.day.empty()) {
                                course.time_slots.push_back(slot);
                            }
                        }
                    }
                }

                courses.push_back(course);
                course_index_map[course.id] = course_count;
                course_count++;
            } catch (...) {
                continue;
            }
        }
        cfile.close();

        cout << "Loaded " << students.size() << " students and " << courses.size() << " courses" << endl;
        data_loaded = (students.size() > 0 && courses.size() > 0);
        return data_loaded;
    }

    bool loadResults(const string& results_file) {
        cout << "Loading results..." << endl;

        ifstream file(results_file);
        if (!file.is_open()) {
            cout << "Error: Cannot open " << results_file << endl;
            return false;
        }

        string line;
        bool in_student_section = false;
        int result_count = 0;

        while (getline(file, line) && result_count < 1000) {
            if (line.find("STUDENT SCHEDULES") != string::npos) {
                in_student_section = true;
                continue;
            }
            if (line.find("COURSE ENROLLMENTS") != string::npos) {
                break;
            }

            if (in_student_section && !line.empty() && line[0] != '#' &&
                line.find("Format:") == string::npos) {

                vector<string> parts = safeSplit(line, '|');
                if (parts.size() >= 4) {
                    try {
                        ScheduleResult result;
                        result.student_id = safeStringToInt(parts[0]);
                        if (result.student_id <= 0) continue;

                        result.student_name = parts[1];
                        result.total_units = safeStringToInt(parts[2]);
                        result.success = (parts[3] == "YES");

                        if (parts.size() > 4 && !parts[4].empty()) {
                            result.assigned_courses = safeSplit(parts[4], ',');
                        }

                        results.push_back(result);
                        result_count++;
                    } catch (...) {
                        continue;
                    }
                }
            }
        }
        file.close();

        cout << "Loaded " << results.size() << " results" << endl;
        results_loaded = (results.size() > 0);
        return results_loaded;
    }

    void addTest(const string& name, bool passed, const string& error,
                const string& feedback, const string& fixes, int points) {
        TestResult result;
        result.test_name = name;
        result.passed = passed;
        result.error_message = error;
        result.detailed_feedback = feedback;
        result.suggested_fixes = fixes;
        result.points_possible = points;
        result.points_earned = passed ? points : 0;
        test_results.push_back(result);

        // Immediate feedback
        cout << "\n" << string(50, '=') << endl;
        cout << "TEST: " << name << endl;
        cout << "RESULT: " << (passed ? "✓ PASSED" : "✗ FAILED") << endl;
        cout << "POINTS: " << result.points_earned << "/" << points << endl;

        if (!passed) {
            cout << "\nERROR: " << error << endl;
            if (!feedback.empty()) {
                cout << "ANALYSIS: " << feedback << endl;
            }
            if (!fixes.empty()) {
                cout << "FIXES: " << fixes << endl;
            }
        }
        cout << string(50, '=') << endl;
    }

    Course* getCourse(const string& course_id) {
        auto it = course_index_map.find(course_id);
        if (it != course_index_map.end() && it->second < (int)courses.size()) {
            return &courses[it->second];
        }
        return nullptr;
    }

    void runAllTests() {
        cout << "\n" << string(60, '=') << endl;
        cout << "RUNNING UNIT TESTS" << endl;
        cout << string(60, '=') << endl;

        if (!data_loaded) {
            addTest("Data Loading", false, "Could not load input files",
                   "Input files are missing or corrupted",
                   "1. Check if files exist\n2. Run scenario generator first", 0);
            return;
        }

        if (!results_loaded) {
            addTest("Results Loading", false, "Could not load results file",
                   "Scheduler didn't generate proper output",
                   "1. Run scheduler manually\n2. Check for crashes", 0);
            return;
        }

        runBasicTests();
    }

    void runBasicTests() {
        cout << "\nRunning Case 1: Basic Requirements" << endl;

        // Test 1.1: Basic Scheduling
        try {
            bool found_successful = false;
            int total = 0, successful = 0;

            for (const auto& result : results) {
                total++;
                if (result.success) {
                    successful++;
                    found_successful = true;
                }
            }

            string error_msg = "";
            string feedback = "";
            string fixes = "";

            if (!found_successful) {
                error_msg = "No students were successfully scheduled";
                if (total == 0) {
                    feedback = "No student data found in results";
                    fixes = "1. Check scheduler output format\n2. Verify file generation";
                } else {
                    feedback = "Found " + to_string(total) + " students but none successful";
                    fixes = "1. Check constraint logic\n2. Verify success flag setting\n3. Debug scheduling algorithm";
                }
            } else {
                feedback = to_string(successful) + "/" + to_string(total) + " students successful";
            }

            addTest("1.1: Basic Scheduling", found_successful, error_msg, feedback, fixes, 1);

        } catch (...) {
            addTest("1.1: Basic Scheduling", false, "Test crashed",
                   "Exception during basic scheduling test",
                   "1. Check for memory issues\n2. Validate data structures", 1);
        }

        // Test 1.2: Minimum Units
        try {
            bool all_meet_min = true;
            int violations = 0;

            for (const auto& result : results) {
                if (result.success) {
                    for (const auto& student : students) {
                        if (student.id == result.student_id) {
                            if (result.total_units < student.min_units) {
                                violations++;
                                all_meet_min = false;
                            }
                            break;
                        }
                    }
                }
            }

            string error_msg = violations > 0 ? to_string(violations) + " students below minimum units" : "";
            string feedback = violations > 0 ?
                "Students marked successful don't meet unit requirements" :
                "All successful students meet minimum units";
            string fixes = violations > 0 ?
                "1. Check: total_units >= min_units before success\n2. Validate unit counting" : "";

            addTest("1.2: Minimum Units", all_meet_min, error_msg, feedback, fixes, 1);

        } catch (...) {
            addTest("1.2: Minimum Units", false, "Test crashed",
                   "Exception during minimum unit test",
                   "1. Check data access\n2. Validate student matching", 1);
        }

        // Test 1.3: Maximum Units
        try {
            bool all_under_max = true;
            int violations = 0;

            for (const auto& result : results) {
                for (const auto& student : students) {
                    if (student.id == result.student_id) {
                        if (result.total_units > student.max_units) {
                            violations++;
                            all_under_max = false;
                        }
                        break;
                    }
                }
            }

            string error_msg = violations > 0 ? to_string(violations) + " students exceed maximum units" : "";
            string feedback = violations > 0 ?
                "Students have more units than allowed" :
                "All students within maximum unit limits";
            string fixes = violations > 0 ?
                "1. Check: total_units <= max_units\n2. Stop enrolling at max" : "";

            addTest("1.3: Maximum Units", all_under_max, error_msg, feedback, fixes, 1);

        } catch (...) {
            addTest("1.3: Maximum Units", false, "Test crashed",
                   "Exception during maximum unit test",
                   "1. Check bounds\n2. Validate data", 1);
        }

        // Test 1.4: Minimum 3-Unit Courses
        try {
            bool all_meet_requirement = true;
            int violations = 0;

            for (const auto& result : results) {
                if (result.success) {
                    int three_unit_count = 0;
                    for (const auto& course_id : result.assigned_courses) {
                        Course* course = getCourse(course_id);
                        if (course && course->units == 3) {
                            three_unit_count++;
                        }
                    }

                    if (three_unit_count < 3) {
                        violations++;
                        all_meet_requirement = false;
                    }
                }
            }

            string error_msg = violations > 0 ? to_string(violations) + " students lack 3-unit courses" : "";
            string feedback = violations > 0 ?
                "Successful students need at least 3 three-unit courses" :
                "All successful students have enough 3-unit courses";
            string fixes = violations > 0 ?
                "1. Count 3-unit courses before marking success\n2. Prioritize 3-unit enrollment" : "";

            addTest("1.4: Minimum 3-Unit Courses", all_meet_requirement, error_msg, feedback, fixes, 1);

        } catch (...) {
            addTest("1.4: Minimum 3-Unit Courses", false, "Test crashed",
                   "Exception during 3-unit course test",
                   "1. Check course lookup\n2. Validate counting", 1);
        }

        // Test 1.5: Maximum 1-Unit Courses
        try {
            bool all_meet_limit = true;
            int violations = 0;

            for (const auto& result : results) {
                int one_unit_count = 0;
                for (const auto& course_id : result.assigned_courses) {
                    Course* course = getCourse(course_id);
                    if (course && course->units == 1) {
                        one_unit_count++;
                    }
                }

                if (one_unit_count > 2) {
                    violations++;
                    all_meet_limit = false;
                }
            }

            string error_msg = violations > 0 ? to_string(violations) + " students have too many 1-unit courses" : "";
            string feedback = violations > 0 ?
                "Students limited to 2 one-unit courses maximum" :
                "All students within 1-unit course limits";
            string fixes = violations > 0 ?
                "1. Count 1-unit courses before enrolling\n2. Limit to 2 maximum" : "";

            addTest("1.5: Maximum 1-Unit Courses", all_meet_limit, error_msg, feedback, fixes, 1);

        } catch (...) {
            addTest("1.5: Maximum 1-Unit Courses", false, "Test crashed",
                   "Exception during 1-unit course test",
                   "1. Check course lookup\n2. Validate counting", 1);
        }
    }

    void printSummary() {
        cout << "\n" << string(60, '=') << endl;
        cout << "FINAL RESULTS SUMMARY" << endl;
        cout << string(60, '=') << endl;

        int total_points = 0, earned_points = 0;
        int passed = 0, failed = 0;

        for (const auto& test : test_results) {
            total_points += test.points_possible;
            earned_points += test.points_earned;
            if (test.passed) passed++;
            else failed++;
        }

        cout << "\nOVERALL SCORE: " << earned_points << "/" << total_points;
        if (total_points > 0) {
            cout << " (" << (100.0 * earned_points / total_points) << "%)";
        }
        cout << "\nTESTS PASSED: " << passed << "/" << (passed + failed) << endl;

        cout << "\nTEST BREAKDOWN:" << endl;
        for (const auto& test : test_results) {
            cout << (test.passed ? "✓" : "✗") << " " << test.test_name;
            if (test.points_possible > 0) {
                cout << " [" << test.points_earned << "/" << test.points_possible << "]";
            }
            cout << endl;
        }
    }

    void saveReport(const string& filename) {
        ofstream file(filename);
        file << "# UNIT TEST DETAILED REPORT" << endl;
        file << "# Course Scheduler Testing Results" << endl;
        file << endl;

        int total_points = 0, earned_points = 0;
        for (const auto& test : test_results) {
            total_points += test.points_possible;
            earned_points += test.points_earned;
        }

        file << "SUMMARY" << endl;
        file << "=======" << endl;
        file << "Score: " << earned_points << "/" << total_points << " points" << endl;
        file << "Percentage: " << (total_points > 0 ? 100.0 * earned_points / total_points : 0) << "%" << endl;
        file << endl;

        file << "DETAILED RESULTS" << endl;
        file << "================" << endl;

        for (const auto& test : test_results) {
            file << endl << string(40, '-') << endl;
            file << "TEST: " << test.test_name << endl;
            file << "RESULT: " << (test.passed ? "PASSED" : "FAILED") << endl;
            file << "POINTS: " << test.points_earned << "/" << test.points_possible << endl;

            if (!test.passed) {
                file << endl << "ERROR: " << test.error_message << endl;
                if (!test.detailed_feedback.empty()) {
                    file << "ANALYSIS: " << test.detailed_feedback << endl;
                }
                if (!test.suggested_fixes.empty()) {
                    file << "SUGGESTED FIXES:" << endl << test.suggested_fixes << endl;
                }
            }
        }

        file.close();
        cout << "\nDetailed report saved to: " << filename << endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        if (argc != 4) {
            cout << "Usage: " << argv[0] << " <students_file> <courses_file> <results_file>" << endl;
            return 1;
        }

        SimpleUnitTester tester;

        cout << "=== Simple Course Scheduler Unit Tester ===" << endl;
        cout << "Students: " << argv[1] << endl;
        cout << "Courses: " << argv[2] << endl;
        cout << "Results: " << argv[3] << endl;

        if (!tester.loadData(argv[1], argv[2])) {
            cout << "Failed to load input data" << endl;
            return 1;
        }

        if (!tester.loadResults(argv[3])) {
            cout << "Failed to load results" << endl;
            return 1;
        }

        tester.runAllTests();
        tester.printSummary();

        string report_file = "simple_test_report.txt";
        tester.saveReport(report_file);

        return 0;

    } catch (...) {
        cout << "Fatal error in unit tester" << endl;
        return 1;
    }
}
