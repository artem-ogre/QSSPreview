cmake_minimum_required(VERSION 2.8.11)
project(QSSPreview)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.

#Fixes glu32 issue of Qt5
if (WIN32)
	set (CMAKE_LIBRARY_PATH "C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x64" CACHE PATH "Path to your WinSDK for glu32 bindings") 
endif (WIN32)
find_package(Qt5 COMPONENTS Widgets UiTools XMLPatterns REQUIRED)

#Preprocessor definitions
if (WIN32)
	add_definitions(-DCL_COMP)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /EHsc")
	# Activate use of multi-processor compilation if we have Microsft Visual C++ compiler.
	if (MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	endif (MSVC)
endif (WIN32)

# GUI Type
if(WIN32)
	set(gui_type WIN32)
endif(WIN32)
if(APPLE)
	set(gui_type MACOSX_BUNDLE)
endif(APPLE)

#Collect .cpp, .h, and .ui sources from the corresponding directies
set(files_cpp
	source/main.cpp
	source/mainwindow.cpp
)
set(files_h
	source/mainwindow.h
	source/common.h
)
set(files_ui
	ui/MainWindow.ui
	ui/About.ui
)
set(files_qrc 
	images/icons.qrc
)

qt5_wrap_ui( files_ui_h ${files_ui})
QT5_ADD_RESOURCES( files_qrc_cpp ${files_qrc} ) #resources have to be added to executables!

#Add an executable
add_executable(QSSPreview ${gui_type} ${files_cpp} ${files_h} ${files_ui_h} ${files_qrc_cpp} QSSPreview.rc)

# Use the Widgets module from Qt 5.
target_link_libraries(QSSPreview Qt5::Widgets Qt5::UiTools Qt5::XmlPatterns)
if(WIN32)
	target_link_libraries(QSSPreview Qt5::WinMain)
endif(WIN32)