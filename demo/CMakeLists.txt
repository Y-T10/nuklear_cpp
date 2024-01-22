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
    demo PRIVATE fmt::fmt Threads::Threads Atm
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,SDL2::SDL2,SDL2::SDL2-static>
    SDL2Cpp
    Fontconfig::Fontconfig
)
