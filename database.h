#pragma once
#include <string>
#include <vector>
#include <mysqlx/xdevapi.h>

struct ScheduledCourse {
    int schedule_id;
    std::string course_code, course_name, department;
    int semester, faculty_id, timeslot_id;
    std::string faculty_name, day, start_time, end_time;
    std::string room_id, room_number, building;
};

class Database {
    mysqlx::Session session;
    mysqlx::Schema db;

public:
    Database(const std::string& host, const std::string& user, const std::string& pass, const std::string& dbname);
    ~Database();

    // Student
    bool studentExists(const std::string& studentId);
    bool validateStudentPassword(const std::string& studentId, const std::string& password);
    bool changeStudentPassword(const std::string& studentId, const std::string& newPassword);
    bool resetStudentPassword(const std::string& studentId);

    int getStudentSemester(const std::string& studentId);
    std::string getStudentDegree(const std::string& studentId);

    // Faculty
    bool facultyExists(const std::string& email);
    bool validateFacultyPassword(const std::string& email, const std::string& password);
    std::string getFacultyId(const std::string& email);
    std::string getFacultyName(const std::string& email);
    bool changeFacultyPassword(const std::string& email, const std::string& newPassword);
    bool resetFacultyPassword(const std::string& email);

    // Courses and Enrollment
    std::vector<ScheduledCourse> getAvailableScheduledCourses(int semester, const std::string& degree);
    bool isAlreadyEnrolled(const std::string& studentId, int schedule_id);
    bool hasClash(const std::string& studentId, int timeslot_id);
    bool addEnrollment(const std::string& studentId, int schedule_id);
    bool dropEnrollment(const std::string& studentId, int schedule_id);
    std::vector<ScheduledCourse> getEnrolledCourses(const std::string& studentId);

    // Admin password
    bool isAdminPasswordCorrect(const std::string& password);

    // Add/remove entities
    void addStudent(const std::string& id, const std::string& fname, const std::string& lname, const std::string& email, const std::string& degree, int semester);
    void removeStudent(const std::string& id);
    void addFaculty(int faculty_id, const std::string& fname, const std::string& lname, const std::string& email, const std::string& degree, const std::string& qualification, const std::string& expertise_sub, const std::string& designation);
    void removeFaculty(int faculty_id);
    void addCourse(const std::string& code, const std::string& name, int credits, int sem, const std::string& dept, int max, const std::string& prereq);
    void removeCourse(const std::string& code);

    void addClassroom(const std::string& id, const std::string& building, const std::string& number, int capacity, const std::string& room_type);
    void removeClassroom(const std::string& id);
    void addTimeslot(const std::string& day, const std::string& start, const std::string& end);
    void removeTimeslot(int timeslot_id);

    // Scheduling
    std::vector<std::pair<std::string, std::string>> getUnscheduledCourses();
    std::vector<std::pair<int, std::string>> getAllTimeslots();
    std::vector<std::pair<std::string, std::string>> getAvailableRooms(int timeslot_id);
    std::vector<std::pair<int, std::string>> getAvailableFaculty(int timeslot_id);
    void addCourseSchedule(const std::string& course_code, int faculty_id, int timeslot_id, const std::string& room_id);
    struct ScheduledAssignment {
        int schedule_id;
        std::string course_code, course_name, faculty_name, room, timeslot;
    };
    std::vector<ScheduledAssignment> getAllCourseSchedules();
    void removeCourseSchedule(int schedule_id);

    // Faculty GUI needs:
    std::vector<std::string> getFacultyCourses(int facultyId);
    struct StudentInfo {
        std::string student_id;
        std::string first_name;
        std::string last_name;
        std::string email;
        int semester;
        std::string degree;
    };
    std::vector<StudentInfo> getEnrolledStudentsInCourse(const std::string& course_code);
    std::vector<ScheduledCourse> getFacultyTimetable(int facultyId);
    int getTotalEnrolledStudents(const std::string& course_code);

    // Marks
    void addMarks(const std::string& course_code, const std::string& student_id, const std::string& assignment_name, int total_marks, int obtained_marks);
    void updateMarks(const std::string& course_code, const std::string& student_id, const std::string& assignment_name, int obtained_marks);
    std::vector<std::string> getAssignmentsForCourse(const std::string& course_code);
    std::vector<std::pair<std::string, std::pair<int, int>>> getStudentMarksForAssignment(const std::string& course_code, const std::string& assignment_name);

    // Student marks viewing
    struct Mark {
        std::string assignment_name;
        int total_marks;
        int obtained_marks;
        std::string course_name;
    };
    std::vector<Mark> getStudentMarks(const std::string& student_id, const std::string& course_code = "");
    std::vector<std::string> getStudentCourses(const std::string& student_id);
};
