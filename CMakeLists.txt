cmake_minimum_required(VERSION 3.16)

project(OOP VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia MultimediaWidgets)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_library(MYSQLCPPCONN_LIB
    NAMES mysqlcppconn8 mysqlcppconnx
    PATHS /usr/local/lib /usr/lib
    REQUIRED
)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    studentmenu.cpp
    studentmenu.h
    adminmenu.cpp
    adminmenu.h
    facultymenu.cpp
    facultymenu.h
    database.cpp
    database.h
)

qt_add_executable(OOP
    ${PROJECT_SOURCES}
)

target_link_libraries(OOP PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    ${MYSQLCPPCONN_LIB}
)

set_target_properties(OOP PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
