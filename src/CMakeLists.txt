# プログラム名
add_executable(sample)

# プログラムファイルの出力場所を追加
set_target_properties(sample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# ソースファイルを追加
target_sources(sample PRIVATE main.cpp)
# ヘッダファイルのディレクトリを追加
target_include_directories(sample PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# プログラムが利用するターゲットを追加
target_link_libraries(
    sample PRIVATE fmt::fmt Threads::Threads
)