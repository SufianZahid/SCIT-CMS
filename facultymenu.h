#pragma once
#include <QWidget>
#include <QPixmap>
#include "database.h"

class QLabel;
class QPushButton;

class FacultyMenu : public QWidget
{
    Q_OBJECT

public:
    FacultyMenu(Database *db, QString facultyId, QString facultyName, QString facultyEmail, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Database *db;
    QString facultyId;
    QString facultyName;
    QString facultyEmail;

    QLabel *titleLabel;
    QPushButton *viewEnrolledStudentsBtn;
    QPushButton *viewTimetableBtn;
    QPushButton *exportTimetableBtn;
    QPushButton *manageMarksBtn;
    QPushButton *viewTotalEnrolledBtn;
    QPushButton *changePasswordBtn;
    QPushButton *logoutBtn;

    QPixmap backgroundImage;

private slots:
    void viewEnrolledStudents();
    void viewTimetable();
    void exportTimetable();
    void manageMarks();
    void viewTotalEnrolledStudents();
    void changePassword();
    void logout();
};
