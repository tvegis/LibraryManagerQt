cmake_minimum_required(VERSION 3.16)

project(LibraryManagerQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 兼容 Qt6 与 Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_standard_project_setup()
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
endif()

set(PROJECT_SOURCES
    src/main.cpp
    src/AppConfig.h
    src/AppConfig.cpp
    src/db/DatabaseManager.h
    src/db/DatabaseManager.cpp

    # models
    src/models/Book.h
    src/models/Reader.h
    src/models/Admin.h
    src/models/Loan.h
    src/models/Reservation.h
    src/models/Message.h

    # repos
    src/repos/BookRepository.h
    src/repos/BookRepository.cpp
    src/repos/ReaderRepository.h
    src/repos/ReaderRepository.cpp
    src/repos/AdminRepository.h
    src/repos/AdminRepository.cpp
    src/repos/LoanRepository.h
    src/repos/LoanRepository.cpp
    src/repos/ReservationRepository.h
    src/repos/ReservationRepository.cpp
    src/repos/MessageRepository.h
    src/repos/MessageRepository.cpp

    # services
    src/services/AuthService.h
    src/services/AuthService.cpp
    src/services/LibraryService.h
    src/services/LibraryService.cpp

    # ui
    src/ui/LoginWindow.h
    src/ui/LoginWindow.cpp
    src/ui/LoginWindow.ui
    src/ui/AdminMainWindow.h
    src/ui/AdminMainWindow.cpp
    src/ui/AdminMainWindow.ui
    src/ui/ReaderMainWindow.h
    src/ui/ReaderMainWindow.cpp
    src/ui/ReaderMainWindow.ui
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Sql
)
