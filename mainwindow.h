#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPixmap>

class StudentMenu;
class AdminMenu;
class FacultyMenu;
class Database;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void resized();

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void setupUI();
    void animateBallTo(QWidget *target);
    void resetBallToBar();

    QPushButton *studentBtn;
    QPushButton *adminBtn;
    QPushButton *facultyBtn;
    QPushButton *exitBtn;

    QLabel *ball;
    QLabel *bottomBar;
    QPropertyAnimation *ballAnimation;

    QPoint barPos;
    bool hovering = false;

    StudentMenu *stuMenu = nullptr;
    AdminMenu *adminMenu = nullptr;
    FacultyMenu *facMenu = nullptr;
    Database *db = nullptr;

    QPixmap backgroundPixmap;
};

#endif
