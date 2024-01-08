include(FetchContent)

set(NK_Fetch_Name "nuklear-header")

# nuklearのリポジトリをダウンロードする
FetchContent_Declare(
  ${NK_Fetch_Name}
  GIT_REPOSITORY https://github.com/Immediate-Mode-UI/Nuklear.git
  URL_HASH MD5=ce77291b280ea54cf18e0403c936c69a
)

# nuklearの情報を集める
FetchContent_Populate(${NK_Fetch_Name})

# Nuklearのターゲットを定義する
add_library(${NK_Fetch_Name} INTERFACE)

# Nuklearのヘッダファイルがあるディレクトリを設定する、
target_include_directories(${NK_Fetch_Name} INTERFACE "${${NK_Fetch_Name}_SOURCE_DIR}")
