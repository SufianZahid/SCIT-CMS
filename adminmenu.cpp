#include "adminmenu.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QLabel>
#include <QFont>
#include <QPixmap>
#include <QPainter>

AdminMenu::AdminMenu(Database *db, QWidget *parent)
    : QWidget(parent), db(db)
{
    setWindowTitle("Admin Menu");
    setMinimumSize(1000, 700);

    setAttribute(Qt::WA_StyledBackground, true);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 10, 20, 20);
    mainLayout->setSpacing(20);

    auto header = new QLabel("ADMIN MENU");
    header->setAlignment(Qt::AlignCenter);
    QFont headerFont("Georgia", 50, QFont::Bold);
    header->setFont(headerFont);
    header->setStyleSheet("color: white; background: transparent;");
    header->setContentsMargins(0, 30, 0, 20);
    mainLayout->addWidget(header);

    auto contentWidget = new QWidget();
    contentWidget->setAttribute(Qt::WA_TranslucentBackground);
    auto contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    auto leftButtonsContainer = new QWidget();
    leftButtonsContainer->setAttribute(Qt::WA_TranslucentBackground);
    auto leftButtons = new QVBoxLayout(leftButtonsContainer);
    leftButtons->setSpacing(25);
    leftButtons->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    leftButtons->setContentsMargins(50, 0, 50, 0);

    auto rightButtonsContainer = new QWidget();
    rightButtonsContainer->setAttribute(Qt::WA_TranslucentBackground);
    auto rightButtons = new QVBoxLayout(rightButtonsContainer);
    rightButtons->setSpacing(25);
    rightButtons->setAlignment(Qt::AlignTop | Qt::AlignRight);
    rightButtons->setContentsMargins(50, 0, 50, 0);

    auto addStudentBtn = new QPushButton("Add Student");
    auto removeStudentBtn = new QPushButton("Remove Student");
    auto addClassroomBtn = new QPushButton("Add Classroom");
    auto removeClassroomBtn = new QPushButton("Remove Classroom");
    auto addFacultyBtn = new QPushButton("Add Faculty");
    auto removeFacultyBtn = new QPushButton("Remove Faculty");
    auto resetStudentPasswordBtn = new QPushButton("Reset Student Password");

    auto addTimeslotBtn = new QPushButton("Add Timeslot");
    auto removeTimeslotBtn = new QPushButton("Remove Timeslot");
    auto addCourseBtn = new QPushButton("Add Course");
    auto removeCourseBtn = new QPushButton("Remove Course");
    auto assignCourseScheduleBtn = new QPushButton("Assign Course Schedule");
    auto removeCourseAssignmentBtn = new QPushButton("Remove Course Assignment");
    auto resetFacultyPasswordBtn = new QPushButton("Reset Faculty Password");

    leftButtons->addWidget(addStudentBtn);
    leftButtons->addWidget(removeStudentBtn);
    leftButtons->addWidget(addClassroomBtn);
    leftButtons->addWidget(removeClassroomBtn);
    leftButtons->addWidget(addFacultyBtn);
    leftButtons->addWidget(removeFacultyBtn);
    leftButtons->addWidget(resetStudentPasswordBtn);

    rightButtons->addWidget(addTimeslotBtn);
    rightButtons->addWidget(removeTimeslotBtn);
    rightButtons->addWidget(addCourseBtn);
    rightButtons->addWidget(removeCourseBtn);
    rightButtons->addWidget(assignCourseScheduleBtn);
    rightButtons->addWidget(removeCourseAssignmentBtn);
    rightButtons->addWidget(resetFacultyPasswordBtn);

    auto logoContainer = new QWidget();
    logoContainer->setAttribute(Qt::WA_TranslucentBackground);
    auto logoLayout = new QVBoxLayout(logoContainer);
    logoLayout->setAlignment(Qt::AlignCenter);
    logoLayout->setSpacing(20);
    logoLayout->setContentsMargins(50, 0, 50, 0);

    QPixmap logo("/Users/sufianzahid/Desktop/Qt/OOP/logo.png");
    if (!logo.isNull()) {
        logo = logo.scaled(220, 220, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        auto logoLabel = new QLabel();
        logoLabel->setPixmap(logo);
        logoLabel->setAlignment(Qt::AlignCenter);
        logoLabel->setAttribute(Qt::WA_TranslucentBackground);
        logoLayout->addWidget(logoLabel, 0, Qt::AlignHCenter);
    }

    // Add logout button below the logo
    auto logoutBtn = new QPushButton("Logout");
    logoutBtn->setStyleSheet("QPushButton {"
                             "background-color: rgba(32, 71, 245, 200);"
                             "color: white;"
                             "border: none;"
                             "border-radius: 15px;"
                             "padding: 12px 20px;"
                             "min-width: 220px;"
                             "min-height: 45px;"
                             "font-size: 15px;"
                             "margin: 5px;"
                             "}"
                             "QPushButton:hover {"
                             "background-color: rgba(0, 46, 207, 200);"
                             "font-weight: bold;"
                             "}");
    logoutBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    logoLayout->addWidget(logoutBtn, 0, Qt::AlignHCenter);

    contentLayout->addWidget(leftButtonsContainer, 0, Qt::AlignLeft);
    contentLayout->addWidget(logoContainer, 1, Qt::AlignCenter);
    contentLayout->addWidget(rightButtonsContainer, 0, Qt::AlignRight);

    contentLayout->setStretch(0, 1);
    contentLayout->setStretch(1, 1);
    contentLayout->setStretch(2, 1);

    mainLayout->addWidget(contentWidget);

    QString buttonStyle = "QPushButton {"
                          "background-color: rgba(32, 71, 245, 200);"
                          "color: white;"
                          "border: none;"
                          "border-radius: 15px;"
                          "padding: 12px 20px;"
                          "min-width: 220px;"
                          "min-height: 45px;"
                          "font-size: 15px;"
                          "margin: 5px;"
                          "}"
                          "QPushButton:hover {"
                          "background-color: rgba(0, 46, 207, 200);"
                          "font-weight: bold;"
                          "}";

    QList<QPushButton*> buttons = {addStudentBtn, removeStudentBtn, addFacultyBtn, removeFacultyBtn,
                                    addCourseBtn, removeCourseBtn, addClassroomBtn, removeClassroomBtn,
                                    addTimeslotBtn, removeTimeslotBtn, assignCourseScheduleBtn,
                                    removeCourseAssignmentBtn, resetStudentPasswordBtn, resetFacultyPasswordBtn};

    for (auto btn : buttons) {
        btn->setStyleSheet(buttonStyle);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    connect(addStudentBtn, &QPushButton::clicked, this, &AdminMenu::addStudent);
    connect(removeStudentBtn, &QPushButton::clicked, this, &AdminMenu::removeStudent);
    connect(addFacultyBtn, &QPushButton::clicked, this, &AdminMenu::addFaculty);
    connect(removeFacultyBtn, &QPushButton::clicked, this, &AdminMenu::removeFaculty);
    connect(addCourseBtn, &QPushButton::clicked, this, &AdminMenu::addCourse);
    connect(removeCourseBtn, &QPushButton::clicked, this, &AdminMenu::removeCourse);
    connect(addClassroomBtn, &QPushButton::clicked, this, &AdminMenu::addClassroom);
    connect(removeClassroomBtn, &QPushButton::clicked, this, &AdminMenu::removeClassroom);
    connect(addTimeslotBtn, &QPushButton::clicked, this, &AdminMenu::addTimeslot);
    connect(removeTimeslotBtn, &QPushButton::clicked, this, &AdminMenu::removeTimeslot);
    connect(assignCourseScheduleBtn, &QPushButton::clicked, this, &AdminMenu::assignCourseSchedule);
    connect(removeCourseAssignmentBtn, &QPushButton::clicked, this, &AdminMenu::removeCourseAssignment);
    connect(resetStudentPasswordBtn, &QPushButton::clicked, this, &AdminMenu::resetStudentPassword);
    connect(resetFacultyPasswordBtn, &QPushButton::clicked, this, &AdminMenu::resetFacultyPassword);
    connect(logoutBtn, &QPushButton::clicked, [this]() { this->close(); });
}

void AdminMenu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    QPixmap bg("/Users/sufianzahid/Desktop/Qt/OOP/admin.png");
    if (!bg.isNull()) {
        painter.drawPixmap(rect(), bg);
    }
}


void AdminMenu::addStudent() {
    bool ok;
    QString id = QInputDialog::getText(this, "Add Student", "Student ID:", QLineEdit::Normal, "", &ok);
    if (!ok || id.isEmpty()) return;
    QString fname = QInputDialog::getText(this, "Add Student", "First Name:", QLineEdit::Normal, "", &ok);
    if (!ok || fname.isEmpty()) return;
    QString lname = QInputDialog::getText(this, "Add Student", "Last Name:", QLineEdit::Normal, "", &ok);
    if (!ok || lname.isEmpty()) return;
    QString email = QInputDialog::getText(this, "Add Student", "Email:", QLineEdit::Normal, "", &ok);
    if (!ok || email.isEmpty()) return;
    QString degree = QInputDialog::getText(this, "Add Student", "Degree:", QLineEdit::Normal, "", &ok);
    if (!ok || degree.isEmpty()) return;
    int semester = QInputDialog::getInt(this, "Add Student", "Semester:", 1, 1, 20, 1, &ok);
    if (!ok) return;
    db->addStudent(id.toStdString(), fname.toStdString(), lname.toStdString(), email.toStdString(), degree.toStdString(), semester);
    QMessageBox::information(this, "Add Student", "Student added (default password 'bnu').");
}

void AdminMenu::removeStudent() {
    bool ok;
    QString id = QInputDialog::getText(this, "Remove Student", "Student ID:", QLineEdit::Normal, "", &ok);
    if (!ok || id.isEmpty()) return;
    db->removeStudent(id.toStdString());
    QMessageBox::information(this, "Remove Student", "Student removed.");
}

void AdminMenu::addFaculty() {
    bool ok;
    int faculty_id = QInputDialog::getInt(this, "Add Faculty", "Faculty ID:", 1, 1, 99999, 1, &ok);
    if (!ok) return;
    QString fname = QInputDialog::getText(this, "Add Faculty", "First Name:", QLineEdit::Normal, "", &ok);
    if (!ok || fname.isEmpty()) return;
    QString lname = QInputDialog::getText(this, "Add Faculty", "Last Name:", QLineEdit::Normal, "", &ok);
    if (!ok || lname.isEmpty()) return;
    QString email = QInputDialog::getText(this, "Add Faculty", "Email:", QLineEdit::Normal, "", &ok);
    if (!ok || email.isEmpty()) return;
    QString degree = QInputDialog::getText(this, "Add Faculty", "Degree:", QLineEdit::Normal, "", &ok);
    if (!ok || degree.isEmpty()) return;
    QString qualification = QInputDialog::getText(this, "Add Faculty", "Qualification:", QLineEdit::Normal, "", &ok);
    if (!ok || qualification.isEmpty()) return;
    QString expertise_sub = QInputDialog::getText(this, "Add Faculty", "Expertise Subject:", QLineEdit::Normal, "", &ok);
    if (!ok || expertise_sub.isEmpty()) return;
    QString designation = QInputDialog::getText(this, "Add Faculty", "Designation:", QLineEdit::Normal, "", &ok);
    if (!ok || designation.isEmpty()) return;
    db->addFaculty(faculty_id, fname.toStdString(), lname.toStdString(), email.toStdString(), degree.toStdString(), qualification.toStdString(), expertise_sub.toStdString(), designation.toStdString());
    QMessageBox::information(this, "Add Faculty", "Faculty added.");
}

void AdminMenu::removeFaculty() {
    bool ok;
    int faculty_id = QInputDialog::getInt(this, "Remove Faculty", "Faculty ID:", 1, 1, 99999, 1, &ok);
    if (!ok) return;
    db->removeFaculty(faculty_id);
    QMessageBox::information(this, "Remove Faculty", "Faculty removed.");
}

void AdminMenu::addCourse() {
    bool ok;
    QString code = QInputDialog::getText(this, "Add Course", "Course Code:", QLineEdit::Normal, "", &ok);
    if (!ok || code.isEmpty()) return;
    QString name = QInputDialog::getText(this, "Add Course", "Course Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;
    int credits = QInputDialog::getInt(this, "Add Course", "Credits:", 1, 1, 20, 1, &ok);
    if (!ok) return;
    int semester = QInputDialog::getInt(this, "Add Course", "Semester:", 1, 1, 20, 1, &ok);
    if (!ok) return;
    QString dept = QInputDialog::getText(this, "Add Course", "Department:", QLineEdit::Normal, "", &ok);
    if (!ok || dept.isEmpty()) return;
    int max = QInputDialog::getInt(this, "Add Course", "Max Students:", 1, 1, 500, 1, &ok);
    if (!ok) return;
    QString prereq = QInputDialog::getText(this, "Add Course", "Prerequisites:", QLineEdit::Normal, "", &ok);
    if (!ok) return;
    db->addCourse(code.toStdString(), name.toStdString(), credits, semester, dept.toStdString(), max, prereq.toStdString());
    QMessageBox::information(this, "Add Course", "Course added.");
}

void AdminMenu::removeCourse() {
    bool ok;
    QString code = QInputDialog::getText(this, "Remove Course", "Course Code:", QLineEdit::Normal, "", &ok);
    if (!ok || code.isEmpty()) return;
    db->removeCourse(code.toStdString());
    QMessageBox::information(this, "Remove Course", "Course removed.");
}

void AdminMenu::addClassroom() {
    bool ok;
    QString id = QInputDialog::getText(this, "Add Classroom", "Room ID:", QLineEdit::Normal, "", &ok);
    if (!ok || id.isEmpty()) return;
    QString number = QInputDialog::getText(this, "Add Classroom", "Room Number:", QLineEdit::Normal, "", &ok);
    if (!ok || number.isEmpty()) return;
    QString building = QInputDialog::getText(this, "Add Classroom", "Building:", QLineEdit::Normal, "", &ok);
    if (!ok || building.isEmpty()) return;
    int capacity = QInputDialog::getInt(this, "Add Classroom", "Capacity:", 1, 1, 1000, 1, &ok);
    if (!ok) return;
    QString room_type = QInputDialog::getText(this, "Add Classroom", "Room Type:", QLineEdit::Normal, "", &ok);
    if (!ok || room_type.isEmpty()) return;
    db->addClassroom(id.toStdString(), building.toStdString(), number.toStdString(), capacity, room_type.toStdString());
    QMessageBox::information(this, "Add Classroom", "Classroom added.");
}

void AdminMenu::removeClassroom() {
    bool ok;
    QString id = QInputDialog::getText(this, "Remove Classroom", "Room ID:", QLineEdit::Normal, "", &ok);
    if (!ok || id.isEmpty()) return;
    db->removeClassroom(id.toStdString());
    QMessageBox::information(this, "Remove Classroom", "Classroom removed.");
}

void AdminMenu::addTimeslot() {
    bool ok;
    QString day = QInputDialog::getText(this, "Add Timeslot", "Day of Week:", QLineEdit::Normal, "", &ok);
    if (!ok || day.isEmpty()) return;
    QString start = QInputDialog::getText(this, "Add Timeslot", "Start Time (HH:MM:SS):", QLineEdit::Normal, "", &ok);
    if (!ok || start.isEmpty()) return;
    QString end = QInputDialog::getText(this, "Add Timeslot", "End Time (HH:MM:SS):", QLineEdit::Normal, "", &ok);
    if (!ok || end.isEmpty()) return;
    db->addTimeslot(day.toStdString(), start.toStdString(), end.toStdString());
    QMessageBox::information(this, "Add Timeslot", "Timeslot added.");
}

void AdminMenu::removeTimeslot() {
    bool ok;
    int id = QInputDialog::getInt(this, "Remove Timeslot", "Timeslot ID:", 1, 1, 99999, 1, &ok);
    if (!ok) return;
    db->removeTimeslot(id);
    QMessageBox::information(this, "Remove Timeslot", "Timeslot removed.");
}

void AdminMenu::assignCourseSchedule() {
    auto courses = db->getUnscheduledCourses();
    if (courses.empty()) {
        QMessageBox::information(this, "Assign Course", "All courses already assigned. Remove an assignment to reassign.");
        return;
    }
    QStringList courseNames;
    for (const auto& c : courses)
        courseNames << QString::fromStdString(c.second);

    bool ok;
    QString selectedCourse = QInputDialog::getItem(this, "Assign Course", "Select course:", courseNames, 0, false, &ok);
    if (!ok || selectedCourse.isEmpty()) return;
    int cidx = courseNames.indexOf(selectedCourse);

    auto timeslots = db->getAllTimeslots();
    QStringList timeslotNames;
    for (const auto& ts : timeslots)
        timeslotNames << QString::number(ts.first) + " - " + QString::fromStdString(ts.second);

    QString selectedTimeslot = QInputDialog::getItem(this, "Assign Course", "Select timeslot:", timeslotNames, 0, false, &ok);
    if (!ok || selectedTimeslot.isEmpty()) return;
    int tidx = timeslotNames.indexOf(selectedTimeslot);

    auto availableFaculty = db->getAvailableFaculty(timeslots[tidx].first);
    if (availableFaculty.empty()) {
        QMessageBox::information(this, "Assign Course", "No available faculty for this timeslot.");
        return;
    }
    QStringList facultyNames;
    for (const auto& f : availableFaculty)
        facultyNames << QString::fromStdString(f.second);

    QString selectedFaculty = QInputDialog::getItem(this, "Assign Course", "Select faculty:", facultyNames, 0, false, &ok);
    if (!ok || selectedFaculty.isEmpty()) return;
    int fidx = facultyNames.indexOf(selectedFaculty);

    auto rooms = db->getAvailableRooms(timeslots[tidx].first);
    if (rooms.empty()) {
        QMessageBox::information(this, "Assign Course", "No available rooms for this timeslot.");
        return;
    }
    QStringList roomNames;
    for (const auto& r : rooms)
        roomNames << QString::fromStdString(r.second);

    QString selectedRoom = QInputDialog::getItem(this, "Assign Course", "Select room:", roomNames, 0, false, &ok);
    if (!ok || selectedRoom.isEmpty()) return;
    int ridx = roomNames.indexOf(selectedRoom);

    db->addCourseSchedule(
        courses[cidx].first,
        availableFaculty[fidx].first,
        timeslots[tidx].first,
        rooms[ridx].first);
    QMessageBox::information(this, "Assign Course", "Assignment completed.");
}

void AdminMenu::removeCourseAssignment() {
    auto assignments = db->getAllCourseSchedules();
    if (assignments.empty()) {
        QMessageBox::information(this, "Remove Assignment", "No assigned courses.");
        return;
    }
    QStringList assignmentNames;
    for (const auto& a : assignments)
        assignmentNames << QString::fromStdString(a.course_code + " - " + a.course_name + " | " + a.faculty_name + " | " + a.room + " | " + a.timeslot);

    bool ok;
    QString selectedAssignment = QInputDialog::getItem(this, "Remove Assignment", "Select assignment to remove:", assignmentNames, 0, false, &ok);
    if (!ok || selectedAssignment.isEmpty()) return;
    int idx = assignmentNames.indexOf(selectedAssignment);
    db->removeCourseSchedule(assignments[idx].schedule_id);
    QMessageBox::information(this, "Remove Assignment", "Assignment removed.");
}

void AdminMenu::resetStudentPassword() {
    bool ok;
    QString studentId = QInputDialog::getText(this, "Reset Student Password", "Enter Student ID to reset password:", QLineEdit::Normal, "", &ok);
    if (!ok || studentId.isEmpty()) return;

    if (db->studentExists(studentId.toStdString())) {
        db->resetStudentPassword(studentId.toStdString());
        QMessageBox::information(this, "Reset Password", "Student password reset to 'bnu'.");
    } else {
        QMessageBox::warning(this, "Reset Password", "Student ID not found.");
    }
}

void AdminMenu::resetFacultyPassword() {
    bool ok;
    QString facultyEmail = QInputDialog::getText(this, "Reset Faculty Password", "Enter Faculty Email:", QLineEdit::Normal, "", &ok);
    if (!ok || facultyEmail.isEmpty()) return;

    if (db->facultyExists(facultyEmail.toStdString())) {
        if (db->resetFacultyPassword(facultyEmail.toStdString()))
            QMessageBox::information(this, "Reset Password", "Faculty password reset to 'faculty_scit'.");
        else
            QMessageBox::warning(this, "Reset Password", "Failed to reset password.");
    } else {
        QMessageBox::warning(this, "Reset Password", "Faculty not found.");
    }
}
