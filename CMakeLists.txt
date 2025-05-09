##****************************************************************************
## Copyright © 2020 Jan Erik Breimo. All rights reserved.
## Created by Jan Erik Breimo on 2020-05-04.
##
## This file is distributed under the Zero-Clause BSD License.
## License text is included with the source distribution.
##****************************************************************************
cmake_minimum_required(VERSION 3.16)
project(Yconvert VERSION 0.4.1)

set(CMAKE_CXX_STANDARD 20)

# Set YCONVERT_MASTER_PROJECT to ON if Yconvert is the root level project.
if (NOT DEFINED YCONVERT_MASTER_PROJECT)
    if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(YCONVERT_MASTER_PROJECT ON)
    else ()
        set(YCONVERT_MASTER_PROJECT OFF)
    endif ()
endif ()

option(YCONVERT_ISO_CODE_PAGES "Enable ISO 8859 code pages" ON)
option(YCONVERT_DOS_CODE_PAGES "Enable old MSDOS code pages" ON)
option(YCONVERT_WIN_CODE_PAGES "Enable Windows code pages" ON)
option(YCONVERT_MAC_CODE_PAGES "Enable Mac code pages" OFF)

option(YCONVERT_BUILD_TESTS "Build tests" ${YCONVERT_MASTER_PROJECT})
option(YCONVERT_INSTALL "Generate the install target" ${YCONVERT_MASTER_PROJECT})

include(GNUInstallDirs)

function(yconvert_target_enable_all_warnings target)
    target_compile_options(${target}
        PRIVATE
            $<$<CXX_COMPILER_ID:Clang>:-Wall>
            $<$<CXX_COMPILER_ID:GNU>:-Wall>
            $<$<CXX_COMPILER_ID:AppleClang>:-Wall>
            $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )
endfunction()

configure_file(src/Yconvert/YconvertVersion.hpp.in YconvertVersion.hpp @ONLY)

add_library(Yconvert
    include/Yconvert/CodepointIterator.hpp
    include/Yconvert/Convert.hpp
    include/Yconvert/Converter.hpp
    include/Yconvert/ConversionException.hpp
    include/Yconvert/Encoding.hpp
    include/Yconvert/EncodingChecker.hpp
    include/Yconvert/ErrorPolicy.hpp
    include/Yconvert/Yconvert.hpp
    include/Yconvert/YconvertDefinitions.hpp
    include/Yconvert/YconvertException.hpp
    include/Yconvert/Details/InputStreamWrapper.hpp
    src/Yconvert/CodePageDecoder.cpp
    src/Yconvert/CodePageDecoder.hpp
    src/Yconvert/CodePageDefinitions.hpp
    src/Yconvert/CodePageEncoder.cpp
    src/Yconvert/CodePageEncoder.hpp
    src/Yconvert/CodepointIterator.cpp
    src/Yconvert/Convert.cpp
    src/Yconvert/Converter.cpp
    src/Yconvert/Decoder.cpp
    src/Yconvert/Decoder.hpp
    src/Yconvert/Encoder.hpp
    src/Yconvert/Encoder.cpp
    src/Yconvert/Encoding.cpp
    src/Yconvert/EncodingChecker.cpp
    src/Yconvert/MakeEncodersAndDecoders.cpp
    src/Yconvert/MakeEncodersAndDecoders.hpp
    src/Yconvert/Utf8Decoder.cpp
    src/Yconvert/Utf8Decoder.hpp
    src/Yconvert/Utf8Encoder.cpp
    src/Yconvert/Utf8Encoder.hpp
    src/Yconvert/Utf16Decoder.hpp
    src/Yconvert/Utf16Encoder.hpp
    src/Yconvert/Utf32Decoder.hpp
    src/Yconvert/Utf32Encoder.hpp
    src/Yconvert/YconvertThrow.hpp
    src/Yconvert/Details/InputStreamWrapper.cpp
)

target_include_directories(Yconvert BEFORE
    PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(Yconvert
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
)

yconvert_target_enable_all_warnings(Yconvert)

add_library(Yconvert::Yconvert ALIAS Yconvert)

if (YCONVERT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/YconvertTest)
endif()

export(TARGETS Yconvert
    NAMESPACE Yconvert::
    FILE YconvertConfig.cmake
)

if (YCONVERT_INSTALL)
    install(TARGETS Yconvert
        EXPORT YconvertConfig
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    install(EXPORT YconvertConfig
        FILE
            YconvertConfig.cmake
        NAMESPACE
            Yconvert::
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/Yconvert
    )

    file(GLOB_RECURSE PUBLIC_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
    install(
        FILES
            ${PUBLIC_INCLUDES}
            ${CMAKE_CURRENT_BINARY_DIR}/YconvertVersion.hpp
        DESTINATION
            ${CMAKE_INSTALL_INCLUDEDIR}/Yconvert
    )

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(YconvertConfigVersion.cmake
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/YconvertConfigVersion.cmake
        DESTINATION
            ${CMAKE_INSTALL_LIBDIR}/cmake/Yconvert
    )
endif ()
