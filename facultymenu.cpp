#include "facultymenu.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QPainter>
#include <QFont>
#include <QSet>
#include <QMap>
#include <fstream>
#include <QSpacerItem>

FacultyMenu::FacultyMenu(Database *db, QString facultyId, QString facultyName, QString facultyEmail, QWidget *parent)
    : QWidget(parent),
    db(db),
    facultyId(std::move(facultyId)),
    facultyName(std::move(facultyName)),
    facultyEmail(std::move(facultyEmail))
{
    setWindowTitle("Faculty Menu");
    setMinimumSize(1000, 800);

    backgroundImage.load("/Users/sufianzahid/Desktop/Qt/OOP/faculty.jpeg");

    titleLabel = new QLabel("FACULTY MENU", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'Georgia';
            font-size: 48px;
            font-weight: bold;
            color: white;
            background: transparent;
        }
    )");

    viewEnrolledStudentsBtn = new QPushButton("View Enrolled Students");
    viewTimetableBtn = new QPushButton("View Timetable");
    exportTimetableBtn = new QPushButton("Export Timetable");
    manageMarksBtn = new QPushButton("Manage Marks");
    viewTotalEnrolledBtn = new QPushButton("View Total Enrolled");
    changePasswordBtn = new QPushButton("Change Password");
    logoutBtn = new QPushButton("Logout");

    QList<QPushButton*> buttons = {
        viewEnrolledStudentsBtn, viewTimetableBtn, exportTimetableBtn,
        manageMarksBtn, viewTotalEnrolledBtn, changePasswordBtn, logoutBtn
    };

    QString btnStyle = R"(
        QPushButton {
            background-color: rgba(32, 71, 245, 200);
            color: white;
            border: none;
            border-radius: 15px;
            min-width: 340px;
            min-height: 48px;
            font-size: 15px;
            font-family: 'Arial Black', 'Arial', sans-serif;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: rgba(0, 46, 207, 200);
            font-weight: bold;
        }
    )";
    for (QPushButton* btn : buttons) {
        btn->setStyleSheet(btnStyle);
        btn->setFixedSize(340, 54);
        btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 80, 20, 40); 
    mainLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    mainLayout->addSpacing(45);

    QVBoxLayout* btnLayout = new QVBoxLayout();
    btnLayout->setAlignment(Qt::AlignHCenter);

    int numButtons = buttons.size();
    int normalGap = 22;
    int extraGap = 80;

    for (int i = 0; i < numButtons; ++i) {
        btnLayout->addWidget(buttons[i], 0, Qt::AlignHCenter);
        if (i == numButtons - 2) 
            btnLayout->addSpacing(extraGap);
        else if (i != numButtons - 1)
            btnLayout->addSpacing(normalGap);
    }

    mainLayout->addStretch(1);
    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch(2);

    connect(viewEnrolledStudentsBtn, &QPushButton::clicked, this, &FacultyMenu::viewEnrolledStudents);
    connect(viewTimetableBtn, &QPushButton::clicked, this, &FacultyMenu::viewTimetable);
    connect(exportTimetableBtn, &QPushButton::clicked, this, &FacultyMenu::exportTimetable);
    connect(manageMarksBtn, &QPushButton::clicked, this, &FacultyMenu::manageMarks);
    connect(viewTotalEnrolledBtn, &QPushButton::clicked, this, &FacultyMenu::viewTotalEnrolledStudents);
    connect(changePasswordBtn, &QPushButton::clicked, this, &FacultyMenu::changePassword);
    connect(logoutBtn, &QPushButton::clicked, this, &FacultyMenu::logout);
}

void FacultyMenu::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (!backgroundImage.isNull())
        painter.drawPixmap(rect(), backgroundImage.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    else
        painter.fillRect(rect(), QColor(245,245,255));
    QWidget::paintEvent(event);
}


void FacultyMenu::viewEnrolledStudents() {
    auto courses = db->getFacultyCourses(facultyId.toInt());
    if (courses.empty()) {
        QMessageBox::information(this, "Enrolled Students", "You are not assigned to any courses.");
        return;
    }
    bool ok;
    QStringList items;
    for (const auto& c : courses) items << QString::fromStdString(c);
    QString selected = QInputDialog::getItem(this, "Enrolled Students", "Select course:", items, 0, false, &ok);
    if (!ok || selected.isEmpty()) return;
    std::string course_code = selected.toStdString().substr(0, selected.indexOf(" - "));
    auto students = db->getEnrolledStudentsInCourse(course_code);
    if (students.empty()) {
        QMessageBox::information(this, "Enrolled Students", "No students enrolled in this course.");
        return;
    }
    QString table = "ID | First Name | Last Name | Email | Sem | Degree\n";
    for (const auto& s : students)
        table += QString("%1 | %2 | %3 | %4 | %5 | %6\n")
                     .arg(QString::fromStdString(s.student_id))
                     .arg(QString::fromStdString(s.first_name))
                     .arg(QString::fromStdString(s.last_name))
                     .arg(QString::fromStdString(s.email))
                     .arg(QString::number(s.semester))
                     .arg(QString::fromStdString(s.degree));
    QMessageBox::information(this, "Enrolled Students", table);
}

void FacultyMenu::viewTimetable() {
    auto tt = db->getFacultyTimetable(facultyId.toInt());
    if (tt.empty()) {
        QMessageBox::information(this, "Timetable", "No classes scheduled.");
        return;
    }
    QString table = "Course | Name | Day | Start-End | Room | Bldg\n";
    for (const auto& t : tt) {
        table += QString("%1 | %2 | %3 | %4-%5 | %6 %7\n")
        .arg(QString::fromStdString(t.course_code))
            .arg(QString::fromStdString(t.course_name))
            .arg(QString::fromStdString(t.day))
            .arg(QString::fromStdString(t.start_time))
            .arg(QString::fromStdString(t.end_time))
            .arg(QString::fromStdString(t.room_number))
            .arg(QString::fromStdString(t.building));
    }
    QMessageBox::information(this, "Timetable", table);
}

void FacultyMenu::exportTimetable() {
    auto tt = db->getFacultyTimetable(facultyId.toInt());
    if (tt.empty()) {
        QMessageBox::information(this, "Export Timetable", "No classes to export.");
        return;
    }
    QString filename = QFileDialog::getSaveFileName(this, "Export Timetable", "faculty_" + facultyId + "_timetable.csv", "CSV files (*.csv)");
    if (filename.isEmpty()) return;
    std::ofstream out(filename.toStdString());
    out << "Course,Name,Day,Start,End,Room,Bldg\n";
    for (const auto& t : tt) {
        out << t.course_code << "," << t.course_name << "," << t.day << "," << t.start_time << ","
            << t.end_time << "," << t.room_number << "," << t.building << "\n";
    }
    out.close();
    QMessageBox::information(this, "Export Timetable", "Timetable exported to " + filename);
}

void FacultyMenu::manageMarks() {
    auto courses = db->getFacultyCourses(facultyId.toInt());
    if (courses.empty()) {
        QMessageBox::information(this, "Marks", "You are not assigned to any courses.");
        return;
    }
    bool ok;
    QStringList items;
    for (const auto& c : courses) items << QString::fromStdString(c);
    QString selected = QInputDialog::getItem(this, "Marks", "Select course:", items, 0, false, &ok);
    if (!ok || selected.isEmpty()) return;
    std::string course_code = selected.toStdString().substr(0, selected.indexOf(" - "));
    QStringList actionList = {"Add Marks", "Edit Marks"};
    QString action = QInputDialog::getItem(this, "Marks", "Action:", actionList, 0, false, &ok);
    if (!ok || action.isEmpty()) return;

    if (action == "Add Marks") {
        QString assignment = QInputDialog::getText(this, "Add Marks", "Assignment Name (e.g., Assignment1, Midterm, Final):", QLineEdit::Normal, "", &ok);
        if (!ok || assignment.isEmpty()) return;
        int total_marks = QInputDialog::getInt(this, "Add Marks", "Total Marks:", 100, 1, 1000, 1, &ok);
        if (!ok) return;
        auto students = db->getEnrolledStudentsInCourse(course_code);
        if (students.empty()) {
            QMessageBox::information(this, "Add Marks", "No students enrolled in this course.");
            return;
        }
        auto existing = db->getStudentMarksForAssignment(course_code, assignment.toStdString());
        QSet<QString> marked;
        for (const auto& mark : existing) marked.insert(QString::fromStdString(mark.first));
        QList<QPair<QString, QString>> studentList; 
        for (const auto& student : students) {
            if (marked.contains(QString::fromStdString(student.student_id))) continue;
            studentList.append(qMakePair(QString::fromStdString(student.student_id), QString::fromStdString(student.first_name + " " + student.last_name)));
        }

        while (!studentList.isEmpty()) {
            QStringList nameList;
            for (const auto& pair : studentList)
                nameList << pair.second;
            QString chosenName = QInputDialog::getItem(this, "Add Marks", "Select student to add marks:", nameList, 0, false, &ok);
            if (!ok || chosenName.isEmpty()) break;
            int idx = nameList.indexOf(chosenName);
            QString studentId = studentList[idx].first;

            int obtained = QInputDialog::getInt(this, "Add Marks",
                                                QString("Obtained marks for %1:").arg(chosenName), 0, 0, total_marks, 1, &ok);
            if (!ok) continue;
            db->addMarks(course_code, studentId.toStdString(), assignment.toStdString(), total_marks, obtained);
            studentList.removeAt(idx);
        }
        QMessageBox::information(this, "Add Marks", "Marks entry complete.");
    } else if (action == "Edit Marks") {
        auto assignments = db->getAssignmentsForCourse(course_code);
        if (assignments.empty()) {
            QMessageBox::information(this, "Edit Marks", "No assignments for this course.");
            return;
        }
        QStringList assgnList;
        for (const auto& a : assignments) assgnList << QString::fromStdString(a);
        QString assignment = QInputDialog::getItem(this, "Edit Marks", "Select assignment:", assgnList, 0, false, &ok);
        if (!ok || assignment.isEmpty()) return;

        auto marks = db->getStudentMarksForAssignment(course_code, assignment.toStdString());
        if (marks.empty()) {
            QMessageBox::information(this, "Edit Marks", "No marks for this assignment.");
            return;
        }
        auto all_students = db->getEnrolledStudentsInCourse(course_code);
        QMap<QString, QString> idToName;
        for (const auto& s : all_students)
            idToName[QString::fromStdString(s.student_id)] = QString::fromStdString(s.first_name + " " + s.last_name);

        for (const auto& mark : marks) {
            int newMark = QInputDialog::getInt(this, "Edit Marks",
                                               QString("Current: %1/%2\nStudent: %3\nEnter new obtained marks:")
                                                   .arg(mark.second.second).arg(mark.second.first)
                                                   .arg(idToName[QString::fromStdString(mark.first)]), mark.second.second, 0, mark.second.first, 1, &ok);
            if (!ok) continue;
            db->updateMarks(course_code, mark.first, assignment.toStdString(), newMark);
        }
        QMessageBox::information(this, "Edit Marks", "Marks updated.");
    }
}

void FacultyMenu::viewTotalEnrolledStudents() {
    auto courses = db->getFacultyCourses(facultyId.toInt());
    if (courses.empty()) {
        QMessageBox::information(this, "Total Enrolled", "You are not assigned to any courses.");
        return;
    }
    QString table = "Course | Total Students\n";
    for (const auto& course : courses) {
        std::string code = course.substr(0, course.find(" - "));
        int total = db->getTotalEnrolledStudents(code);
        table += QString("%1 | %2\n").arg(QString::fromStdString(code)).arg(total);
    }
    QMessageBox::information(this, "Total Enrolled", table);
}

void FacultyMenu::changePassword() {
    bool ok;
    QString oldPwd = QInputDialog::getText(this, "Change Password", "Enter current password:", QLineEdit::Password, "", &ok);
    if (!ok || oldPwd.isEmpty()) return;
    if (!db->validateFacultyPassword(facultyEmail.toStdString(), oldPwd.toStdString())) {
        QMessageBox::warning(this, "Change Password", "Current password incorrect.");
        return;
    }
    QString newPwd = QInputDialog::getText(this, "Change Password", "Enter new password:", QLineEdit::Password, "", &ok);
    if (!ok || newPwd.isEmpty()) return;
    if (db->changeFacultyPassword(facultyEmail.toStdString(), newPwd.toStdString()))
        QMessageBox::information(this, "Change Password", "Password changed successfully.");
    else
        QMessageBox::warning(this, "Change Password", "Failed to change password.");
}

void FacultyMenu::logout() {
    QMessageBox::information(this, "Logout", "You have been logged out.");
    close();
}
