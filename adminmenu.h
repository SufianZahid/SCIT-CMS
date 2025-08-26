#pragma once
#include <QWidget>
#include "database.h"

class AdminMenu : public QWidget
{
    Q_OBJECT
    Database *db;

public:
    AdminMenu(Database *db, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void addStudent();
    void removeStudent();
    void addFaculty();
    void removeFaculty();
    void addCourse();
    void removeCourse();
    void addClassroom();
    void removeClassroom();
    void addTimeslot();
    void removeTimeslot();
    void assignCourseSchedule();
    void removeCourseAssignment();
    void resetStudentPassword();
    void resetFacultyPassword();
};
