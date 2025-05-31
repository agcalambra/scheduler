#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <set>

using namespace std;

struct ScenarioConfig {
    string name;
    int num_students;
    int num_3unit_courses;
    int num_1unit_courses;
    int seats_per_course;
    bool allow_conflicts;
    bool use_half_hour_slots;
    double unavailable_time_probability;
    int max_preferences_per_student;

    void print() const {
        cout << "=== " << name << " ===" << endl;
        cout << "Students: " << num_students << endl;
        cout << "3-unit courses: " << num_3unit_courses << endl;
        cout << "1-unit courses: " << num_1unit_courses << endl;
        cout << "Seats per course: " << seats_per_course << endl;
        cout << "Allow schedule conflicts: " << (allow_conflicts ? "Yes" : "No") << endl;
        cout << "Half-hour time slots: " << (use_half_hour_slots ? "Yes" : "No") << endl;
        cout << "Unavailable time probability: " << (unavailable_time_probability * 100) << "%" << endl;
        cout << "Max preferences per student: " << max_preferences_per_student << endl;
        cout << endl;
    }
};

class ScenarioGenerator {
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

    vector<string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

    vector<string> instructors = {
        "Dr. Smith", "Prof. Johnson", "Dr. Williams", "Prof. Brown", "Dr. Jones",
        "Prof. Garcia", "Dr. Miller", "Prof. Davis", "Dr. Rodriguez", "Prof. Martinez"
    };

    vector<string> rooms = {
        "Room A101", "Room A102", "Room A103", "Room B201", "Room B202", "Room B203",
        "Lab C301", "Lab C302", "Hall D401", "Hall D402"
    };

public:
    ScenarioGenerator() : rng(random_device{}()) {}

    vector<string> generateTimeSlots(bool use_half_hour) {
        vector<string> slots;

        if (use_half_hour) {
            // Half-hour slots from 8:00 to 17:00
            for (const auto& day : days) {
                for (int hour = 8; hour < 17; hour++) {
                    for (int half = 0; half < 2; half++) {
                        string start_time = (hour < 10 ? "0" : "") + to_string(hour) + ":" + (half == 0 ? "00" : "30");
                        string end_time;
                        if (half == 0) {
                            end_time = (hour < 10 ? "0" : "") + to_string(hour) + ":30";
                        } else {
                            int next_hour = hour + 1;
                            end_time = (next_hour < 10 ? "0" : "") + to_string(next_hour) + ":00";
                        }
                        slots.push_back(day + "_" + start_time + "-" + end_time);
                    }
                }
            }
        } else {
            // Hour slots from 8:00 to 17:00
            for (const auto& day : days) {
                for (int hour = 8; hour < 17; hour++) {
                    string start_time = (hour < 10 ? "0" : "") + to_string(hour) + ":00";
                    string end_time = (hour + 1 < 10 ? "0" : "") + to_string(hour + 1) + ":00";
                    slots.push_back(day + "_" + start_time + "-" + end_time);
                }
            }
        }

        return slots;
    }

    string generateName() {
        uniform_int_distribution<> first_dist(0, first_names.size() - 1);
        uniform_int_distribution<> last_dist(0, last_names.size() - 1);
        return first_names[first_dist(rng)] + " " + last_names[last_dist(rng)];
    }

    void generateStudents(const ScenarioConfig& config, const string& filename) {
        ofstream file(filename);
        file << "# Student Data File - " << config.name << endl;
        file << "# Format: ID|Name|MinUnits|MaxUnits|PreferredCourses|UnavailableTimes" << endl;
        file << endl;

        set<string> used_names;
        vector<string> all_course_ids;

        // Generate course IDs
        for (int i = 1; i <= config.num_3unit_courses; i++) {
            all_course_ids.push_back("COURSE3_" + to_string(i));
        }
        for (int i = 1; i <= config.num_1unit_courses; i++) {
            all_course_ids.push_back("COURSE1_" + to_string(i));
        }

        vector<string> time_slots = generateTimeSlots(config.use_half_hour_slots);

        for (int i = 1; i <= config.num_students; i++) {
            try {
                string name;
                do {
                    name = generateName();
                } while (used_names.count(name));
                used_names.insert(name);

                // Generate unit range
                uniform_int_distribution<> range_dist(1, 4);
                pair<int, int> unit_range;
                switch (range_dist(rng)) {
                    case 1: unit_range = {12, 15}; break;
                    case 2: unit_range = {9, 12}; break;
                    case 3: unit_range = {15, 18}; break;
                    default: unit_range = {6, 9}; break;
                }

                // Generate preferred courses
                vector<string> preferred_courses = all_course_ids;
                shuffle(preferred_courses.begin(), preferred_courses.end(), rng);
                uniform_int_distribution<> pref_dist(3, min(config.max_preferences_per_student, (int)all_course_ids.size()));
                int num_prefs = pref_dist(rng);
                preferred_courses.resize(num_prefs);

                // Generate unavailable times
                vector<string> unavailable_times;
                uniform_real_distribution<> prob_dist(0.0, 1.0);
                if (prob_dist(rng) < config.unavailable_time_probability) {
                    vector<string> selected_times = time_slots;
                    shuffle(selected_times.begin(), selected_times.end(), rng);
                    uniform_int_distribution<> time_count_dist(1, 3);
                    int num_unavailable = time_count_dist(rng);
                    selected_times.resize(min(num_unavailable, (int)selected_times.size()));
                    unavailable_times = selected_times;
                }

                // Write student data
                file << i << "|" << name << "|" << unit_range.first << "|" << unit_range.second << "|";

                for (size_t j = 0; j < preferred_courses.size(); j++) {
                    file << preferred_courses[j];
                    if (j < preferred_courses.size() - 1) file << ",";
                }
                file << "|";

                for (size_t j = 0; j < unavailable_times.size(); j++) {
                    file << unavailable_times[j];
                    if (j < unavailable_times.size() - 1) file << ",";
                }
                file << endl;

            } catch (const exception& e) {
                cerr << "Error generating student " << i << ": " << e.what() << endl;
                continue;
            }
        }

        file.close();
        cout << "Generated " << config.num_students << " students for " << config.name << endl;
    }

    string getNextTimeSlot(const string& current_slot, bool use_half_hour) {
        try {
            size_t dash_pos = current_slot.find('-');
            size_t underscore_pos = current_slot.find('_');

            if (dash_pos == string::npos || underscore_pos == string::npos) {
                return ""; // Invalid format
            }

            string day = current_slot.substr(0, underscore_pos);
            string end_time = current_slot.substr(dash_pos + 1);

            if (end_time.length() < 5) {
                return ""; // Invalid time format
            }

            // Parse end time
            int hours = stoi(end_time.substr(0, 2));
            int minutes = stoi(end_time.substr(3, 2));

            // Calculate next slot
            if (use_half_hour) {
                minutes += 30;
                if (minutes >= 60) {
                    hours++;
                    minutes = 0;
                }
            } else {
                hours++;
            }

            if (hours >= 17) return ""; // End of day

            string new_start = (hours < 10 ? "0" : "") + to_string(hours) + ":" + (minutes < 10 ? "0" : "") + to_string(minutes);

            if (use_half_hour) {
                minutes += 30;
                if (minutes >= 60) {
                    hours++;
                    minutes = 0;
                }
            } else {
                hours++;
            }

            if (hours > 17) return ""; // Would extend beyond day

            string new_end = (hours < 10 ? "0" : "") + to_string(hours) + ":" + (minutes < 10 ? "0" : "") + to_string(minutes);

            return day + "_" + new_start + "-" + new_end;

        } catch (const exception& e) {
            cerr << "Error parsing time slot '" << current_slot << "': " << e.what() << endl;
            return "";
        }
    }

    void generateCourses(const ScenarioConfig& config, const string& filename) {
        ofstream file(filename);
        file << "# Course Data File - " << config.name << endl;
        file << "# Format: ID|Name|Units|MaxStudents|Instructor|Room|TimeSlots" << endl;
        file << endl;

        vector<string> time_slots = generateTimeSlots(config.use_half_hour_slots);
        set<string> used_time_slots;

        try {
            // Generate 3-unit courses
            for (int i = 1; i <= config.num_3unit_courses; i++) {
                string course_id = "COURSE3_" + to_string(i);
                string course_name = "Advanced Course " + to_string(i);

                // Select instructor and room
                uniform_int_distribution<> inst_dist(0, instructors.size() - 1);
                uniform_int_distribution<> room_dist(0, rooms.size() - 1);
                string instructor = instructors[inst_dist(rng)];
                string room = rooms[room_dist(rng)];

                // Generate time slots for 3-unit course
                vector<string> course_slots;
                vector<string> available_slots = time_slots;

                if (!config.allow_conflicts) {
                    // Remove already used slots
                    available_slots.erase(
                        remove_if(available_slots.begin(), available_slots.end(),
                            [&used_time_slots](const string& slot) {
                                return used_time_slots.count(slot) > 0;
                            }),
                        available_slots.end()
                    );
                }

                if (available_slots.size() >= 3) {
                    shuffle(available_slots.begin(), available_slots.end(), rng);

                    // Two 1-hour sessions and one 3-hour session
                    course_slots.push_back(available_slots[0]);
                    course_slots.push_back(available_slots[1]);
                    course_slots.push_back(available_slots[2]);

                    // Mark slots as used if no conflicts allowed
                    if (!config.allow_conflicts) {
                        for (const auto& slot : course_slots) {
                            used_time_slots.insert(slot);
                            // Also mark consecutive slots for 3-hour session (simplified)
                            string next1 = getNextTimeSlot(slot, config.use_half_hour_slots);
                            string next2 = getNextTimeSlot(next1, config.use_half_hour_slots);
                            if (!next1.empty()) used_time_slots.insert(next1);
                            if (!next2.empty()) used_time_slots.insert(next2);
                        }
                    }
                }

                // Write course data
                file << course_id << "|" << course_name << "|3|" << config.seats_per_course
                     << "|" << instructor << "|" << room << "|";

                // Write time slots with durations
                for (size_t j = 0; j < course_slots.size(); j++) {
                    int duration = (j == 2) ? 3 : 1; // Last slot is 3 hours
                    file << course_slots[j] << ":" << duration;
                    if (j < course_slots.size() - 1) file << ",";
                }
                file << endl;
            }

            // Generate 1-unit courses
            for (int i = 1; i <= config.num_1unit_courses; i++) {
                string course_id = "COURSE1_" + to_string(i);
                string course_name = "Elective Course " + to_string(i);

                uniform_int_distribution<> inst_dist(0, instructors.size() - 1);
                uniform_int_distribution<> room_dist(0, rooms.size() - 1);
                string instructor = instructors[inst_dist(rng)];
                string room = rooms[room_dist(rng)];

                // Single time slot
                vector<string> available_slots = time_slots;
                if (!config.allow_conflicts) {
                    available_slots.erase(
                        remove_if(available_slots.begin(), available_slots.end(),
                            [&used_time_slots](const string& slot) {
                                return used_time_slots.count(slot) > 0;
                            }),
                        available_slots.end()
                    );
                }

                string selected_slot;
                if (!available_slots.empty()) {
                    uniform_int_distribution<> slot_dist(0, available_slots.size() - 1);
                    selected_slot = available_slots[slot_dist(rng)];

                    if (!config.allow_conflicts) {
                        used_time_slots.insert(selected_slot);
                    }
                }

                file << course_id << "|" << course_name << "|1|" << config.seats_per_course
                     << "|" << instructor << "|" << room << "|";

                if (!selected_slot.empty()) {
                    file << selected_slot << ":1";
                }
                file << endl;
            }

        } catch (const exception& e) {
            cerr << "Error generating courses: " << e.what() << endl;
        }

        file.close();
        cout << "Generated " << config.num_3unit_courses << " 3-unit and "
             << config.num_1unit_courses << " 1-unit courses for " << config.name << endl;
    }

    static vector<ScenarioConfig> getScenarios() {
        vector<ScenarioConfig> scenarios;

        // Case 1: Easy - No conflicts, enough seats, hour slots
        scenarios.push_back({
            "Case 1 - Easy",
            50,          // students
            12,          // 3-unit courses
            6,           // 1-unit courses
            15,          // seats per course (plenty)
            false,       // no conflicts
            false,       // hour slots only
            0.1,         // 10% unavailable times
            8            // max preferences
        });

        // Case 2: Medium - With conflicts, enough seats, hour slots
        scenarios.push_back({
            "Case 2 - Medium",
            75,          // students
            10,          // 3-unit courses
            5,           // 1-unit courses
            12,          // seats per course (enough)
            true,        // allow conflicts
            false,       // hour slots only
            0.25,        // 25% unavailable times
            6            // max preferences
        });

        // Case 3: Difficult - With conflicts, limited seats, hour slots
        scenarios.push_back({
            "Case 3 - Difficult",
            100,         // students
            9,           // 3-unit courses
            4,           // 1-unit courses
            8,           // seats per course (limited)
            true,        // allow conflicts
            false,       // hour slots only
            0.3,         // 30% unavailable times
            5            // max preferences
        });

        // Case 4: Stretch - With conflicts, limited seats, half-hour slots
        scenarios.push_back({
            "Case 4 - Stretch Goal",
            120,         // students
            8,           // 3-unit courses
            6,           // 1-unit courses
            7,           // seats per course (very limited)
            true,        // allow conflicts
            true,        // half-hour slots
            0.35,        // 35% unavailable times
            4            // max preferences
        });

        return scenarios;
    }
};

int main() {
    ScenarioGenerator generator;
    vector<ScenarioConfig> scenarios = ScenarioGenerator::getScenarios();

    cout << "=== Course Scheduling Scenario Generator ===" << endl;
    cout << "This will generate 4 test cases of increasing difficulty." << endl;
    cout << endl;

    for (size_t i = 0; i < scenarios.size(); i++) {
        scenarios[i].print();

        string student_file = "students_case" + to_string(i + 1) + ".txt";
        string course_file = "courses_case" + to_string(i + 1) + ".txt";

        generator.generateStudents(scenarios[i], student_file);
        generator.generateCourses(scenarios[i], course_file);

        cout << "Files generated: " << student_file << ", " << course_file << endl;
        cout << string(50, '-') << endl;
    }

    cout << "\nTo test each scenario:" << endl;
    cout << "./scheduler students_case1.txt courses_case1.txt" << endl;
    cout << "./scheduler students_case2.txt courses_case2.txt" << endl;
    cout << "./scheduler students_case3.txt courses_case3.txt" << endl;
    cout << "./scheduler students_case4.txt courses_case4.txt" << endl;

    return 0;
}
