#pragma once
#include <QWidget>
#include <QVector>
#include <QFont>
#include <QTimer>
#include <QPixmap>
#include "database.h"

class QLabel;
class QPushButton;

class StudentMenu : public QWidget
{
    Q_OBJECT

public:
    StudentMenu(Database *db, QString studentId, QString studentName, QString studentEmail, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Database *db;
    QString studentId;
    QString studentName;
    QString studentEmail;
    QPixmap backgroundImage;

    QLabel *titleLabel;
    QPushButton *addCourseBtn;
    QPushButton *dropCourseBtn;
    QPushButton *viewTimetableBtn;
    QPushButton *viewTeachersBtn;
    QPushButton *viewClassroomDetailsBtn;
    QPushButton *exportTimetableBtn;
    QPushButton *changePasswordBtn;
    QPushButton *viewMarksBtn;
    QPushButton *logoutBtn;

private slots:
    void addCourse();
    void dropCourse();
    void viewTimetable();
    void viewTeachers();
    void viewClassroomDetails();
    void exportTimetable();
    void changePassword();
    void viewMarks();
    void logout();
};
