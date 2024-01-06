include(FetchContent)

# nuklearのリポジトリをダウンロードする
FetchContent_Declare(
  nuklear
  GIT_REPOSITORY https://github.com/Immediate-Mode-UI/Nuklear.git
  URL_HASH MD5=ce77291b280ea54cf18e0403c936c69a
)

# nuklearの情報を集める
FetchContent_Populate(nuklear)

# Nuklearのターゲットを定義する
add_library(Nuklear INTERFACE)
add_library(Nuklear::Nuklear ALIAS Nuklear)

# Nuklearのヘッダファイルがあるディレクトリを設定する、
target_include_directories(Nuklear INTERFACE "${nuklear_SOURCE_DIR}")
