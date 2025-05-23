cmake_minimum_required(VERSION 3.5)

project(Tetgen VERSION 1.6.0)

set(TETGEN_MASTER_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(TETGEN_MASTER_PROJECT ON)
endif()

# Set some default options
option(BUILD_TETGEN_BIN "Build the tetgen binary?" ${TETGEN_MASTER_PROJECT})
option(SINGLE "Use single precision floating point numbers?" OFF)

####################################################################
# Define various compile defs and flags for proper function
#####################################################################
set(COMPILE_DEFS "")
if(SINGLE)
    list(APPEND COMPILE_DEFS SINGLE)
endif(SINGLE)

# Set some definitions in accord with Shewchuk's recommendations
if(CMAKE_SYSTEM_NAME STREQUAL Linux AND CMAKE_COMPILER_IS_GNUCC)   
    # Linux & gcc only... APPLE does not define fpu_control
    list(APPEND COMPILE_DEFS LINUX)
    set_source_files_properties(predicates.cxx PROPERTIES COMPILE_FLAGS -O0)
elseif(WIN32 AND CMAKE_COMPILER_IS_GNUCC)
	# workaround for mingw, doe not fo FPU initalization therefore the result will not be
	# binary exact, but it seems that we are not using tetgen from cellblender anyway
    set_source_files_properties(predicates.cxx PROPERTIES COMPILE_FLAGS -O0)
elseif(WIN32)
    list(APPEND COMPILE_DEFS CPU86)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        list(APPEND COMPILE_DEFS _WIN64)
    endif()
    set_source_files_properties(predicates.cxx PROPERTIES COMPILE_FLAGS /Od)
endif()

#####################################################################
# Define library targets
#####################################################################

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12)
    # OBJECT LIBRARY: compiles the sources only once
    add_library(tet_objlib OBJECT tetgen.cxx predicates.cxx)
    target_compile_definitions(tet_objlib PRIVATE TETLIBRARY ${COMPILE_DEFS})
    target_compile_options(tet_objlib PRIVATE -w)   # Suppress warnings
    target_include_directories(tet_objlib INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

    # SHARED LIBRARY
    add_library(tetshared SHARED $<TARGET_OBJECTS:tet_objlib>)
    target_link_libraries(tetshared PUBLIC tet_objlib)
    install(TARGETS tetshared DESTINATION lib)

    # STATIC LIBRARY
    add_library(tetstatic STATIC $<TARGET_OBJECTS:tet_objlib>)
    target_link_libraries(tetstatic PUBLIC tet_objlib)
    install(TARGETS tetstatic DESTINATION lib)

else() #CMAKE_VERSION < 3.12
    # SHARED LIBRARY
    add_library(tetshared SHARED tetgen.cxx predicates.cxx)
    target_compile_definitions(tetshared PRIVATE TETLIBRARY ${COMPILE_DEFS})
    target_compile_options(tetshared PRIVATE -w)   # Suppress warnings
    target_include_directories(tetshared INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
    install(TARGETS tetshared DESTINATION lib)

    # STATIC LIBRARY
    add_library(tetstatic STATIC tetgen.cxx predicates.cxx)
    target_compile_definitions(tetstatic PRIVATE TETLIBRARY ${COMPILE_DEFS})
    target_compile_options(tetstatic PRIVATE -w)   # Suppress warnings
    target_include_directories(tetstatic INTERFACE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )
    install(TARGETS tetstatic DESTINATION lib)
endif()


#####################################################################
# Setup binary targets
#####################################################################
if(BUILD_TETGEN_BIN)
    add_executable(tetgen tetgen.cxx predicates.cxx)
    target_compile_definitions(tetgen PRIVATE ${COMPILE_DEFS})
    install(TARGETS tetgen DESTINATION bin)
endif()
