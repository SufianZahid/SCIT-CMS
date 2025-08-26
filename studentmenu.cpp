#include "studentmenu.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QPainter>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QRandomGenerator>
#include <QFontMetrics>
#include <QTimer>
#include <fstream>

StudentMenu::StudentMenu(Database *db, QString studentId, QString studentName, QString studentEmail, QWidget *parent)
    : QWidget(parent),
    db(db),
    studentId(std::move(studentId)),
    studentName(std::move(studentName)),
    studentEmail(std::move(studentEmail))
{
    setWindowTitle("Student Menu");
    setMinimumSize(1000, 700);

    backgroundImage.load("/Users/sufianzahid/Desktop/Qt/OOP/student.jpeg");

    if (backgroundImage.isNull()) {
        backgroundImage = QPixmap(size());
        backgroundImage.fill(QColor(20, 25, 45));
    }

    titleLabel = new QLabel("STUDENT MENU", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
        QLabel {
            font-family: 'Georgia';
            font-size: 50px;
            font-weight: bold;
            color: white;
            background: transparent;
        }
    )");

    addCourseBtn = new QPushButton("Add Course");
    dropCourseBtn = new QPushButton("Drop Course");
    viewTimetableBtn = new QPushButton("View Timetable");
    viewTeachersBtn = new QPushButton("View Teachers");
    viewClassroomDetailsBtn = new QPushButton("View Classroom");
    exportTimetableBtn = new QPushButton("Export Timetable");
    changePasswordBtn = new QPushButton("Change Password");
    viewMarksBtn = new QPushButton("View Marks");
    logoutBtn = new QPushButton("Logout");

    QList<QPushButton*> buttons = {
        addCourseBtn, dropCourseBtn, viewTimetableBtn, viewTeachersBtn,
        viewClassroomDetailsBtn, exportTimetableBtn, changePasswordBtn, viewMarksBtn, logoutBtn
    };

    for (QPushButton* btn : buttons) {
        btn->setMinimumSize(150, 150);
        btn->setMaximumSize(150, 150);
        btn->setStyleSheet(R"(
            QPushButton {
                border-radius: 75px;
                background-color: rgba(32, 71, 245, 200);
                color: white;
                font-size: 15px;
                font-family: 'Arial Black', 'Arial', sans-serif;
            }
            QPushButton:hover {
                background-color: rgba(0, 46, 207, 200);
            }
        )");
    }

    QGridLayout* grid = new QGridLayout();
    grid->setHorizontalSpacing(40);
    grid->setVerticalSpacing(60);

    grid->addWidget(addCourseBtn,             0, 0, Qt::AlignCenter);
    grid->addWidget(dropCourseBtn,            0, 2, Qt::AlignCenter);
    grid->addWidget(viewTimetableBtn,         0, 4, Qt::AlignCenter);
    grid->addWidget(viewTeachersBtn,          0, 6, Qt::AlignCenter);
    grid->addWidget(viewClassroomDetailsBtn,  1, 1, Qt::AlignCenter);
    grid->addWidget(exportTimetableBtn,       1, 3, Qt::AlignCenter);
    grid->addWidget(changePasswordBtn,        1, 5, Qt::AlignCenter);
    grid->addWidget(viewMarksBtn,             2, 2, Qt::AlignCenter);
    grid->addWidget(logoutBtn,                2, 4, Qt::AlignCenter);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->addSpacing(60);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(60);
    mainLayout->addLayout(grid);
    mainLayout->addStretch();

    connect(addCourseBtn, &QPushButton::clicked, this, &StudentMenu::addCourse);
    connect(dropCourseBtn, &QPushButton::clicked, this, &StudentMenu::dropCourse);
    connect(viewTimetableBtn, &QPushButton::clicked, this, &StudentMenu::viewTimetable);
    connect(viewTeachersBtn, &QPushButton::clicked, this, &StudentMenu::viewTeachers);
    connect(viewClassroomDetailsBtn, &QPushButton::clicked, this, &StudentMenu::viewClassroomDetails);
    connect(exportTimetableBtn, &QPushButton::clicked, this, &StudentMenu::exportTimetable);
    connect(changePasswordBtn, &QPushButton::clicked, this, &StudentMenu::changePassword);
    connect(viewMarksBtn, &QPushButton::clicked, this, &StudentMenu::viewMarks);
    connect(logoutBtn, &QPushButton::clicked, this, &StudentMenu::logout);
}

void StudentMenu::resizeEvent(QResizeEvent *event)
{
    if (!backgroundImage.isNull()) {
        QPixmap scaled = backgroundImage.scaled(
            event->size(),
            Qt::KeepAspectRatioByExpanding,
            Qt::SmoothTransformation
            );
        backgroundImage = scaled;
    }
    QWidget::resizeEvent(event);
}

void StudentMenu::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (!backgroundImage.isNull()) {
        int x = (width() - backgroundImage.width()) / 2;
        int y = (height() - backgroundImage.height()) / 2;
        painter.drawPixmap(x, y, backgroundImage);
    }
    else {
        painter.fillRect(rect(), QColor(20, 25, 45));
    }

    QWidget::paintEvent(event);
}

void StudentMenu::addCourse() {
    int semester = db->getStudentSemester(studentId.toStdString());
    std::string degree = db->getStudentDegree(studentId.toStdString());
    auto courses = db->getAvailableScheduledCourses(semester, degree);

    if (courses.empty()) {
        QMessageBox::information(this, "Add Course", "No scheduled courses for your degree/semester.");
        return;
    }

    QStringList items;
    for (const auto& sc : courses)
        items << QString("%1 - %2 | %3 | %4 %5-%6")
                     .arg(QString::fromStdString(sc.course_code))
                     .arg(QString::fromStdString(sc.course_name))
                     .arg(QString::fromStdString(sc.faculty_name))
                     .arg(QString::fromStdString(sc.day))
                     .arg(QString::fromStdString(sc.start_time))
                     .arg(QString::fromStdString(sc.end_time));

    bool ok;
    QString selected = QInputDialog::getItem(this, "Add Course", "Select a course:", items, 0, false, &ok);
    if (!ok || selected.isEmpty()) return;

    int idx = items.indexOf(selected);
    const auto& sc = courses[idx];

    QString info = QString("Class Timing:\nDay: %1\nStart: %2\nEnd: %3")
                       .arg(QString::fromStdString(sc.day))
                       .arg(QString::fromStdString(sc.start_time))
                       .arg(QString::fromStdString(sc.end_time));
    QMessageBox::information(this, "Class Timing", info);

    if (db->isAlreadyEnrolled(studentId.toStdString(), sc.schedule_id)) {
        QMessageBox::information(this, "Add Course", "Already enrolled in this course.");
        return;
    }
    if (db->hasClash(studentId.toStdString(), sc.timeslot_id)) {
        QMessageBox::information(this, "Add Course", "Course timeslot clashes with your existing courses.");
        return;
    }
    if (db->addEnrollment(studentId.toStdString(), sc.schedule_id))
        QMessageBox::information(this, "Add Course", "Enrolled successfully.");
    else
        QMessageBox::warning(this, "Add Course", "Course full or error occurred.");
}

void StudentMenu::dropCourse() {
    auto enrolled = db->getEnrolledCourses(studentId.toStdString());
    if (enrolled.empty()) {
        QMessageBox::information(this, "Drop Course", "No enrolled courses.");
        return;
    }
    QStringList items;
    for (const auto& sc : enrolled)
        items << QString::fromStdString(sc.course_code + " - " + sc.course_name + " | " + sc.faculty_name);

    bool ok;
    QString selected = QInputDialog::getItem(this, "Drop Course", "Select a course to drop:", items, 0, false, &ok);
    if (!ok || selected.isEmpty()) return;

    int idx = items.indexOf(selected);
    const auto& sc = enrolled[idx];
    if (db->dropEnrollment(studentId.toStdString(), sc.schedule_id))
        QMessageBox::information(this, "Drop Course", "Dropped successfully.");
    else
        QMessageBox::warning(this, "Drop Course", "Error or not enrolled.");
}

void StudentMenu::viewTimetable() {
    auto tt = db->getEnrolledCourses(studentId.toStdString());
    if (tt.empty()) {
        QMessageBox::information(this, "Timetable", "No enrolled courses.");
        return;
    }
    QString table = "Course | Name | Day | Start-End | Room | Bldg | Teacher\n"; 
    for (const auto& t : tt) {
        table += QString("%1 | %2 | %3 | %4-%5 | %6 %7 | %8\n")
        .arg(QString::fromStdString(t.course_code))
            .arg(QString::fromStdString(t.course_name))
            .arg(QString::fromStdString(t.day))
            .arg(QString::fromStdString(t.start_time))
            .arg(QString::fromStdString(t.end_time))
            .arg(QString::fromStdString(t.room_number))
            .arg(QString::fromStdString(t.building))
            .arg(QString::fromStdString(t.faculty_name));
    }
    QMessageBox::information(this, "Timetable", table);
}

void StudentMenu::viewTeachers() {
    auto tt = db->getEnrolledCourses(studentId.toStdString());
    if (tt.empty()) {
        QMessageBox::information(this, "Teachers", "No enrolled courses.");
        return;
    }
    QStringList teachers;
    for (const auto& t : tt) {
        if (!teachers.contains(QString::fromStdString(t.faculty_name)))
            teachers << QString::fromStdString(t.faculty_name);
    }
    QMessageBox::information(this, "Teachers", "Your Teachers:\n" + teachers.join("\n"));
}

void StudentMenu::viewClassroomDetails() {
    auto tt = db->getEnrolledCourses(studentId.toStdString());
    if (tt.empty()) {
        QMessageBox::information(this, "Classrooms", "No enrolled courses.");
        return;
    }
    QStringList rooms;
    for (const auto& t : tt) {
        QString room = QString::fromStdString("Room " + t.room_number + " in " + t.building);
        if (!rooms.contains(room)) rooms << room;
    }
    QMessageBox::information(this, "Classrooms", "Your Classrooms:\n" + rooms.join("\n"));
}

void StudentMenu::exportTimetable() {
    auto tt = db->getEnrolledCourses(studentId.toStdString());
    if (tt.empty()) {
        QMessageBox::information(this, "Export Timetable", "No enrolled courses.");
        return;
    }
    QString filename = QFileDialog::getSaveFileName(this, "Export Timetable", studentId + "_timetable.csv", "CSV files (*.csv)");
    if (filename.isEmpty()) return;
    std::ofstream out(filename.toStdString());
    out << "Course,Name,Day,Start,End,Room,Bldg,Teacher\n";
    for (const auto& t : tt) {
        out << t.course_code << "," << t.course_name << "," << t.day << "," << t.start_time << ","
            << t.end_time << "," << t.room_number << "," << t.building << "," << t.faculty_name << "\n";
    }
    out.close();
    QMessageBox::information(this, "Export Timetable", "Timetable exported to " + filename);
}

void StudentMenu::changePassword() {
    bool ok;
    QString oldPwd = QInputDialog::getText(this, "Change Password", "Enter current password:", QLineEdit::Password, "", &ok);
    if (!ok || oldPwd.isEmpty()) return;

    if (!db->validateStudentPassword(studentId.toStdString(), oldPwd.toStdString())) {
        QMessageBox::warning(this, "Change Password", "Current password incorrect.");
        return;
    }

    QString newPwd = QInputDialog::getText(this, "Change Password", "Enter new password:", QLineEdit::Password, "", &ok);
    if (!ok || newPwd.isEmpty()) return;

    QString confirmPwd = QInputDialog::getText(this, "Change Password", "Confirm new password:", QLineEdit::Password, "", &ok);
    if (!ok || confirmPwd.isEmpty()) return;

    if (newPwd != confirmPwd) {
        QMessageBox::warning(this, "Change Password", "New passwords do not match.");
        return;
    }

    if (db->changeStudentPassword(studentId.toStdString(), newPwd.toStdString())) {
        QMessageBox::information(this, "Change Password", "Password changed successfully.");
    } else {
        QMessageBox::warning(this, "Change Password", "Failed to change password.");
    }
}

void StudentMenu::viewMarks() {
    auto courses = db->getStudentCourses(studentId.toStdString());
    if (courses.empty()) {
        QMessageBox::information(this, "Marks", "You are not enrolled in any courses.");
        return;
    }
    bool ok;
    QStringList items;
    for (const auto& c : courses) items << QString::fromStdString(c);
    QString selected = QInputDialog::getItem(this, "Marks", "Select course to view marks:", items, 0, false, &ok);
    if (!ok || selected.isEmpty()) return;
    std::string course_code = selected.toStdString().substr(0, selected.indexOf(" - "));
    auto marks = db->getStudentMarks(studentId.toStdString(), course_code);
    if (marks.empty()) {
        QMessageBox::information(this, "Marks", "No marks available for the selected course.");
        return;
    }

    QString table = "<table border='1' cellspacing='0' cellpadding='3'><tr><th>Assignment</th><th>Marks</th><th>Percentage</th></tr>";
    for (const auto& mark : marks) {
        double percent = (mark.total_marks > 0) ? (100.0 * mark.obtained_marks / mark.total_marks) : 0.0;
        QString color;
        if (percent >= 80) color = "green";
        else if (percent >= 50) color = "orange";
        else color = "red";
        table += QString("<tr><td>%1</td><td>%2/%3</td><td><span style='color:%4;'>%5%</span></td></tr>")
                     .arg(QString::fromStdString(mark.assignment_name))
                     .arg(mark.obtained_marks)
                     .arg(mark.total_marks)
                     .arg(color)
                     .arg(QString::number(percent, 'f', 2));
    }
    table += "</table>";

    QDialog dlg(this);
    dlg.setWindowTitle("Marks");
    QVBoxLayout layout(&dlg);
    QLabel label;
    label.setTextFormat(Qt::RichText);
    label.setText(table);
    layout.addWidget(&label);
    QPushButton okBtn("OK");
    QObject::connect(&okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    layout.addWidget(&okBtn);
    dlg.exec();
}
void StudentMenu::logout()
{
    QMessageBox::information(this, "Logout", "You have been logged out.");
    close();
}
