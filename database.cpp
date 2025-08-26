#include "database.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <map>
#include <sstream>

Database::Database(const std::string& host, const std::string& user, const std::string& pass, const std::string& dbname)
try : session(mysqlx::SessionOption::HOST, host,
              mysqlx::SessionOption::PORT, 33060,
              mysqlx::SessionOption::USER, user,
              mysqlx::SessionOption::PWD, pass,
              mysqlx::SessionOption::DB, dbname),
    db(session.getSchema(dbname))
{
    try {
        db = session.getSchema(dbname);
        if (!db.existsInDatabase()) {
            throw std::runtime_error("Database " + dbname + " does not exist");
        }
    }
    catch (const mysqlx::Error& err) {
        throw std::runtime_error("Failed to get schema: " + std::string(err.what()));
    }
}
catch (const mysqlx::Error& err) {
    throw std::runtime_error("Connection failed: " + std::string(err.what()));
}

Database::~Database() {
    try { session.close(); } catch (...) {}
}

// --- Student ---
bool Database::studentExists(const std::string& studentId) {
    auto students = db.getTable("students");
    auto res = students.select("COUNT(*)").where("student_id = :sid").bind("sid", studentId).execute();
    auto row = res.fetchOne();
    return row && row[0].get<int>() > 0;
}
bool Database::validateStudentPassword(const std::string& studentId, const std::string& password) {
    auto students = db.getTable("students");
    auto res = students.select("password").where("student_id = :sid").bind("sid", studentId).execute();
    auto row = res.fetchOne();
    return row && row[0].get<std::string>() == password;
}
bool Database::changeStudentPassword(const std::string& studentId, const std::string& newPassword) {
    auto students = db.getTable("students");
    auto res = students.update().set("password", newPassword).where("student_id = :sid").bind("sid", studentId).execute();
    return res.getAffectedItemsCount() > 0;
}
bool Database::resetStudentPassword(const std::string& studentId) {
    return changeStudentPassword(studentId, "bnu");
}
int Database::getStudentSemester(const std::string& studentId) {
    auto students = db.getTable("students");
    auto res = students.select("semester").where("student_id = :sid").bind("sid", studentId).execute();
    auto row = res.fetchOne();
    return row ? row[0].get<int>() : -1;
}
std::string Database::getStudentDegree(const std::string& studentId) {
    auto students = db.getTable("students");
    auto res = students.select("degree").where("student_id = :sid").bind("sid", studentId).execute();
    auto row = res.fetchOne();
    return row ? std::string(row[0].get<std::string>()) : "";
}

// --- Faculty ---
bool Database::facultyExists(const std::string& email) {
    auto faculty = db.getTable("faculty");
    auto res = faculty.select("COUNT(*)").where("email = :email").bind("email", email).execute();
    auto row = res.fetchOne();
    return row && row[0].get<int>() > 0;
}
bool Database::validateFacultyPassword(const std::string& email, const std::string& password) {
    auto faculty = db.getTable("faculty");
    auto res = faculty.select("password").where("email = :email").bind("email", email).execute();
    auto row = res.fetchOne();
    return row && row[0].get<std::string>() == password;
}
std::string Database::getFacultyId(const std::string& email) {
    auto faculty = db.getTable("faculty");
    auto res = faculty.select("faculty_id").where("email = :email").bind("email", email).execute();
    auto row = res.fetchOne();
    return row ? std::to_string(row[0].get<int>()) : "";
}
std::string Database::getFacultyName(const std::string& email) {
    auto faculty = db.getTable("faculty");
    auto res = faculty.select("first_name", "last_name").where("email = :email").bind("email", email).execute();
    auto row = res.fetchOne();
    return row ? (row[0].get<std::string>() + " " + row[1].get<std::string>()) : "";
}
bool Database::changeFacultyPassword(const std::string& email, const std::string& newPassword) {
    auto faculty = db.getTable("faculty");
    auto res = faculty.update().set("password", newPassword).where("email = :email").bind("email", email).execute();
    return res.getAffectedItemsCount() > 0;
}
bool Database::resetFacultyPassword(const std::string& email) {
    return changeFacultyPassword(email, "faculty_scit");
}

// --- Course/Enrollment ---
std::vector<ScheduledCourse> Database::getAvailableScheduledCourses(int semester, const std::string& degree) {
    std::vector<ScheduledCourse> result;
    std::string query =
        "SELECT cs.schedule_id, cs.course_code, c.course_name, f.first_name, f.last_name, "
        "t.day_of_week, CAST(t.start_time AS CHAR), CAST(t.end_time AS CHAR), "
        "cl.room_number, cl.building, cs.timeslot_id "
        "FROM course_schedule cs "
        "JOIN courses c ON cs.course_code = c.course_code "
        "JOIN faculty f ON cs.faculty_id = f.faculty_id "
        "JOIN timeslots t ON cs.timeslot_id = t.timeslot_id "
        "JOIN classrooms cl ON cs.room_id = cl.room_id "
        "WHERE c.semester = ? AND c.department = ?";
    auto res = session.sql(query).bind(semester, degree).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne())) {
        ScheduledCourse sc;
        sc.schedule_id   = row[0].get<int>();
        sc.course_code   = row[1].get<std::string>();
        sc.course_name   = row[2].get<std::string>();
        sc.faculty_name  = row[3].get<std::string>() + " " + row[4].get<std::string>();
        sc.day           = row[5].get<std::string>();
        sc.start_time    = row[6].get<std::string>();
        sc.end_time      = row[7].get<std::string>();
        sc.room_number   = row[8].get<std::string>();
        sc.building      = row[9].get<std::string>();
        sc.timeslot_id   = row[10].get<int>();
        result.push_back(sc);
    }
    return result;
}
bool Database::isAlreadyEnrolled(const std::string& studentId, int schedule_id) {
    auto enrollments = db.getTable("enrollments");
    auto res = enrollments.select("COUNT(*)")
                   .where("student_id = :sid AND schedule_id = :scid")
                   .bind("sid", studentId)
                   .bind("scid", schedule_id)
                   .execute();
    auto row = res.fetchOne();
    return row && row[0].get<int>() > 0;
}
bool Database::hasClash(const std::string& studentId, int timeslot_id) {
    std::string query =
        "SELECT COUNT(*) FROM enrollments e "
        "JOIN course_schedule cs ON e.schedule_id = cs.schedule_id "
        "WHERE e.student_id = ? AND cs.timeslot_id = ?";
    auto res = session.sql(query).bind(studentId, timeslot_id).execute();
    auto row = res.fetchOne();
    return row && row[0].get<int>() > 0;
}
bool Database::addEnrollment(const std::string& studentId, int schedule_id) {
    std::string course_code;
    {
        std::string query = "SELECT course_code FROM course_schedule WHERE schedule_id = ?";
        auto res = session.sql(query).bind(schedule_id).execute();
        auto row = res.fetchOne();
        if (!row) return false;
        course_code = row[0].get<std::string>();
    }
    int max_students = 0;
    {
        std::string query = "SELECT max_students FROM courses WHERE course_code = ?";
        auto res = session.sql(query).bind(course_code).execute();
        auto row = res.fetchOne();
        if (!row) return false;
        max_students = row[0].get<int>();
    }
    int enrolled = 0;
    {
        std::string query = "SELECT COUNT(*) FROM enrollments WHERE schedule_id = ?";
        auto res = session.sql(query).bind(schedule_id).execute();
        auto row = res.fetchOne();
        if (!row) return false;
        enrolled = row[0].get<int>();
    }
    if (enrolled >= max_students)
        return false;
    auto enrollments = db.getTable("enrollments");
    enrollments.insert("student_id", "schedule_id").values(studentId, schedule_id).execute();
    return true;
}
bool Database::dropEnrollment(const std::string& studentId, int schedule_id) {
    auto enrollments = db.getTable("enrollments");
    auto res = enrollments.remove()
                   .where("student_id = :sid AND schedule_id = :scid")
                   .bind("sid", studentId)
                   .bind("scid", schedule_id)
                   .execute();
    return res.getAffectedItemsCount() > 0;
}
std::vector<ScheduledCourse> Database::getEnrolledCourses(const std::string& studentId) {
    std::vector<ScheduledCourse> result;
    std::string query =
        "SELECT cs.schedule_id, c.course_code, c.course_name, c.department, c.semester, "
        "f.faculty_id, CONCAT(f.first_name,' ',f.last_name) AS faculty_name, "
        "t.timeslot_id, t.day_of_week, CAST(t.start_time AS CHAR), CAST(t.end_time AS CHAR), "
        "cl.room_id, cl.room_number, cl.building "
        "FROM enrollments e "
        "JOIN course_schedule cs ON e.schedule_id = cs.schedule_id "
        "JOIN courses c ON cs.course_code = c.course_code "
        "JOIN faculty f ON cs.faculty_id = f.faculty_id "
        "JOIN timeslots t ON cs.timeslot_id = t.timeslot_id "
        "JOIN classrooms cl ON cs.room_id = cl.room_id "
        "WHERE e.student_id = ?";
    auto res = session.sql(query).bind(studentId).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne()))
    {
        ScheduledCourse sc;
        sc.schedule_id = row[0].get<int>();
        sc.course_code = row[1].get<std::string>();
        sc.course_name = row[2].get<std::string>();
        sc.department = row[3].get<std::string>();
        sc.semester = row[4].get<int>();
        sc.faculty_id = row[5].get<int>();
        sc.faculty_name = row[6].get<std::string>();
        sc.timeslot_id = row[7].get<int>();
        sc.day = row[8].get<std::string>();
        sc.start_time = row[9].get<std::string>();
        sc.end_time = row[10].get<std::string>();
        sc.room_id = row[11].get<std::string>();
        sc.room_number = row[12].get<std::string>();
        sc.building = row[13].get<std::string>();
        result.push_back(sc);
    }
    return result;
}
bool Database::isAdminPasswordCorrect(const std::string& password) {
    return password == "admin123";
}

// --- Add/Remove ---
void Database::addStudent(const std::string& id, const std::string& fname, const std::string& lname, const std::string& email, const std::string& degree, int semester) {
    auto students = db.getTable("students");
    students.insert("student_id", "first_name", "last_name", "email", "degree", "semester", "password")
        .values(id, fname, lname, email, degree, semester, "bnu")
        .execute();
}
void Database::removeStudent(const std::string& id) {
    auto students = db.getTable("students");
    students.remove().where("student_id = :sid").bind("sid", id).execute();
}
void Database::addFaculty(int faculty_id, const std::string& fname, const std::string& lname, const std::string& email, const std::string& degree, const std::string& qualification, const std::string& expertise_sub, const std::string& designation) {
    auto faculty = db.getTable("faculty");
    faculty.insert("faculty_id", "first_name", "last_name", "email", "degree", "qualification", "expertise_sub", "designation", "password")
        .values(faculty_id, fname, lname, email, degree, qualification, expertise_sub, designation, "faculty_scit")
        .execute();
}
void Database::removeFaculty(int faculty_id) {
    auto faculty = db.getTable("faculty");
    faculty.remove().where("faculty_id = :fid").bind("fid", faculty_id).execute();
}
void Database::addCourse(const std::string& code, const std::string& name, int credits, int sem, const std::string& dept, int max, const std::string& prereq) {
    auto courses = db.getTable("courses");
    courses.insert("course_code", "course_name", "credits", "semester", "department", "max_students", "prerequisites")
        .values(code, name, credits, sem, dept, max, prereq)
        .execute();
}
void Database::removeCourse(const std::string& code) {
    auto courses = db.getTable("courses");
    courses.remove().where("course_code = :ccode").bind("ccode", code).execute();
}
void Database::addClassroom(const std::string& id, const std::string& building, const std::string& number, int capacity, const std::string& room_type) {
    auto classrooms = db.getTable("classrooms");
    classrooms.insert("room_id", "building", "room_number", "capacity", "room_type")
        .values(id, building, number, capacity, room_type)
        .execute();
}
void Database::removeClassroom(const std::string& id) {
    auto classrooms = db.getTable("classrooms");
    classrooms.remove().where("room_id = :rid").bind("rid", id).execute();
}
void Database::addTimeslot(const std::string& day, const std::string& start, const std::string& end) {
    auto timeslots = db.getTable("timeslots");
    timeslots.insert("day_of_week", "start_time", "end_time")
        .values(day, start, end)
        .execute();
}
void Database::removeTimeslot(int timeslot_id) {
    auto timeslots = db.getTable("timeslots");
    timeslots.remove().where("timeslot_id = :tid").bind("tid", timeslot_id).execute();
}

// --- Scheduling ---
std::vector<std::pair<std::string, std::string>> Database::getUnscheduledCourses() {
    std::vector<std::pair<std::string, std::string>> resvec;
    std::string query =
        "SELECT course_code, course_name FROM courses WHERE course_code NOT IN (SELECT course_code FROM course_schedule)";
    auto res = session.sql(query).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne()))
        resvec.emplace_back(row[0].get<std::string>(), row[1].get<std::string>());
    return resvec;
}
std::vector<std::pair<int, std::string>> Database::getAllTimeslots() {
    std::vector<std::pair<int, std::string>> resvec;
    std::string query =
        "SELECT timeslot_id, CONCAT(day_of_week, ' ', start_time, '-', end_time) FROM timeslots";
    auto res = session.sql(query).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne()))
        resvec.emplace_back(row[0].get<int>(), row[1].get<std::string>());
    return resvec;
}
std::vector<std::pair<std::string, std::string>> Database::getAvailableRooms(int timeslot_id) {
    std::vector<std::pair<std::string, std::string>> resvec;
    std::string query =
        "SELECT room_id, CONCAT(room_number, ' ', building) FROM classrooms "
        "WHERE room_id NOT IN (SELECT room_id FROM course_schedule WHERE timeslot_id = ?)";
    auto res = session.sql(query).bind(timeslot_id).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne()))
        resvec.emplace_back(row[0].get<std::string>(), row[1].get<std::string>());
    return resvec;
}
std::vector<std::pair<int, std::string>> Database::getAvailableFaculty(int timeslot_id) {
    std::vector<std::pair<int, std::string>> resvec;
    std::string query =
        "SELECT faculty_id, CONCAT(first_name, ' ', last_name) FROM faculty "
        "WHERE faculty_id NOT IN (SELECT faculty_id FROM course_schedule WHERE timeslot_id = ?)";
    auto res = session.sql(query).bind(timeslot_id).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne()))
        resvec.emplace_back(row[0].get<int>(), row[1].get<std::string>());
    return resvec;
}
void Database::addCourseSchedule(const std::string& course_code, int faculty_id, int timeslot_id, const std::string& room_id) {
    auto course_schedule = db.getTable("course_schedule");
    course_schedule.insert("course_code", "faculty_id", "timeslot_id", "room_id")
        .values(course_code, faculty_id, timeslot_id, room_id)
        .execute();
}
std::vector<Database::ScheduledAssignment> Database::getAllCourseSchedules() {
    std::vector<ScheduledAssignment> result;
    std::string query =
        "SELECT cs.schedule_id, cs.course_code, c.course_name, CONCAT(f.first_name, ' ', f.last_name) AS faculty, "
        "CONCAT(cl.room_number, ' ', cl.building) AS room, CONCAT(t.day_of_week, ' ', t.start_time, '-', t.end_time) AS timeslot "
        "FROM course_schedule cs "
        "JOIN courses c ON cs.course_code = c.course_code "
        "JOIN faculty f ON cs.faculty_id = f.faculty_id "
        "JOIN timeslots t ON cs.timeslot_id = t.timeslot_id "
        "JOIN classrooms cl ON cs.room_id = cl.room_id";
    auto res = session.sql(query).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne()))
    {
        result.push_back({
            row[0].get<int>(),
            row[1].get<std::string>(),
            row[2].get<std::string>(),
            row[3].get<std::string>(),
            row[4].get<std::string>(),
            row[5].get<std::string>()
        });
    }
    return result;
}
void Database::removeCourseSchedule(int schedule_id) {
    {
        auto enrollments = db.getTable("enrollments");
        enrollments.remove().where("schedule_id = :sid").bind("sid", schedule_id).execute();
    }
    {
        auto course_schedule = db.getTable("course_schedule");
        course_schedule.remove().where("schedule_id = :sid").bind("sid", schedule_id).execute();
    }
}

// --- Faculty GUI ---
std::vector<std::string> Database::getFacultyCourses(int facultyId) {
    std::vector<std::string> result;
    std::string query =
        "SELECT DISTINCT cs.course_code, c.course_name FROM course_schedule cs "
        "JOIN courses c ON cs.course_code = c.course_code "
        "WHERE cs.faculty_id = ?";
    auto res = session.sql(query).bind(facultyId).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne())) {
        result.push_back(row[0].get<std::string>() + " - " + row[1].get<std::string>());
    }
    return result;
}
std::vector<Database::StudentInfo> Database::getEnrolledStudentsInCourse(const std::string& course_code) {
    std::vector<StudentInfo> result;
    std::string query =
        "SELECT DISTINCT s.student_id, s.first_name, s.last_name, s.email, s.semester, s.degree "
        "FROM enrollments e "
        "JOIN students s ON e.student_id = s.student_id "
        "JOIN course_schedule cs ON e.schedule_id = cs.schedule_id "
        "WHERE cs.course_code = ?";
    auto res = session.sql(query).bind(course_code).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne()))
    {
        StudentInfo si;
        si.student_id = row[0].get<std::string>();
        si.first_name = row[1].get<std::string>();
        si.last_name = row[2].get<std::string>();
        si.email = row[3].get<std::string>();
        si.semester = row[4].get<int>();
        si.degree = row[5].get<std::string>();
        result.push_back(si);
    }
    return result;
}
std::vector<ScheduledCourse> Database::getFacultyTimetable(int facultyId) {
    std::vector<ScheduledCourse> result;
    std::string query =
        "SELECT cs.schedule_id, cs.course_code, c.course_name, c.department, c.semester, "
        "cs.faculty_id, cs.timeslot_id, CONCAT(f.first_name, ' ', f.last_name) AS faculty_name, "
        "t.day_of_week, CAST(t.start_time AS CHAR), CAST(t.end_time AS CHAR), "
        "cs.room_id, cl.room_number, cl.building "
        "FROM course_schedule cs "
        "JOIN courses c ON cs.course_code = c.course_code "
        "JOIN faculty f ON cs.faculty_id = f.faculty_id "
        "JOIN timeslots t ON cs.timeslot_id = t.timeslot_id "
        "JOIN classrooms cl ON cs.room_id = cl.room_id "
        "WHERE cs.faculty_id = ?";
    auto res = session.sql(query).bind(facultyId).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne()))
    {
        ScheduledCourse sc;
        sc.schedule_id = row[0].get<int>();
        sc.course_code = row[1].get<std::string>();
        sc.course_name = row[2].get<std::string>();
        sc.department = row[3].get<std::string>();
        sc.semester = row[4].get<int>();
        sc.faculty_id = row[5].get<int>();
        sc.timeslot_id = row[6].get<int>();
        sc.faculty_name = row[7].get<std::string>();
        sc.day = row[8].get<std::string>();
        sc.start_time = row[9].get<std::string>();
        sc.end_time = row[10].get<std::string>();
        sc.room_id = row[11].get<std::string>();
        sc.room_number = row[12].get<std::string>();
        sc.building = row[13].get<std::string>();
        result.push_back(sc);
    }
    return result;
}
int Database::getTotalEnrolledStudents(const std::string& course_code) {
    std::string query =
        "SELECT COUNT(DISTINCT e.student_id) FROM enrollments e "
        "JOIN course_schedule cs ON e.schedule_id = cs.schedule_id "
        "WHERE cs.course_code = ?";
    auto res = session.sql(query).bind(course_code).execute();
    auto row = res.fetchOne();
    return row ? row[0].get<int>() : 0;
}

// --- Marks ---
void Database::addMarks(const std::string& course_code, const std::string& student_id, const std::string& assignment_name, int total_marks, int obtained_marks) {
    try {
        std::string query = "INSERT INTO marks (course_code, student_id, assignment_name, total_marks, obtained_marks) VALUES (?, ?, ?, ?, ?) "
                            "ON DUPLICATE KEY UPDATE total_marks = VALUES(total_marks), obtained_marks = VALUES(obtained_marks)";
        session.sql(query).bind(course_code, student_id, assignment_name, total_marks, obtained_marks).execute();
    }
    catch (const mysqlx::Error& err) {
        std::cout << "Error adding marks: " << err.what() << std::endl;
    }
}
void Database::updateMarks(const std::string& course_code, const std::string& student_id, const std::string& assignment_name, int obtained_marks) {
    try {
        std::string query = "UPDATE marks SET obtained_marks = ? WHERE course_code = ? AND student_id = ? AND assignment_name = ?";
        session.sql(query).bind(obtained_marks, course_code, student_id, assignment_name).execute();
    }
    catch (const mysqlx::Error& err) {
        std::cout << "Error updating marks: " << err.what() << std::endl;
    }
}
std::vector<std::string> Database::getAssignmentsForCourse(const std::string& course_code) {
    std::vector<std::string> assignments;
    std::string query = "SELECT DISTINCT assignment_name FROM marks WHERE course_code = ?";
    auto res = session.sql(query).bind(course_code).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne())) {
        assignments.push_back(row[0].get<std::string>());
    }
    return assignments;
}
std::vector<std::pair<std::string, std::pair<int, int>>> Database::getStudentMarksForAssignment(const std::string& course_code, const std::string& assignment_name) {
    std::vector<std::pair<std::string, std::pair<int, int>>> marks;
    std::string query = "SELECT student_id, total_marks, obtained_marks FROM marks WHERE course_code = ? AND assignment_name = ?";
    auto res = session.sql(query).bind(course_code, assignment_name).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne())) {
        marks.emplace_back(row[0].get<std::string>(), std::make_pair(row[1].get<int>(), row[2].get<int>()));
    }
    return marks;
}

// --- Student marks viewing ---
std::vector<Database::Mark> Database::getStudentMarks(const std::string& student_id, const std::string& course_code) {
    std::vector<Mark> result;
    std::string query =
        "SELECT m.assignment_name, m.total_marks, m.obtained_marks, c.course_name "
        "FROM marks m "
        "JOIN courses c ON m.course_code = c.course_code "
        "WHERE m.student_id = ?";
    if (!course_code.empty()) {
        query += " AND m.course_code = ?";
    }
    query += " ORDER BY m.assignment_name";
    mysqlx::SqlStatement stmt = session.sql(query).bind(student_id);
    if (!course_code.empty()) {
        stmt.bind(course_code);
    }
    auto res = stmt.execute();
    mysqlx::Row row;
    while ((row = res.fetchOne())) {
        Mark mark;
        mark.assignment_name = row[0].get<std::string>();
        mark.total_marks = row[1].get<int>();
        mark.obtained_marks = row[2].get<int>();
        mark.course_name = row[3].get<std::string>();
        result.push_back(mark);
    }
    return result;
}
std::vector<std::string> Database::getStudentCourses(const std::string& student_id) {
    std::vector<std::string> result;
    std::string query =
        "SELECT DISTINCT c.course_code, c.course_name "
        "FROM enrollments e "
        "JOIN course_schedule cs ON e.schedule_id = cs.schedule_id "
        "JOIN courses c ON cs.course_code = c.course_code "
        "WHERE e.student_id = ?";
    auto res = session.sql(query).bind(student_id).execute();
    mysqlx::Row row;
    while ((row = res.fetchOne())) {
        result.push_back(row[0].get<std::string>() + " - " + row[1].get<std::string>());
    }
    return result;
}
