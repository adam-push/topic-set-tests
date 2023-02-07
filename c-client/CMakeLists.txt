cmake_minimum_required(VERSION 3.20)

project(c_performance_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

set(TARGET ${CMAKE_CURRENT_SOURCE_DIR}/target)
set(DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

set(CMAKE_BINARY_DIR ${TARGET})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TARGET}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TARGET}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${TARGET}/bin)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

add_library(diffusion STATIC IMPORTED)
add_library(apr::apr-1 STATIC IMPORTED)
add_library(apr::aprutil-1 STATIC IMPORTED)

if(WIN32)
    # Windows (32-bit or 64-bit)
    set(WINDOWS_DIR ${DEPENDENCIES_DIR}/windows)

    if (CMAKE_SIZEOF_VOID_P MATCHES 8)
        # 64-bit
        set(ARCH_DIR "x64")
    elseif(CMAKE_SIZEOF_VOID_P MATCHES 4)
        # 32-bit
        set(ARCH_DIR "x86")
    endif()

    set_target_properties(apr::apr-1 PROPERTIES
        IMPORTED_IMPLIB "${WINDOWS_DIR}/apr/${ARCH_DIR}/lib/libapr-1.lib"
        IMPORTED_LOCATION "${WINDOWS_DIR}/apr/${ARCH_DIR}/lib/libapr-1.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${WINDOWS_DIR}/apr/${ARCH_DIR}/include"
    )

    set_target_properties(apr::aprutil-1 PROPERTIES
        IMPORTED_IMPLIB "${WINDOWS_DIR}/apr/${ARCH_DIR}/lib/libaprutil-1.lib"
        IMPORTED_LOCATION "${WINDOWS_DIR}/apr/${ARCH_DIR}/lib/libaprutil-1.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${WINDOWS_DIR}/apr/${ARCH_DIR}/include"
    )

    add_library(pcre::pcre3 STATIC IMPORTED)
    set_target_properties(pcre::pcre3 PROPERTIES
        IMPORTED_IMPLIB "${WINDOWS_DIR}/pcre/${ARCH_DIR}/lib/pcre3.lib"
        IMPORTED_LOCATION "${WINDOWS_DIR}/pcre/${ARCH_DIR}/lib/pcre3.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${WINDOWS_DIR}/pcre/${ARCH_DIR}/include"
    )

    add_library(pcre::pcreposix3 STATIC IMPORTED)
    set_target_properties(pcre::pcreposix3 PROPERTIES
        IMPORTED_IMPLIB "${WINDOWS_DIR}/pcre/${ARCH_DIR}/lib/pcreposix3.lib"
        IMPORTED_LOCATION "${WINDOWS_DIR}/pcre/${ARCH_DIR}/lib/pcreposix3.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${WINDOWS_DIR}/pcre/${ARCH_DIR}/include"
    )

    add_library(zlib:zlib STATIC IMPORTED)
    set_target_properties(zlib::zlib PROPERTIES
        IMPORTED_IMPLIB "${WINDOWS_DIR}/zlib/${ARCH_DIR}/lib/zlib.lib"
        IMPORTED_LOCATION "${WINDOWS_DIR}/zlib/${ARCH_DIR}/lib/zlib.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${WINDOWS_DIR}/zlib/${ARCH_DIR}/include"
    )

    set_target_properties(diffusion PROPERTIES
        IMPORTED_IMPLIB "${WINDOWS_DIR}/diffusion/${ARCH_DIR}/lib/uci.lib"
        IMPORTED_LOCATION "${WINDOWS_DIR}/diffusion/${ARCH_DIR}/lib/uci.lib"
        INTERFACE_INCLUDE_DIRECTORIES "${WINDOWS_DIR}/diffusion/${ARCH_DIR}/include"
    )

    # PCRE library that merges pcre3 and pcreposix3
    add_library(pcre INTERFACE IMPORTED)
    set_property(TARGET pcre PROPERTY INTERFACE_LINK_LIBRARIES pcre::pcre3 pcre::pcreposix3)

    # ZLIB library that merges zlib::zlib
    add_library(zlib INTERFACE IMPORTED)
    set_property(TARGET zlib PROPERTY INTERFACE_LINK_LIBRARIES zlib::zlib)

else()
    if(UNIX AND NOT APPLE)
        # Linux
        set(OS_DIR "linux")
    else()
        # MacOS
        set(OS_DIR "macos")
    endif()

    set_target_properties(apr::apr-1 PROPERTIES
        IMPORTED_LOCATION "${DEPENDENCIES_DIR}/${OS_DIR}/apr/lib/libapr-1.a"
        INTERFACE_INCLUDE_DIRECTORIES "${DEPENDENCIES_DIR}/${OS_DIR}/apr/include"
    )

    set_target_properties(apr::aprutil-1 PROPERTIES
        IMPORTED_LOCATION "${DEPENDENCIES_DIR}/${OS_DIR}/apr/lib/libaprutil-1.a"
        INTERFACE_INCLUDE_DIRECTORIES "${DEPENDENCIES_DIR}/${OS_DIR}/apr/include"
    )

    set_target_properties(diffusion PROPERTIES
        IMPORTED_LOCATION "${DEPENDENCIES_DIR}/${OS_DIR}/diffusion/lib/libdiffusion.a"
        INTERFACE_INCLUDE_DIRECTORIES "${DEPENDENCIES_DIR}/${OS_DIR}/diffusion/include"
    )

    find_package(PCRE REQUIRED)
    include_directories(${PCRE_INCLUDE_DIRS})

    # PCRE library that merges pcre3 and pcreposix3
    add_library(pcre INTERFACE IMPORTED)
    set_property(TARGET pcre PROPERTY INTERFACE_LINK_LIBRARIES ${PCRE_LIBRARIES})

    find_package(ZLIB REQUIRED)
    include_directories(${ZLIB_INCLUDE_DIRS})

    # ZLIB library that merges zlib::zlib
    add_library(zlib INTERFACE IMPORTED)
    set_property(TARGET zlib PROPERTY INTERFACE_LINK_LIBRARIES ${ZLIB_LIBRARIES})

endif()

# APR library that merges apr and apr-util
add_library(apr INTERFACE IMPORTED)
set_property(TARGET apr PROPERTY INTERFACE_LINK_LIBRARIES apr::apr-1 apr::aprutil-1)


# test executable
file(GLOB_RECURSE SRCS source/*.[hc])
add_executable(test ${SRCS})
target_link_libraries(test PRIVATE diffusion apr pcre zlib)
