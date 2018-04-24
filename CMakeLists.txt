cmake_minimum_required(VERSION 3.8)
project(APO_SEM)

set(CMAKE_CXX_STANDARD 11)

file(GLOB SOURCES
		*.h
		*.cpp
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

set(SOURCE_FILES_DISPLAY
        display_test.cpp
        MZApi/MZRegisters.h
        Misc/IOTools.cpp
        Misc/IOTools.h
        Network/NetworkHandler.cpp
        Network/NetworkHandler.h
        Unit/LightUnit.cpp
        Unit/LightUnit.h
        Engine.cpp
        Engine.h
        test.ppm
		Misc
        MZApi/Mapper.cpp
        MZApi/Mapper.h
        MZApi/DeviceInput.cpp
        MZApi/DeviceInput.h
        DisplayUtils/Colour.h
        DisplayUtils/Colour.cpp
        MZApi/Display.cpp
        MZApi/Display.h
        DisplayUtils/font_types.h
        DisplayUtils/font_rom8x16.h
        DisplayUtils/font_prop14x16.h
        DisplayUtils/Screen.h
        DisplayUtils/ListScreen.h
        DisplayUtils/UnitScreen.h
        DisplayUtils/ListScreen.cpp
        DisplayUtils/UnitScreen.cpp)

add_executable(APO_SEM ${SOURCES})
add_executable(DISPLAY_TEST ${SOURCE_FILES_DISPLAY})