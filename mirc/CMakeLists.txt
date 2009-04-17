set (mirc_SOURCES
	src/mirc.cpp
	src/parser.cpp
	src/script.cpp
)
set (mirc_MOC_HEADERS
	includes/mirc.h
	includes/parser.h
	includes/script.h
)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)
QT4_WRAP_CPP (mirc_MOC_SOURCES ${mirc_MOC_HEADERS})
add_library (mirc STATIC ${mirc_SOURCES} ${mirc_MOC_SOURCES})

link_directories(${CMAKE_CURRENT_BINARY_DIR})
add_executable(mirctest src/main.cpp)
target_link_libraries (mirctest mirc ${LIBS})
