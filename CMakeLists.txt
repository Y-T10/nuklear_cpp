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

# 外部ソース読み込み
include(FetchContent)

## Nuklear
# nuklearのリポジトリをダウンロードする
FetchContent_Declare(
  Nuklear
  GIT_REPOSITORY https://github.com/Immediate-Mode-UI/Nuklear.git
  GIT_TAG be0a3f6
)

# nuklearの情報を集める
FetchContent_MakeAvailable(Nuklear)
# Nuklearのターゲットを定義する
add_library(Nuklear INTERFACE)
# Nuklearのヘッダファイルがあるディレクトリを設定する、
target_include_directories(Nuklear INTERFACE "${nuklear_SOURCE_DIR}")

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
target_link_libraries(${PROJECT_NAME} PUBLIC Nuklear)
# ライブラリ設定マクロを定義する
target_compile_definitions(${PROJECT_NAME} PUBLIC
  NK_INCLUDE_DEFAULT_ALLOCATOR
  NK_INCLUDE_FIXED_TYPES
  NK_INCLUDE_STANDARD_IO
  NK_INCLUDE_STANDARD_VARARGS
  NK_INCLUDE_VERTEX_BUFFER_OUTPUT
  NK_INCLUDE_FONT_BAKING
  NK_INCLUDE_DEFAULT_FONT
)

if (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    find_package(SDL2 REQUIRED)
    # Add sub add_subdirectories
    add_subdirectory(demo)
endif()
