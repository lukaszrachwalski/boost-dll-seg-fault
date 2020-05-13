cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
project("BoostDllTest")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.67.0 REQUIRED COMPONENTS filesystem)

add_executable(BoostDllTest)

target_sources(BoostDllTest PRIVATE
	main.cpp
	)

target_link_libraries(BoostDllTest PRIVATE
        dl
	Boost::boost
        Boost::filesystem
	)
