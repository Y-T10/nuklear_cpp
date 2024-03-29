cmake_minimum_required(VERSION 3.20)

# このプロジェクトの概要を設定する
project(Nuklear-cpp
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Personal cpp template repository."
)

# CMakeのポリシーを設定する
cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0128 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0128 NEW)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# C++コンパイラに渡すオプションを設定する
add_compile_options(-Wall $<$<CONFIG:Debug>:-g3>)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)

# clangd向けの設定ファイルを生成させる
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# CPMを有効にする
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake)
# Nuklearを読み込む
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Nuklear.cmake)

# CPMを用いてネット上のパッケージを取り込む
CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 10.2.1
)

# システム内のパッケージを読み込む
find_package(Threads REQUIRED)
#find_package(Boost REQUIRED)

# ライブラリを宣言する
add_library(${PROJECT_NAME})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS ${PROJECT_NAME})

# ソースファイルを追加
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/nuklear-impl.cpp)
# ヘッダファイルのディレクトリを追加
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# プログラムが利用するターゲットを追加
target_link_libraries(${PROJECT_NAME} PUBLIC nuklear-header)

if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    find_package(SDL2 REQUIRED)
    # Add sub add_subdirectories
    add_subdirectory(demo)
endif()
