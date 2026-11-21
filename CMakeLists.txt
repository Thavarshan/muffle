cmake_minimum_required(VERSION 3.20)

project(muffle VERSION 0.1.0 LANGUAGES CXX)

if(WIN32)
    message(FATAL_ERROR "Muffle v0.1 requires POSIX process APIs")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(MUFFLE_ENABLE_SANITIZERS "Enable ASan and UBSan" OFF)

add_library(muffle_core STATIC src/cli.cpp src/runner.cpp)
target_include_directories(muffle_core PUBLIC include)

if(MSVC)
    target_compile_options(muffle_core PRIVATE /W4 /WX /permissive-)
else()
    target_compile_options(muffle_core PRIVATE
        -Wall -Wextra -Wpedantic -Werror -Wconversion
        -Wsign-conversion -Wshadow -Wformat=2
    )
endif()

if(MUFFLE_ENABLE_SANITIZERS)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message(FATAL_ERROR "Sanitizers require Clang or GCC")
    endif()
    target_compile_options(muffle_core PUBLIC
        -fsanitize=address,undefined -fno-omit-frame-pointer
    )
    target_link_options(muffle_core PUBLIC -fsanitize=address,undefined)
endif()

add_executable(muffle src/main.cpp)
target_link_libraries(muffle PRIVATE muffle_core)
target_compile_definitions(muffle PRIVATE
    MUFFLE_VERSION="${PROJECT_VERSION}"
)

if(MSVC)
    target_compile_options(muffle PRIVATE /W4 /WX /permissive-)
else()
    target_compile_options(muffle PRIVATE
        -Wall -Wextra -Wpedantic -Werror -Wconversion
        -Wsign-conversion -Wshadow -Wformat=2
    )
endif()

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_executable(muffle_unit tests/unit_cli.cpp)
    target_link_libraries(muffle_unit PRIVATE muffle_core)
    if(MSVC)
        target_compile_options(muffle_unit PRIVATE /W4 /WX /permissive-)
    else()
        target_compile_options(muffle_unit PRIVATE
            -Wall -Wextra -Wpedantic -Werror -Wconversion
            -Wsign-conversion -Wshadow -Wformat=2
        )
    endif()
    add_test(NAME unit.cli COMMAND muffle_unit)
    add_test(NAME integration.cli
        COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/tests/integration.py $<TARGET_FILE:muffle>
    )
endif()

include(GNUInstallDirs)

install(TARGETS muffle RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES LICENSE README.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/muffle
)
install(FILES docs/man/muffle.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
)

set(CPACK_PACKAGE_NAME "muffle")
set(CPACK_PACKAGE_VENDOR "Jerome Thayananthajothy")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Suppress successful command output and show failure details")
set(CPACK_PACKAGE_DESCRIPTION
    "Muffle captures command output, suppresses it on success, and displays a
useful tail and log path on failure.")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/Thavarshan/muffle")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_LICENSE "MIT")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_FILE_NAME
    "muffle-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_GENERATOR "TGZ;DEB;RPM")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jerome Thayananthajothy")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_RPM_PACKAGE_LICENSE "MIT")
set(CPACK_RPM_PACKAGE_GROUP "Applications/System")
set(CPACK_RPM_PACKAGE_AUTOREQ ON)

include(CPack)
