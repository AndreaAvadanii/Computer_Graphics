cmake_minimum_required(VERSION 3.24)

project(Computer_Graphics VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- 1. GLFW (Libreria Finestre) ---
# Questa va compilata, quindi usiamo MakeAvailable
message(STATUS "Scaricando GLFW...")
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# --- 2. GLM (Matematica) ---
# TRUCCO PER RISOLVERE IL TUO ERRORE:
# Scarichiamo GLM, ma NON usiamo "MakeAvailable".
# Usiamo "Populate". Questo scarica i file ma ignora il CMakeLists rotto di GLM.
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

# --- ESEGUIBILE ---
add_executable(${PROJECT_NAME} src/main.cpp src/glad.c)

# --- LINKING ---
target_link_libraries(${PROJECT_NAME} PRIVATE glfw opengl32)

# --- INCLUDE ---
target_include_directories(${PROJECT_NAME} PRIVATE 
    include             # Per glad
    ${glm_SOURCE_DIR}   # Per glm (la cartella che abbiamo appena scaricato con Populate)
)