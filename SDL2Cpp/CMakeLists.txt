# 自作ライブラリSDL2Cppを定義
add_library(SDL2Cpp)

target_sources(
    SDL2Cpp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/SDLCpp_video.cpp
)

# ヘッダファイルのディレクトリを追加
target_include_directories(
    SDL2Cpp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# プログラムが利用するターゲットを追加
target_link_libraries(
    SDL2Cpp PUBLIC
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<BOOL:${BUILD_SHARED_LIBS}>,SDL2::SDL2,SDL2::SDL2-static>
    SDL2_ttf::SDL2_ttf
)