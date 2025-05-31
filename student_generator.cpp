#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <set>

using namespace std;

struct Student {
    int id;
    string name;
    int min_units;
    int max_units;
    vector<string> preferred_courses;
    vector<string> unavailable_times;
};

class StudentGenerator {
private:
    mt19937 rng;
    vector<string> first_names = {
        "Alice", "Bob", "Charlie", "Diana", "Edward", "Fiona", "George", "Hannah",
        "Ivan", "Julia", "Kevin", "Laura", "Michael", "Nina", "Oscar", "Paula",
        "Quinn", "Rachel", "Steve", "Tina", "Uma", "Victor", "Wendy", "Xavier",
        "Yara", "Zach", "Emma", "Liam", "Olivia", "Noah", "Ava", "Isabella",
        "Sophia", "Jackson", "Lucas", "Harper", "Evelyn", "Alexander", "Abigail",
        "Emily", "Elizabeth", "Mila", "Ella", "Avery", "Sofia", "Camila", "Aria",
        "Scarlett", "Victoria", "Madison", "Luna", "Grace", "Chloe", "Penelope"
    };

    vector<string> last_names = {
        "Smith", "Johnson", "Williams", "Brown", "Jones", "Garcia", "Miller",
        "Davis", "Rodriguez", "Martinez", "Hernandez", "Lopez", "Gonzalez",
        "Wilson", "Anderson", "Thomas", "Taylor", "Moore", "Jackson", "Martin",
        "Lee", "Perez", "Thompson", "White", "Harris", "Sanchez", "Clark",
        "Ramirez", "Lewis", "Robinson", "Walker", "Young", "Allen", "King",
        "Wright", "Scott", "Torres", "Nguyen", "Hill", "Flores", "Green"
    };

    vector<string> course_names = {
        "MATH301", "PHYS401", "CHEM302", "BIOL303", "COMP404",
        "ENGL201", "HIST202", "PSYC203", "ECON204",
        "ART101", "MUS102", "PE103", "LAB104"
    };

    vector<string> time_slots = {
        "Monday_08:00-09:00", "Monday_09:00-10:00", "Monday_10:00-11:00",
        "Monday_11:00-12:00", "Monday_13:00-14:00", "Monday_14:00-15:00",
        "Tuesday_08:00-09:00", "Tuesday_09:00-10:00", "Tuesday_10:00-11:00",
        "Tuesday_11:00-12:00", "Tuesday_13:00-14:00", "Tuesday_14:00-15:00",
        "Wednesday_08:00-09:00", "Wednesday_09:00-10:00", "Wednesday_10:00-11:00",
        "Wednesday_11:00-12:00", "Wednesday_13:00-14:00", "Wednesday_14:00-15:00",
        "Thursday_08:00-09:00", "Thursday_09:00-10:00", "Thursday_10:00-11:00",
        "Thursday_11:00-12:00", "Thursday_13:00-14:00", "Thursday_14:00-15:00",
        "Friday_08:00-09:00", "Friday_09:00-10:00", "Friday_10:00-11:00",
        "Friday_11:00-12:00", "Friday_13:00-14:00", "Friday_14:00-15:00"
    };

public:
    StudentGenerator() : rng(random_device{}()) {}

    string generateName() {
        uniform_int_distribution<> first_dist(0, first_names.size() - 1);
        uniform_int_distribution<> last_dist(0, last_names.size() - 1);
        return first_names[first_dist(rng)] + " " + last_names[last_dist(rng)];
    }

    vector<string> selectPreferredCourses() {
        // Students prefer 4-8 courses (more than they can take to create competition)
        uniform_int_distribution<> num_dist(4, 8);
        int num_courses = num_dist(rng);

        vector<string> selected_courses = course_names;
        shuffle(selected_courses.begin(), selected_courses.end(), rng);
        selected_courses.resize(num_courses);

        return selected_courses;
    }

    vector<string> generateUnavailableTimes() {
        // 20% chance of having unavailable times
        uniform_int_distribution<> chance_dist(1, 10);
        if (chance_dist(rng) > 2) {
            return {};
        }

        // If they have unavailable times, 1-3 slots
        uniform_int_distribution<> num_dist(1, 3);
        int num_unavailable = num_dist(rng);

        vector<string> selected_times = time_slots;
        shuffle(selected_times.begin(), selected_times.end(), rng);
        selected_times.resize(num_unavailable);

        return selected_times;
    }

    pair<int, int> generateUnitRange() {
        // Generate realistic unit ranges within 6-18 constraint
        uniform_int_distribution<> type_dist(1, 4);
        int type = type_dist(rng);

        switch(type) {
            case 1: return {12, 15}; // Full-time student
            case 2: return {9, 12};  // Part-time student
            case 3: return {15, 18}; // Heavy load student
            default: return {6, 9};  // Light load student
        }
    }

    vector<Student> generateStudents(int count) {
        vector<Student> students;
        set<string> used_names; // Avoid duplicate names

        for (int i = 0; i < count; i++) {
            Student student;
            student.id = i + 1;

            // Generate unique name
            do {
                student.name = generateName();
            } while (used_names.count(student.name));
            used_names.insert(student.name);

            auto unit_range = generateUnitRange();
            student.min_units = unit_range.first;
            student.max_units = unit_range.second;

            student.preferred_courses = selectPreferredCourses();
            student.unavailable_times = generateUnavailableTimes();

            students.push_back(student);
        }

        return students;
    }

    void saveStudentsToFile(const vector<Student>& students, const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }

        // Write header
        file << "# Student Data File" << endl;
        file << "# Format: ID|Name|MinUnits|MaxUnits|PreferredCourses|UnavailableTimes" << endl;
        file << "# Courses and times are comma-separated" << endl;
        file << endl;

        for (const auto& student : students) {
            file << student.id << "|" << student.name << "|"
                 << student.min_units << "|" << student.max_units << "|";

            // Write preferred courses
            for (size_t i = 0; i < student.preferred_courses.size(); i++) {
                file << student.preferred_courses[i];
                if (i < student.preferred_courses.size() - 1) file << ",";
            }
            file << "|";

            // Write unavailable times
            for (size_t i = 0; i < student.unavailable_times.size(); i++) {
                file << student.unavailable_times[i];
                if (i < student.unavailable_times.size() - 1) file << ",";
            }
            file << endl;
        }

        file.close();
        cout << "Generated " << students.size() << " students and saved to " << filename << endl;
    }
};

int main() {
    StudentGenerator generator;
    vector<Student> students = generator.generateStudents(100);
    generator.saveStudentsToFile(students, "students.txt");

    // Print some statistics
    int light_load = 0, part_time = 0, full_time = 0, heavy_load = 0;
    for (const auto& student : students) {
        if (student.max_units <= 9) light_load++;
        else if (student.max_units <= 12) part_time++;
        else if (student.max_units <= 15) full_time++;
        else heavy_load++;
    }

    cout << "\nStudent Distribution:" << endl;
    cout << "Light load (6-9 units): " << light_load << endl;
    cout << "Part-time (9-12 units): " << part_time << endl;
    cout << "Full-time (12-15 units): " << full_time << endl;
    cout << "Heavy load (15-18 units): " << heavy_load << endl;

    return 0;
}
