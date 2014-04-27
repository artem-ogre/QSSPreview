cmake_minimum_required(VERSION 2.8.11)
project(qss)

# Eliminate a warning when building in Windows that relates
# to static linking of Qt executables to qtmain.lib.
# This policy was introduced in CMake version 2.8.11.
# CMake version 2.8.11.2 warns when the policy is not set
# and uses OLD behavior.
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 OLD)
endif()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Widgets finds its own dependencies.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5UiTools REQUIRED)
find_package(Qt5XmlPatterns REQUIRED)

#Зreprocessor definitions
if (WIN32)
	add_definitions(-DCL_COMP)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /EHsc")
	# Activate use of multi-processor compilation if we have Microsft Visual C++ compiler.
	if (MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
	endif (MSVC)
endif (WIN32)

#Collect .cpp, .h, and .ui sources from the corresponding directies
file(GLOB files_cpp "source/*.cpp")
file(GLOB files_h "source/*.h")
file(GLOB files_ui "ui/*.ui")

qt5_wrap_ui( files_ui_h ${files_ui})

#Add an executable
add_executable(qss ${files_cpp} ${files_h} ${files_ui_h})

# Use the Widgets module from Qt 5.
target_link_libraries(qss Qt5::Widgets Qt5::UiTools Qt5::XmlPatterns)