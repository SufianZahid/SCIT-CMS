#include "mainwindow.h"
#include "studentmenu.h"
#include "adminmenu.h"
#include "facultymenu.h"
#include "database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>
#include <QEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    db = new Database("127.0.0.1", "root", "Sufian312", "project_db");

    backgroundPixmap = QPixmap("/Users/sufianzahid/Desktop/Qt/OOP/main.jpg");

    setupUI();
}

MainWindow::~MainWindow() {
    delete stuMenu;
    delete adminMenu;
    delete facMenu;
    delete db;
}

void MainWindow::setupUI() {
    auto central = new QWidget(this);
    setCentralWidget(central);

    auto mainLayout = new QVBoxLayout(central);

    mainLayout->addSpacing(100);
    auto logo = new QLabel();
    logo->setAlignment(Qt::AlignCenter);
    logo->setPixmap(QPixmap("/Users/sufianzahid/Desktop/Qt/OOP/logo.png")
                        .scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logo->setStyleSheet("background: transparent;");
    mainLayout->addWidget(logo);

    mainLayout->addSpacing(100);

    auto heading = new QLabel("SCIT MANAGEMENT SYSTEM");
    heading->setAlignment(Qt::AlignCenter);
    QFont font("Georgia", 50, QFont::Bold);
    heading->setFont(font);
    heading->setStyleSheet("color: #2047f5; background: transparent; padding: 14px 36px; letter-spacing: 2px;");
    mainLayout->addWidget(heading);
    mainLayout->addSpacing(100);

    auto btnLayout = new QHBoxLayout();
    mainLayout->addLayout(btnLayout);

    QString btnStyle =
        "QPushButton { background-color: rgba(255, 59, 59, 180); color: white; padding: 14px 34px; border: none; border-radius: 8px; font-size: 18px; font-weight: bold; }"
        "QPushButton:hover { background-color: rgba(213, 0, 0, 200); }";

    studentBtn = new QPushButton("Student Login");
    adminBtn = new QPushButton("Admin Login");
    facultyBtn = new QPushButton("Faculty Login");
    exitBtn = new QPushButton("Exit");

    for (QPushButton *btn : {studentBtn, adminBtn, facultyBtn, exitBtn}) {
        btn->setStyleSheet(btnStyle);
        btn->setMinimumWidth(160);
        btn->installEventFilter(this);
    }

    btnLayout->addStretch();
    btnLayout->addWidget(studentBtn);
    btnLayout->addSpacing(40);
    btnLayout->addWidget(adminBtn);
    btnLayout->addSpacing(40);
    btnLayout->addWidget(facultyBtn);
    btnLayout->addSpacing(40);
    btnLayout->addWidget(exitBtn);
    btnLayout->addStretch();

    mainLayout->addStretch();

    bottomBar = new QLabel(central);
    bottomBar->setFixedSize(120, 10);
    bottomBar->setStyleSheet("background-color: #2047f5; border-radius: 5px;");
    bottomBar->raise();

    ball = new QLabel(central);
    ball->setFixedSize(20, 20);
    ball->setStyleSheet("background-color: #f54291; border-radius: 10px;");
    ball->raise();

    ballAnimation = new QPropertyAnimation(ball, "pos");
    ballAnimation->setDuration(400);
    ballAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(studentBtn, &QPushButton::clicked, this, [=]() {
        bool ok;
        QString id = QInputDialog::getText(this, "Student Login", "Enter Student ID:", QLineEdit::Normal, "", &ok);
        if (!ok || id.isEmpty()) return;

        QString password = QInputDialog::getText(this, "Student Login", "Enter Password:", QLineEdit::Password, "", &ok);
        if (!ok || password.isEmpty()) return;

        if (db->studentExists(id.toStdString()) && db->validateStudentPassword(id.toStdString(), password.toStdString())) {
            delete stuMenu;
            stuMenu = new StudentMenu(db, id, "StudentName", "student@email.com", nullptr);
            stuMenu->setAttribute(Qt::WA_DeleteOnClose);
            stuMenu->show();
        } else {
            QMessageBox::warning(this, "Student Login", "Invalid Student ID or Password.");
        }
    });

    connect(adminBtn, &QPushButton::clicked, this, [=]() {
        bool ok;
        QString pw = QInputDialog::getText(this, "Admin Login", "Enter Admin Password:", QLineEdit::Password, "", &ok);
        if (ok && !pw.isEmpty()) {
            if (db->isAdminPasswordCorrect(pw.toStdString())) {
                delete adminMenu;
                adminMenu = new AdminMenu(db, nullptr);
                adminMenu->setAttribute(Qt::WA_DeleteOnClose);
                adminMenu->show();
            } else {
                QMessageBox::warning(this, "Admin Login", "Invalid password.");
            }
        }
    });

    connect(facultyBtn, &QPushButton::clicked, this, [=]() {
        bool ok;
        QString email = QInputDialog::getText(this, "Faculty Login", "Enter Faculty Email (without @bnu.edu.pk):", QLineEdit::Normal, "", &ok);
        if (!ok || email.isEmpty()) return;
        email += "@bnu.edu.pk";
        QString password = QInputDialog::getText(this, "Faculty Login", "Enter Password:", QLineEdit::Password, "", &ok);
        if (!ok || password.isEmpty()) return;
        if (db->facultyExists(email.toStdString()) && db->validateFacultyPassword(email.toStdString(), password.toStdString())) {
            int facultyId = QString::fromStdString(db->getFacultyId(email.toStdString())).toInt();
            QString facultyName = QString::fromStdString(db->getFacultyName(email.toStdString()));
            delete facMenu;
            facMenu = new FacultyMenu(db, QString::number(facultyId), facultyName, email, nullptr);
            facMenu->setAttribute(Qt::WA_DeleteOnClose);
            facMenu->show();
        } else {
            QMessageBox::warning(this, "Faculty Login", "Invalid Faculty Email or Password.");
        }
    });

    connect(exitBtn, &QPushButton::clicked, this, &QApplication::quit);
}


void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (!backgroundPixmap.isNull()) {
        backgroundPixmap = QPixmap("/Users/sufianzahid/Desktop/Qt/OOP/main.jpg")
        .scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }

    int barX = (width() - bottomBar->width()) / 2;
    int barY = height() - bottomBar->height() - 30;

    bottomBar->move(barX, barY);
    barPos = QPoint(barX + bottomBar->width() / 2 - ball->width() / 2, barY - ball->height());

    if (!hovering)
        ball->move(barPos);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Enter) {
        if (obj == studentBtn || obj == adminBtn || obj == facultyBtn || obj == exitBtn) {
            auto btn = qobject_cast<QWidget *>(obj);
            if (btn) {
                hovering = true;
                animateBallTo(btn);
            }
        }
    } else if (event->type() == QEvent::Leave) {
        hovering = false;
        resetBallToBar();
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::animateBallTo(QWidget *target) {
    QPoint global = target->mapTo(this, QPoint(0, 0));
    QPoint newPos(global.x() + target->width() / 2 - ball->width() / 2,
                  global.y() + target->height() + 8);

    ballAnimation->stop();
    ballAnimation->setEndValue(newPos);
    ballAnimation->start();
}

void MainWindow::resetBallToBar() {
    ballAnimation->stop();
    ballAnimation->setEndValue(barPos);
    ballAnimation->start();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    if (!backgroundPixmap.isNull()) {
        painter.drawPixmap(rect(), backgroundPixmap);
    }
    QMainWindow::paintEvent(event);
}
