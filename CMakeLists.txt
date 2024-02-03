cmake_minimum_required(VERSION 3.20)

# このプロジェクトの概要を設定する
project(Nuklear-cpp
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Personal cpp template repository."
)

# ビルド設定
set(CMAKE_BUILD_TYPE Release)
set(BUILD_SHARED_LIBS OFF)

# CMakeのポリシーを設定する
cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0128 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0128 NEW)
cmake_policy(SET CMP0074 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0074 NEW)
cmake_policy(SET CMP0077 NEW)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# C++コンパイラに渡すオプションを設定する
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall $<$<CONFIG:Debug>:-g3>)

# clangd向けの設定ファイルを生成させる
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# CPMを有効にする
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake)

# CPMを用いてネット上のパッケージを取り込む
CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 10.2.1
)
CPMAddPackage(
    NAME SDL2_ttf
    GITHUB_REPOSITORY libsdl-org/SDL_ttf
    GIT_TAG release-2.20.2
)

# システム内のパッケージを読み込む
find_package(Threads REQUIRED)
find_package(Boost 1.74.0 REQUIRED)

# 自作ライブラリAtmを定義
add_library(Atm INTERFACE)

# ヘッダファイルのディレクトリを追加
target_include_directories(Atm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# プログラムが利用するターゲットを追加
target_link_libraries(Atm INTERFACE Threads::Threads Boost::boost)

if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
find_package(SDL2 REQUIRED)
    find_package(Fontconfig REQUIRED)
    # Add sub add_subdirectories
    add_subdirectory(FontconfigCpp)
    add_subdirectory(SDL2Cpp)
    add_subdirectory(demo)
endif()
