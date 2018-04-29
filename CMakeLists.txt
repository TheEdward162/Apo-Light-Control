cmake_minimum_required(VERSION 3.8)
project(APO_SEM)

set(CMAKE_CXX_STANDARD 11)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

file(GLOB SOURCES
		*.h
		Engine.cpp
		main.cpp
		DisplayUtils/*.h
		DisplayUtils/*.cpp
		Misc/*.h
		Misc/*.cpp
		MZApi/*.h
		MZApi/*.cpp
		Network/*.h
		Network/*.cpp
		Unit/*.h
		Unit/*.cpp
		)

add_executable(APO_SEM ${SOURCES})

target_link_libraries(APO_SEM pthread)
