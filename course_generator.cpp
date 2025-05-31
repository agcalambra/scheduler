#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

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
};

class CourseGenerator {
private:
    mt19937 rng;

    vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    vector<string> time_starts = {"08:00", "09:00", "10:00", "11:00", "13:00", "14:00", "15:00"};

    vector<string> instructors = {
        "Dr. Smith", "Prof. Johnson", "Dr. Williams", "Prof. Brown", "Dr. Jones",
        "Prof. Garcia", "Dr. Miller", "Prof. Davis", "Dr. Rodriguez", "Prof. Martinez",
        "Dr. Hernandez", "Prof. Lopez", "Dr. Gonzalez"
    };

    vector<string> rooms = {
        "Room A101", "Room A102", "Room A103", "Room B201", "Room B202", "Room B203",
        "Lab C301", "Lab C302", "Hall D401", "Hall D402", "Studio E501", "Studio E502",
        "Auditorium F601"
    };

    vector<pair<string, string>> three_unit_courses = {
        {"MATH301", "Advanced Calculus"},
        {"PHYS401", "Quantum Physics"},
        {"CHEM302", "Organic Chemistry"},
        {"BIOL303", "Molecular Biology"},
        {"COMP404", "Data Structures & Algorithms"},
        {"ENGL201", "Advanced Literature"},
        {"HIST202", "World History"},
        {"PSYC203", "Cognitive Psychology"},
        {"ECON204", "Macroeconomics"}
    };

    vector<pair<string, string>> one_unit_courses = {
        {"ART101", "Art Appreciation"},
        {"MUS102", "Music Theory Basics"},
        {"PE103", "Physical Education"},
        {"LAB104", "Research Methods Lab"}
    };

public:
    CourseGenerator() : rng(random_device{}()) {}

    string getNextHour(const string& time) {
        int hour = stoi(time.substr(0, 2));
        hour++;
        return (hour < 10 ? "0" : "") + to_string(hour) + ":00";
    }

    string getThreeHoursLater(const string& time) {
        int hour = stoi(time.substr(0, 2));
        hour += 3;
        return (hour < 10 ? "0" : "") + to_string(hour) + ":00";
    }

    vector<TimeSlot> generateThreeUnitSchedule() {
        vector<TimeSlot> slots;

        // Select 3 different days for the course
        vector<string> selected_days = days;
        shuffle(selected_days.begin(), selected_days.end(), rng);
        selected_days.resize(3);

        // Randomly choose which day gets the 3-hour session
        uniform_int_distribution<> day_dist(0, 2);
        int long_day_idx = day_dist(rng);

        for (int i = 0; i < 3; i++) {
            TimeSlot slot;
            slot.day = selected_days[i];

            if (i == long_day_idx) {
                // 3-hour session
                uniform_int_distribution<> time_dist(0, 4); // Earlier times for 3-hour sessions
                string start = time_starts[time_dist(rng)];
                slot.start_time = start;
                slot.end_time = getThreeHoursLater(start);
                slot.duration_hours = 3;
            } else {
                // 1-hour session
                uniform_int_distribution<> time_dist(0, time_starts.size() - 1);
                string start = time_starts[time_dist(rng)];
                slot.start_time = start;
                slot.end_time = getNextHour(start);
                slot.duration_hours = 1;
            }

            slots.push_back(slot);
        }

        return slots;
    }

    vector<TimeSlot> generateOneUnitSchedule() {
        vector<TimeSlot> slots;

        // Single 1-hour session per week
        uniform_int_distribution<> day_dist(0, days.size() - 1);
        uniform_int_distribution<> time_dist(0, time_starts.size() - 1);

        TimeSlot slot;
        slot.day = days[day_dist(rng)];
        slot.start_time = time_starts[time_dist(rng)];
        slot.end_time = getNextHour(slot.start_time);
        slot.duration_hours = 1;

        slots.push_back(slot);
        return slots;
    }

    string selectInstructor() {
        uniform_int_distribution<> dist(0, instructors.size() - 1);
        return instructors[dist(rng)];
    }

    string selectRoom() {
        uniform_int_distribution<> dist(0, rooms.size() - 1);
        return rooms[dist(rng)];
    }

    vector<Course> generateCourses() {
        vector<Course> courses;

        // Generate 9 three-unit courses
        for (const auto& course_info : three_unit_courses) {
            Course course;
            course.id = course_info.first;
            course.name = course_info.second;
            course.units = 3;
            course.max_students = 10;
            course.time_slots = generateThreeUnitSchedule();
            course.instructor = selectInstructor();
            course.room = selectRoom();
            courses.push_back(course);
        }

        // Generate 4 one-unit courses
        for (const auto& course_info : one_unit_courses) {
            Course course;
            course.id = course_info.first;
            course.name = course_info.second;
            course.units = 1;
            course.max_students = 10;
            course.time_slots = generateOneUnitSchedule();
            course.instructor = selectInstructor();
            course.room = selectRoom();
            courses.push_back(course);
        }

        return courses;
    }

    void saveCoursesToFile(const vector<Course>& courses, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }

        // Write header
        file << "# Course Data File" << endl;
        file << "# Format: ID|Name|Units|MaxStudents|Instructor|Room|TimeSlots" << endl;
        file << "# TimeSlots format: Day_StartTime-EndTime:Duration,..." << endl;
        file << endl;

        for (const auto& course : courses) {
            file << course.id << "|" << course.name << "|" << course.units << "|"
                 << course.max_students << "|" << course.instructor << "|"
                 << course.room << "|";

            // Write time slots
            for (size_t i = 0; i < course.time_slots.size(); i++) {
                const auto& slot = course.time_slots[i];
                file << slot.toString() << ":" << slot.duration_hours;
                if (i < course.time_slots.size() - 1) file << ",";
            }
            file << endl;
        }

        file.close();
        cout << "Generated " << courses.size() << " courses and saved to " << filename << endl;
    }

    void printCourseSchedules(const vector<Course>& courses) {
        cout << "\nCourse Schedules:" << endl;
        cout << "=================" << endl;

        for (const auto& course : courses) {
            cout << course.id << " - " << course.name << " (" << course.units << " units)" << endl;
            cout << "Instructor: " << course.instructor << ", Room: " << course.room << endl;
            for (const auto& slot : course.time_slots) {
                cout << "  " << slot.day << " " << slot.start_time << "-" << slot.end_time;
                if (slot.duration_hours == 3) cout << " (3-hour session)";
                cout << endl;
            }
            cout << endl;
        }
    }
};

int main() {
    CourseGenerator generator;
    vector<Course> courses = generator.generateCourses();
    generator.saveCoursesToFile(courses, "courses.txt");
    generator.printCourseSchedules(courses);

    // Print summary
    int three_unit_count = 0, one_unit_count = 0;
    for (const auto& course : courses) {
        if (course.units == 3) three_unit_count++;
        else one_unit_count++;
    }

    cout << "Summary:" << endl;
    cout << "3-unit courses: " << three_unit_count << endl;
    cout << "1-unit courses: " << one_unit_count << endl;
    cout << "Total capacity: " << courses.size() * 10 << " student-course slots" << endl;

    return 0;
}
