# プログラム名
add_executable(demo)

# プログラムファイルの出力場所を追加
set_target_properties(demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ソースファイルを追加
target_sources(demo PRIVATE main.cpp)
# ヘッダファイルのディレクトリを追加
target_include_directories(demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# プログラムが利用するターゲットを追加
target_link_libraries(
    demo PRIVATE Threads::Threads Nuklear-cpp::Nuklear-cpp
)

add_executable(original_demo)

# プログラムファイルの出力場所を追加
set_target_properties(original_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ソースファイルを追加
target_sources(original_demo PRIVATE main_origin.cpp)
# ヘッダファイルのディレクトリを追加
target_include_directories(original_demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# プログラムが利用するターゲットを追加
target_link_libraries(
    original_demo PRIVATE
    Nuklear-cpp::Nuklear-cpp
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,SDL2::SDL2,SDL2::SDL2-static>
)
