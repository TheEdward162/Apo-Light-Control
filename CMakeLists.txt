cmake_minimum_required(VERSION 3.8)
project(APO_SEM)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        MZApi/MZRegisters.h
        Misc/IOTools.cpp
        Misc/IOTools.h
        Network/NetworkHandler.cpp
        Network/NetworkHandler.h
        Unit/LightUnit.cpp
        Unit/LightUnit.h
        Engine.cpp
        Engine.h
        main.cpp
        test.ppm
        MZApi/DeviceInput.cpp
        MZApi/DeviceInput.h MZApi/Display.cpp MZApi/Display.h MZApi/Mapper.cpp MZApi/Mapper.h)

add_executable(APO_SEM ${SOURCE_FILES})