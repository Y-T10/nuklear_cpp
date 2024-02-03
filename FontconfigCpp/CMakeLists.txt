# 自作ライブラリSDL2Cppを定義
add_library(Fcpp)

target_sources(
    Fcpp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Fcpp.cpp
)

# ヘッダファイルのディレクトリを追加
target_include_directories(
    Fcpp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# プログラムが利用するターゲットを追加
target_link_libraries(
    Fcpp PUBLIC
    Fontconfig::Fontconfig
    Boost::boost
)