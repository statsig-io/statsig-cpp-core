cmake_minimum_required(VERSION 3.28)

project(Statsig
    VERSION 0.10.2
    DESCRIPTION "Statsig C++ Core SDK"
    LANGUAGES CXX
)

# Wrap the prebuilt dylib as an imported library
file(GLOB STATSIG_SOURCE_FILES "src/*")
add_library(Statsig ${STATSIG_SOURCE_FILES})

# Set include directories
target_include_directories(Statsig PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# External Dependency
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0  
)

FetchContent_MakeAvailable(json)
target_link_libraries(Statsig PUBLIC nlohmann_json::nlohmann_json)

# Check if we should use local build file
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

if(USE_LOCAL_BUILD_FILE)
    # Get the FFI binary path from environment variable
    set(FFI_BINARY_PATH $ENV{FFI_BINARY_PATH})
    if(NOT FFI_BINARY_PATH)
        message(WARN "USE_LOCAL_BUILD_FILE is true but FFI_BINARY_PATH environment variable is not set, using default")
        set(FFI_BINARY_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../target/release/libstatsig_ffi.dylib")
    endif()
    message(STATUS "Using local build file from: ${FFI_BINARY_PATH}")
    target_link_libraries(Statsig PRIVATE ${FFI_BINARY_PATH})
else()
    set(GITHUB_OWNER "statsig-io")
    #TODO(xinli) We probably want to publish to statsig-cpp-core repo so we can publish non-prod version 
    set(GITHUB_REPO "statsig-server-core") 

    # Adjust asset name according to your OS/arch
   
    # generate_target_name(${PROJECT_VERSION} ASSET_NAME SUFFIX) TODO(xinli)
    set(ASSET_NAME "statsig-ffi-0.10.2-aarch64-apple-darwin.zip")
    set(ASSET_URL "https://github.com/${GITHUB_OWNER}/${GITHUB_REPO}/releases/download/${PROJECT_VERSION}/${ASSET_NAME}")
    set(ASSET_PATH "${ASSET_NAME}")
    set(SUFFIX "dylib") #
    message(STATUS "Downloading asset from: ${ASSET_URL}")
    file(DOWNLOAD
        "${ASSET_URL}"
        "${ASSET_PATH}"
    )
    message(STATUS "Extracting ${ASSET_NAME} to ${CMAKE_BINARY_DIR}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E tar xvf  "${ASSET_PATH}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
    target_link_libraries(Statsig PRIVATE "${CMAKE_BINARY_DIR}/libstatsig_ffi.${SUFFIX}")
endif()


if(ENABLE_STATSIG_CPP_UNIT_TEST)
    enable_testing()
    add_subdirectory(tests)
endif()

#--------------------- Built Binary Resolution Helper ---------------------#
function(generate_target_name version target_var suffix_var)
    # Detect OS
    set(os_name "${CMAKE_SYSTEM_NAME}")
    if(os_name STREQUAL "Darwin")
        set(os_target "apple-darwin")
        set(library_suffix "dylib")
    elseif(os_name STREQUAL "Linux")
        set()
        execute_process(
            COMMAND ldd --version
            RESULT_VARIABLE _ldd_result
            OUTPUT_VARIABLE _ldd_output
            ERROR_VARIABLE _ldd_error
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_STRIP_TRAILING_WHITESPACE
        )
        if(_ldd_result EQUAL 0)
            if(_ldd_output MATCHES "musl")
                set(os_target "linux-musl")
                set(library_suffix "so")

            else()
                set(os_target "linux-gnu")
                set(library_suffix "so")
            endif()
        endif()
    else()
        message(FATAL_ERROR "Unsupported OS: ${os_name}, Contact Statsig support to add support for it")
    endif()

    # Detect CPU Architecture
    set(cpu_arch "${CMAKE_SYSTEM_PROCESSOR}")
    if(cpu_arch MATCHES "x86_64|AMD64")
        set(cpu_target "x86_64")
    elseif(cpu_arch MATCHES "aarch64|arm64")
        set(cpu_target "aarch64")
    else()
        message(FATAL_ERROR "Unsupported CPU architecture: ${cpu_arch}, Contact Statsig support to add support for it")
    endif()    

    # Compose target name
    set(target_name "statsig-ffi-${version}-${cpu_target}-${os_target}.zip")

    # Return value to caller
    set(${target_var} "${target_name}" PARENT_SCOPE)
    set(${suffix_var} "${library_suffix}" PARENT_SCOPE)
endfunction()