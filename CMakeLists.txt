cmake_minimum_required(VERSION 3.24)

project(Computer_Graphics VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- 1. GLFW ---
message(STATUS "Scaricando GLFW...")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# --- 2. GLM ---
message(STATUS "Scaricando GLM (Solo sorgenti)...")
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        0.9.9.8
)
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
    FetchContent_Populate(glm)
endif()

# --- 3. ASSIMP (CORRETTO) ---
message(STATUS "Scaricando Assimp...")
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.2.5
)

# --- FIX PER IL TUO ERRORE ---
# 1. Disabilita i Warning come Errori (così non blocca la build per piccolezze)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

# 2. DISABILITA L'IMPORTATORE MMD (Quello che ha causato l'errore su uint8_t)
set(ASSIMP_BUILD_MMD_IMPORTER OFF CACHE BOOL "" FORCE)

# 3. Ottimizzazioni standard (disabilita test e tool extra)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

# --- ESEGUIBILE ---
add_executable(${PROJECT_NAME} src/main.cpp src/glad.c)

# --- LINKING ---
target_link_libraries(${PROJECT_NAME} PRIVATE glfw opengl32 assimp)

# --- INCLUDE ---
target_include_directories(${PROJECT_NAME} PRIVATE 
    include 
    ${glm_SOURCE_DIR}
)