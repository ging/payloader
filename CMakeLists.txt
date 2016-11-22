cmake_minimum_required(VERSION 2.6)
find_package(PkgConfig)
#functions
function(test_lib LIB_NAME)
  if (${LIB_NAME} MATCHES "^.*-NOTFOUND")
    message(FATAL_ERROR "lib not found: " ${LIB_NAME} " check README")
    return()
  endif(${LIB_NAME} MATCHES "^.*-NOTFOUND")
endfunction(test_lib)
project (PTLOADER)

set(CMAKE_MODULE_PATH "cmake/")
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -fno-stack-protector")
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

FIND_PACKAGE( OpenMP REQUIRED)
if(OPENMP_FOUND)
	message("OPENMP FOUND")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/libs/" "/home/alvaro/Documentos/arqueopterix/payloader_test/libdeps/include/")

file(GLOB_RECURSE SOURCES_PROT1 "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/prototype1.cpp")

add_executable(prototype1 ${SOURCES_PROT1})

# GLIB
find_package(Glib REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})
target_link_libraries(prototype1 ${GLIB_LIBRARIES})

# BOOST
set (BOOST_LIBS thread regex system)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
target_link_libraries(prototype1 ${Boost_LIBRARIES})

# GTHREAD
find_library(GTHREAD gthread-2.0 HINTS "/home/alvaro/Documentos/webrtc/git/licode/build/libdeps/build/lib/")
test_lib(${GTHREAD})

# OPENSSL
find_library(SSL ssl HINTS "/home/alvaro/Documentos/webrtc/git/licode/build/libdeps/build/lib/")
test_lib(${SSL})
find_library(CRYPTO crypto HINTS "/home/alvaro/Documentos/webrtc/git/licode/build/libdeps/build/lib/")
test_lib(${CRYPTO})

# Math and Zlib
find_library(MATH m)
test_lib(${MATH})
find_library(ZLIB z)
test_lib(${ZLIB})
find_library(LZMA lzma)
test_lib(${LZMA})

find_library(AVRESAMP swresample)
test_lib(${AVRESAMP})
find_library(BZ2 bz2)
test_lib(${BZ2})

# Libav
find_library(AVUTIL avutil)
test_lib(${AVUTIL})
set (LIBS ${AVUTIL})
find_library(AVCODEC avcodec)
test_lib(${AVCODEC})
set (LIBS ${LIBS} ${AVCODEC})
find_library(AVFORMAT avformat)
test_lib(${AVFORMAT})
set (LIBS ${LIBS} ${AVFORMAT})

# LOG4CXX
find_library(LOG log4cxx)
test_lib(${LOG})

message("Libs ${GTHREAD} ${SSL} ${CRYPTO} ${LIBS} ${LOG}")

target_link_libraries(prototype1 ${GTHREAD} ${SSL} ${CRYPTO} ${BZ2} ${AVRESAMP} ${LZMA} ${MATH} ${ZLIB} ${LIBS} ${LOG})
