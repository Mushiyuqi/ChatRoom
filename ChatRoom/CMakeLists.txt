cmake_minimum_required(VERSION 3.27)

project(ChatRoom VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)

# 启用MOC/RCC/UIC自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
# 包含构建目录（解决ui_*.h路径问题）
set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_standard_project_setup()

qt_add_executable(ChatRoom
    timerbutton.h timerbutton.cpp
    mainwindow.h mainwindow.cpp
    mainwindow.ui
    main.cpp
    resource.qrc
    logindialog.h logindialog.cpp logindialog.ui
    registerdialog.h registerdialog.cpp registerdialog.ui
    global.h
    global.cpp
    httpmanager.h httpmanager.cpp
    config.ini
    clickedlabel.h clickedlabel.cpp
    resetdialog.h resetdialog.cpp resetdialog.ui
)



target_link_libraries(ChatRoom PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    # 添加crypto++库的依赖
    /usr/lib/libcryptopp.so
)

set_target_properties(ChatRoom PROPERTIES
    AUTORCC TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# 将config.ini打包到构建目录方便调试
configure_file(
    "${CMAKE_SOURCE_DIR}/config.ini"
    "${CMAKE_BINARY_DIR}/config.ini"
    COPYONLY  # 纯拷贝，不替换变量
)

# GNU 标准安装路径
include(GNUInstallDirs)
install(TARGETS ChatRoom
    BUNDLE DESTINATION .
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # 静态库
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # 动态库
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # 可执行文件
)
# 将config.ini安装到bin目录
install(FILES config.ini DESTINATION ${CMAKE_INSTALL_BINDIR})

qt_finalize_executable(ChatRoom)

# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ChatRoom APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation














































# target_sources(ChatRoom
#     PRIVATE
#         main.cpp
#         mainwindow.cpp
#         mainwindow.h
#     @if true
#         mainwindow.ui
#     @endif
#     @if false
#         
#     @endif
#         
#     PUBLIC
#         
# )

# qt_standard_project_setup()     #set(CMAKE_AUTOUIC ON)  set(CMAKE_AUTOMOC ON)
# set_target_properties(ChatRoom PROPERTIES
#     MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )
# 
# install(TARGETS ChatRoom
#     BUNDLE DESTINATION .
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )
# 
# if(QT_VERSION_MAJOR EQUAL 6)
#     qt_finalize_executable(ChatRoom)
# endif()





#qt_add_executable(ChatRoom)  //Creates and finalizes an application target of a platform-specific type.
#This command performs the following tasks:

#    1. Create a CMake target of the appropriate type for the target platform.
#    2. Link the target to the Qt::Core library.
#    3. Handle finalization of the CMake target.












# cmake_minimum_required(VERSION 3.5)
# 
# project(ChatRoom VERSION 0.1 LANGUAGES CXX)
# 
# set(CMAKE_AUTOUIC ON)
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
# 
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 
# @if false
# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)
# 
# set(TS_FILES )
# @else
# find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
# find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
# @endif
# 
# set(PROJECT_SOURCES
#         main.cpp
#         mainwindow.cpp
#         mainwindow.h
#     @if true
#         mainwindow.ui
#     @endif
#     @if false
#         ${TS_FILES}
#     @endif
# )
# 
# if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
#     qt_add_executable(ChatRoom
#         MANUAL_FINALIZATION
#         ${PROJECT_SOURCES}
#     )
# # Define target properties for Android with Qt 6 as:
# #    set_property(TARGET ChatRoom APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
# #                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
# @if false
# 
#     qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
# @endif
# else()
#     if(ANDROID)
#         add_library(ChatRoom SHARED
#             ${PROJECT_SOURCES}
#         )
# # Define properties for Android with Qt 5 after find_package() calls as:
# #    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#     else()
#         add_executable(ChatRoom
#             ${PROJECT_SOURCES}
#         )
#     endif()
# @if false
# 
#     qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
# @endif
# endif()
# 
# target_link_libraries(ChatRoom PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
# 
# # Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# # If you are developing for iOS or macOS you should consider setting an
# # explicit, fixed bundle identifier manually though.
# if(${QT_VERSION} VERSION_LESS 6.1.0)
#   set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ChatRoom)
# endif()
# set_target_properties(ChatRoom PROPERTIES
#     ${BUNDLE_ID_OPTION}
#     MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#     MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#     MACOSX_BUNDLE TRUE
#     WIN32_EXECUTABLE TRUE
# )
# 
# include(GNUInstallDirs)
# install(TARGETS ChatRoom
#     BUNDLE DESTINATION .
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )
# 
# if(QT_VERSION_MAJOR EQUAL 6)
#     qt_finalize_executable(ChatRoom)
# endif()
