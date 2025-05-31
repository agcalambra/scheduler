#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <sstream>

using namespace std;

struct TimeSlot {
    string day;
    string start_time;
    string end_time;
    int duration_hours;

    string toString() const {
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
    vector<int> enrolled_students;
};

struct Student {
    int id;
    string name;
    int min_units;
    int max_units;
    vector<string> preferred_courses;
    vector<string> unavailable_times;

    // Scheduling results
    vector<string> assigned_courses;
    int total_units = 0;
    bool scheduling_successful = false;
};

class Scheduler {
private:
    vector<Student> students;
    vector<Course> courses;
    unordered_map<string, Course*> course_map;
    mt19937 rng;

    vector<string> split(const string& str, char delimiter) {
        vector<string> tokens;
        stringstream ss(str);
        string token;
        while (getline(ss, token, delimiter)) {
            if (!token.empty()) {
                tokens.push_back(token);
            }
        }
        return tokens;
    }

    TimeSlot parseTimeSlot(const string& slot_str) {
        TimeSlot slot;

        try {
            size_t colon_pos = slot_str.find(':');
            if (colon_pos == string::npos) {
                cerr << "Warning: Invalid time slot format (no colon): " << slot_str << endl;
                slot.day = "Monday";
                slot.start_time = "08:00";
                slot.end_time = "09:00";
                slot.duration_hours = 1;
                return slot;
            }

            string time_part = slot_str.substr(0, colon_pos);
            string duration_str = slot_str.substr(colon_pos + 1);

            if (!duration_str.empty()) {
                slot.duration_hours = stoi(duration_str);
            } else {
                slot.duration_hours = 1;
            }

            size_t underscore_pos = time_part.find('_');
            if (underscore_pos == string::npos) {
                cerr << "Warning: Invalid time slot format (no underscore): " << slot_str << endl;
                slot.day = "Monday";
                slot.start_time = "08:00";
                slot.end_time = "09:00";
                return slot;
            }

            slot.day = time_part.substr(0, underscore_pos);

            string time_range = time_part.substr(underscore_pos + 1);
            size_t dash_pos = time_range.find('-');
            if (dash_pos == string::npos) {
                cerr << "Warning: Invalid time range format (no dash): " << slot_str << endl;
                slot.start_time = "08:00";
                slot.end_time = "09:00";
                return slot;
            }

            slot.start_time = time_range.substr(0, dash_pos);
            slot.end_time = time_range.substr(dash_pos + 1);

        } catch (const exception& e) {
            cerr << "Error parsing time slot '" << slot_str << "': " << e.what() << endl;
            // Provide safe defaults
            slot.day = "Monday";
            slot.start_time = "08:00";
            slot.end_time = "09:00";
            slot.duration_hours = 1;
        }

        return slot;
    }

public:
    Scheduler() : rng(random_device{}()) {}

    bool loadStudents(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening students file: " << filename << endl;
            return false;
        }

        string line;
        int line_number = 0;
        while (getline(file, line)) {
            line_number++;
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') continue;

            vector<string> parts = split(line, '|');
            if (parts.size() != 6) {
                cerr << "Warning: Skipping malformed line " << line_number << ": " << line << endl;
                continue;
            }

            try {
                Student student;
                student.id = stoi(parts[0]);
                student.name = parts[1];
                student.min_units = stoi(parts[2]);
                student.max_units = stoi(parts[3]);

                // Parse preferred courses
                if (!parts[4].empty()) {
                    student.preferred_courses = split(parts[4], ',');
                }

                // Parse unavailable times
                if (!parts[5].empty()) {
                    student.unavailable_times = split(parts[5], ',');
                }

                students.push_back(student);
            } catch (const exception& e) {
                cerr << "Error parsing student on line " << line_number << ": " << e.what() << endl;
                continue;
            }
        }

        cout << "Loaded " << students.size() << " students" << endl;
        return true;
    }

    bool loadCourses(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening courses file: " << filename << endl;
            return false;
        }

        string line;
        int line_number = 0;
        while (getline(file, line)) {
            line_number++;
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') continue;

            vector<string> parts = split(line, '|');
            if (parts.size() != 7) {
                cerr << "Warning: Skipping malformed course line " << line_number << ": " << line << endl;
                continue;
            }

            try {
                Course course;
                course.id = parts[0];
                course.name = parts[1];
                course.units = stoi(parts[2]);
                course.max_students = stoi(parts[3]);
                course.instructor = parts[4];
                course.room = parts[5];

                // Parse time slots
                if (!parts[6].empty()) {
                    vector<string> slot_strings = split(parts[6], ',');
                    for (const auto& slot_str : slot_strings) {
                        if (!slot_str.empty()) {
                            course.time_slots.push_back(parseTimeSlot(slot_str));
                        }
                    }
                }

                courses.push_back(course);
            } catch (const exception& e) {
                cerr << "Error parsing course on line " << line_number << ": " << e.what() << endl;
                continue;
            }
        }

        // Build course map after all courses are loaded
        for (size_t i = 0; i < courses.size(); i++) {
            course_map[courses[i].id] = &courses[i];
        }

        cout << "Loaded " << courses.size() << " courses" << endl;
        return true;
    }

    bool hasTimeConflict(const Student& student, const Course& course) {
        // Check if student has unavailable times that conflict with course
        for (const auto& unavailable : student.unavailable_times) {
            for (const auto& slot : course.time_slots) {
                if (unavailable == slot.toString()) {
                    return true;
                }
            }
        }

        // Check if student's already assigned courses conflict
        for (const auto& assigned_course_id : student.assigned_courses) {
            auto assigned_course = course_map[assigned_course_id];
            for (const auto& assigned_slot : assigned_course->time_slots) {
                for (const auto& new_slot : course.time_slots) {
                    if (timeSlotsOverlap(assigned_slot, new_slot)) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool timeSlotsOverlap(const TimeSlot& slot1, const TimeSlot& slot2) {
        if (slot1.day != slot2.day) return false;

        // Convert time to minutes for easier comparison
        auto timeToMinutes = [](const string& time) -> int {
            try {
                if (time.length() < 5) return 0; // Invalid time format
                int hours = stoi(time.substr(0, 2));
                int minutes = stoi(time.substr(3, 2));
                return hours * 60 + minutes;
            } catch (const exception& e) {
                cerr << "Warning: Invalid time format: " << time << endl;
                return 0; // Return safe default
            }
        };

        int start1 = timeToMinutes(slot1.start_time);
        int end1 = timeToMinutes(slot1.end_time);
        int start2 = timeToMinutes(slot2.start_time);
        int end2 = timeToMinutes(slot2.end_time);

        return !(end1 <= start2 || end2 <= start1);
    }

    bool meetsUnitConstraints(const Student& student, const string& course_id) {
        auto course = course_map[course_id];
        int new_total = student.total_units + course->units;

        if (new_total > student.max_units) return false;

        // Check unit type constraints
        int three_unit_count = 0;
        int one_unit_count = 0;

        for (const auto& assigned_id : student.assigned_courses) {
            if (course_map[assigned_id]->units == 3) three_unit_count++;
            else one_unit_count++;
        }

        if (course->units == 3) three_unit_count++;
        else one_unit_count++;

        // At most 2 one-unit courses
        if (one_unit_count > 2) return false;

        // If we're at min units, check if we have at least 3 three-unit courses
        if (new_total >= student.min_units && three_unit_count < 3) {
            return false;
        }

        return true;
    }

    bool canEnrollInCourse(const Student& student, const Course& course) {
        // Check if course is full
        if ((int)course.enrolled_students.size() >= course.max_students) return false;

        // Check if student already enrolled
        if (find(student.assigned_courses.begin(), student.assigned_courses.end(), course.id)
            != student.assigned_courses.end()) return false;

        // Check time conflicts
        if (hasTimeConflict(student, course)) return false;

        // Check unit constraints
        if (!meetsUnitConstraints(student, course.id)) return false;

        return true;
    }

    void scheduleStudents() {
        cout << "\nStarting scheduling process..." << endl;

        // Shuffle students for fair distribution
        shuffle(students.begin(), students.end(), rng);

        int scheduled_students = 0;
        int total_enrollments = 0;

        for (size_t student_idx = 0; student_idx < students.size(); student_idx++) {
            auto& student = students[student_idx];

            // Create a prioritized list of courses based on preferences
            vector<string> course_priority;

            // First, add preferred courses that are available
            for (const auto& pref : student.preferred_courses) {
                if (course_map.find(pref) != course_map.end()) {
                    course_priority.push_back(pref);
                }
            }

            // Add remaining courses (for students who need more options)
            for (size_t course_idx = 0; course_idx < courses.size(); course_idx++) {
                const string& course_id = courses[course_idx].id;
                if (find(course_priority.begin(), course_priority.end(), course_id) == course_priority.end()) {
                    course_priority.push_back(course_id);
                }
            }

            // Try to enroll in courses
            for (const auto& course_id : course_priority) {
                // Check if course exists
                if (course_map.find(course_id) == course_map.end()) {
                    continue;
                }

                // Find the course in our courses vector
                Course* course_ptr = nullptr;
                for (size_t i = 0; i < courses.size(); i++) {
                    if (courses[i].id == course_id) {
                        course_ptr = &courses[i];
                        break;
                    }
                }

                if (!course_ptr) continue;

                if (canEnrollInCourse(student, *course_ptr)) {
                    // Enroll student
                    student.assigned_courses.push_back(course_id);
                    student.total_units += course_ptr->units;
                    course_ptr->enrolled_students.push_back(student.id);
                    total_enrollments++;

                    // Update the course_map pointer
                    course_map[course_id] = course_ptr;

                    // Check if student has enough units
                    if (student.total_units >= student.min_units) {
                        // Verify minimum 3-unit course requirement
                        int three_unit_count = 0;
                        for (const auto& assigned_id : student.assigned_courses) {
                            // Find course safely
                            for (const auto& c : courses) {
                                if (c.id == assigned_id && c.units == 3) {
                                    three_unit_count++;
                                    break;
                                }
                            }
                        }

                        if (three_unit_count >= 3) {
                            student.scheduling_successful = true;
                            break;
                        }
                    }
                }

                // Stop if at maximum units
                if (student.total_units >= student.max_units) {
                    break;
                }
            }

            if (student.scheduling_successful) {
                scheduled_students++;
            }
        }

        cout << "Scheduling complete!" << endl;
        cout << "Successfully scheduled: " << scheduled_students << "/" << students.size() << " students" << endl;
        cout << "Total enrollments: " << total_enrollments << endl;
    }

    void saveScheduleToFile(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening output file: " << filename << endl;
            return;
        }

        // Write summary
        file << "# Course Scheduling Results" << endl;
        file << "# Generated schedule for " << students.size() << " students and " << courses.size() << " courses" << endl;
        file << endl;

        int successful = 0;
        int total_enrollments = 0;
        for (const auto& student : students) {
            if (student.scheduling_successful) successful++;
            total_enrollments += student.assigned_courses.size();
        }

        file << "SUMMARY" << endl;
        file << "Total Students: " << students.size() << endl;
        file << "Successfully Scheduled: " << successful << endl;
        file << "Success Rate: " << (100.0 * successful / students.size()) << "%" << endl;
        file << "Total Enrollments: " << total_enrollments << endl;
        file << endl;

        // Write student schedules
        file << "STUDENT SCHEDULES" << endl;
        file << "Format: StudentID|Name|TotalUnits|Success|AssignedCourses" << endl;
        file << endl;

        for (const auto& student : students) {
            file << student.id << "|" << student.name << "|" << student.total_units << "|"
                 << (student.scheduling_successful ? "YES" : "NO") << "|";

            for (size_t i = 0; i < student.assigned_courses.size(); i++) {
                file << student.assigned_courses[i];
                if (i < student.assigned_courses.size() - 1) file << ",";
            }
            file << endl;
        }

        file << endl;

        // Write course enrollment data
        file << "COURSE ENROLLMENTS" << endl;
        file << "Format: CourseID|Name|Units|EnrolledCount|MaxStudents|EnrolledStudents" << endl;
        file << endl;

        for (const auto& course : courses) {
            file << course.id << "|" << course.name << "|" << course.units << "|"
                 << course.enrolled_students.size() << "|" << course.max_students << "|";

            for (size_t i = 0; i < course.enrolled_students.size(); i++) {
                file << course.enrolled_students[i];
                if (i < course.enrolled_students.size() - 1) file << ",";
            }
            file << endl;
        }

        file.close();
        cout << "Schedule saved to " << filename << endl;
    }

    void printStatistics() {
        cout << "\n=== SCHEDULING STATISTICS ===" << endl;

        int successful = 0;
        int total_units_assigned = 0;
        int three_unit_enrollments = 0;
        int one_unit_enrollments = 0;

        for (const auto& student : students) {
            if (student.scheduling_successful) successful++;
            total_units_assigned += student.total_units;

            for (const auto& course_id : student.assigned_courses) {
                // Find course safely
                for (const auto& course : courses) {
                    if (course.id == course_id) {
                        if (course.units == 3) three_unit_enrollments++;
                        else one_unit_enrollments++;
                        break;
                    }
                }
            }
        }

        cout << "Students successfully scheduled: " << successful << "/" << students.size()
             << " (" << (100.0 * successful / students.size()) << "%)" << endl;
        cout << "Average units per student: " << (double)total_units_assigned / students.size() << endl;
        cout << "Total 3-unit course enrollments: " << three_unit_enrollments << endl;
        cout << "Total 1-unit course enrollments: " << one_unit_enrollments << endl;

        cout << "\nCourse utilization:" << endl;
        for (const auto& course : courses) {
            double utilization = (double)course.enrolled_students.size() / course.max_students * 100;
            cout << course.id << ": " << course.enrolled_students.size() << "/"
                 << course.max_students << " (" << utilization << "%)" << endl;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <students_file> <courses_file>" << endl;
        cout << "Example: " << argv[0] << " students_case1.txt courses_case1.txt" << endl;
        return 1;
    }

    string students_file = argv[1];
    string courses_file = argv[2];

    // Generate output filename based on input
    string output_file = "schedule_" + students_file.substr(students_file.find_last_of("/\\") + 1);
    output_file = output_file.substr(0, output_file.find_last_of('.')) + "_results.txt";

    Scheduler scheduler;

    cout << "=== Course Scheduler ===" << endl;
    cout << "Students file: " << students_file << endl;
    cout << "Courses file: " << courses_file << endl;
    cout << "Output file: " << output_file << endl;
    cout << endl;

    if (!scheduler.loadStudents(students_file)) {
        return 1;
    }

    if (!scheduler.loadCourses(courses_file)) {
        return 1;
    }

    scheduler.scheduleStudents();
    scheduler.saveScheduleToFile(output_file);
    scheduler.printStatistics();

    return 0;
}
