cmake_minimum_required(VERSION 3.27)
project(smk)

option(BUILD_UNIT_TESTS "Build unit tests executable" OFF)
option(BUILD_EXAMPLES "Build examples executable" OFF)
option(INCLUDE_STB_IMAGE "Include stb image binary code" ON)

if (NOT DEFINED VERSION_MAJOR)
	set(VERSION_MAJOR 1)
	set(VERSION_MINOR 0)
	set(VERSION_BUILD 2)

	add_compile_definitions(VERSION_MAJOR=${VERSION_MAJOR})
	add_compile_definitions(VERSION_MINOR=${VERSION_MINOR})
	add_compile_definitions(VERSION_BUILD=${VERSION_BUILD})
endif()

set(CMAKE_CXX_STANDARD 20)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# tell visual studio to assume all source code is UTF8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

if(INCLUDE_STB_IMAGE)
    add_definitions(-DINCLUDE_STB_IMAGE)
endif()

#
# Smokin Lib
#
set(LIB_FILES
	${PROJECT_NAME}/Lang.hpp
	${PROJECT_NAME}/Lang.cpp
	${PROJECT_NAME}/Text.hpp
	${PROJECT_NAME}/Text.cpp
	${PROJECT_NAME}/File.hpp
	${PROJECT_NAME}/File.cpp
	${PROJECT_NAME}/Timer.hpp
	${PROJECT_NAME}/Timer.cpp
	${PROJECT_NAME}/CommandLine.hpp
	${PROJECT_NAME}/CommandLine.cpp
	${PROJECT_NAME}/Log.hpp
	${PROJECT_NAME}/Log.cpp

	${PROJECT_NAME}/math/RunningStats.hpp
	${PROJECT_NAME}/math/Mathematics.hpp
	${PROJECT_NAME}/math/Mathematics.cpp
	${PROJECT_NAME}/math/Easing.hpp
	${PROJECT_NAME}/math/Easing.cpp
	${PROJECT_NAME}/math/Matrix.hpp
	${PROJECT_NAME}/math/Matrix.cpp
	${PROJECT_NAME}/math/Vector.hpp
	${PROJECT_NAME}/math/Quaternion.hpp
	${PROJECT_NAME}/math/Quaternion.cpp

	${PROJECT_NAME}/vision/Mat.hpp
	${PROJECT_NAME}/vision/Mat.cpp
	${PROJECT_NAME}/vision/Features.hpp
	${PROJECT_NAME}/vision/Features.cpp
	${PROJECT_NAME}/vision/Filter.hpp
	${PROJECT_NAME}/vision/Filter.cpp
	${PROJECT_NAME}/vision/Image.hpp
	${PROJECT_NAME}/vision/Image.cpp
	${PROJECT_NAME}/vision/Optimization.hpp
	${PROJECT_NAME}/vision/Optimization.cpp
	${PROJECT_NAME}/vision/Opticalflow.hpp
	${PROJECT_NAME}/vision/Opticalflow.cpp
	${PROJECT_NAME}/vision/Drawing.hpp
	${PROJECT_NAME}/vision/Drawing.cpp

	${PROJECT_NAME}/audio/CircularBuffer.hpp
	${PROJECT_NAME}/audio/Wav.hpp
	${PROJECT_NAME}/audio/Wav.cpp
	${PROJECT_NAME}/audio/Audio.hpp
	${PROJECT_NAME}/audio/Audio.cpp
	${PROJECT_NAME}/audio/Recorder.hpp
	${PROJECT_NAME}/audio/Recorder.cpp

	${PROJECT_NAME}/threading/Barrier.hpp
	${PROJECT_NAME}/threading/WaitEvent.hpp
	${PROJECT_NAME}/threading/Worker.hpp
	${PROJECT_NAME}/threading/Worker.cpp
	${PROJECT_NAME}/threading/WorkQueue.hpp
	${PROJECT_NAME}/threading/WorkQueue.cpp

	${PROJECT_NAME}/UnitTest.hpp
	${PROJECT_NAME}/UnitTest.cpp
)
# organize in folders for VS
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${LIB_FILES}) 

add_library(${PROJECT_NAME} STATIC ${LIB_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC vendor)

#
# Tests
#
if(BUILD_UNIT_TESTS)
	set(FAST_TEST_FILES
		test/Tester.cpp
		test/Text.cpp
		test/File.cpp
		test/Timer.cpp
		test/Mathematics.cpp
	)

	set(SLOW_TEST_FILES
		test/Vision.cpp
	)

	# disable slow tests
	# set(SLOW_TEST_FILES "")

	add_executable(test ${FAST_TEST_FILES} ${SLOW_TEST_FILES})
	target_link_libraries(test ${PROJECT_NAME})
endif()

#
# Examples
#
if(BUILD_EXAMPLES)
	add_executable(panorama examples/panorama.cpp)
	target_link_libraries(panorama ${PROJECT_NAME})

	add_executable(opticalflow examples/opticalflow.cpp)
	target_link_libraries(opticalflow ${PROJECT_NAME})
endif()
