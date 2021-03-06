cmake_minimum_required (VERSION 2.6)
project (ircclient)

option (WITH_GUI "Build the GUI test app" ON)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
find_package (Qt4 REQUIRED)
set (QT_DONT_USE_QTGUI TRUE)
set (QT_USE_QTNETWORK TRUE)
find_package (Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include (${QT_USE_FILE})
set (LIBS ${QT_LIBRARIES})
add_subdirectory(src)
add_subdirectory(mirc)
add_subdirectory(testapp)
if (WITH_GUI)
	if (WIN32)
		set (GUI_TYPE WIN32)
	endif()
	if (APPLE)
		set (GUI_TYPE MACOSX_BUNDLE)
	endif()
	set (QT_DONT_USE_QTGUI FALSE)
	include (${QT_USE_FILE})
	set (LIBS ${QT_LIBRARIES})
	add_subdirectory(guiapp)
endif()
