#!/usr/bin/bash
set -e

BUILD_DIR="build"
BUILD_TYPE="Release"

#pre build
DO_CLEAN=false

#main build
CONAN_STEP=false
DO_BUILD=false

#post build
DO_CLANG_TIDY=false
DO_CLANG_FORMAT=false
DO_UPDATE_RESOURCES=false
DO_RUN_BIN=false

# 🔍 Analyse des options
while getopts ":dcxfrtpb" opt; do
  case $opt in
    d)
      BUILD_TYPE="Debug"
      ;;
    c)
      DO_CLEAN=true
      ;;
    b)
      DO_BUILD=true
      ;;
    x)
      echo "🔄 Étape Conan forcée."
      CONAN_STEP=true
      ;;
    r)
      echo "Lancement du binaire"
      DO_RUN_BIN=true
      ;;
    t)
      echo "Lancement de clang-tidy"
      DO_CLANG_TIDY=true
      ;;
    f)
      echo "Lancement de clang-format"
      DO_CLANG_FORMAT=true
      ;;
    p)
      echo "Mettre a jour des fichiers conf et resources"
      DO_UPDATE_RESOURCES=true
      ;;
    \?)
      echo "❌ Option invalide: -$OPTARG"
      echo "Usage: $0 [-d] [-c] [-x](pour forcer l'étape Conan) [-b](pour construire le projet) [-r](pour lancer le binaire) [-t] (pour lancer le linter) [-f] (pour lancer clang-format) [-p] (pour mettre a jour les fichiers resources et config)"
      exit 1
      ;;
  esac
done

do_update_resources() {
  rm -rf ./build/config/*
  rm -rf ./build/resources/*
  cp -r ./config/* ./build/config/
  cp -r ./resources/* ./build/resources/
}

do_clang_format() {
  clang-format-18 -i $(find ./lib/* ./src/* -name "*.cpp" -o -name "*.hpp")
}

do_clean() {
  if [ -d $BUILD_DIR ]; then
      echo "🧹 Nettoyage du dossier $BUILD_DIR..."
      rm -rvf "$BUILD_DIR" && echo "✅ Dossier nettoyé."
  else
      echo "ℹ️ Le dossier a été supprimé. donc Rien à nettoyer."
  fi
}

do_run_bin() {
  cd $BUILD_DIR
  ./sfml-app
}

detect_os() {
  case "$(uname -s)" in
    Linux*) echo "Linux" ;;
    *) echo "Windows" ;;
  esac
}

#conan setup begin

WIN_MSVC_VS_PROFILE="
[settings]
build_type=Release
os=Windows
compiler=msvc
compiler.cppstd=23
[conf]
tools.build:compiler_executables={'c': 'cl', 'cpp': 'cl'}
"

WIN_MSVC_NINJA_PROFILE="
[settings]
build_type=Release
os=Windows
compiler=msvc
compiler.cppstd=23
[conf]
tools.build:compiler_executables={'c': 'cl', 'cpp': 'cl'}
tools.cmake.cmaketoolchain:generator=Ninja
"

ACTIVE_WIN_PROFILE="$WIN_MSVC_NINJA_PROFILE"

WIN_CLANG_PROFILE="
[settings]
build_type=Release
os=Windows
compiler=clang
compiler.cppstd=23
compiler.version=19
compiler.runtime=dynamic
compiler.runtime_type=Release
compiler.runtime_version=v144
[conf]
tools.build:compiler_executables={'c': 'clang', 'cpp': 'clang++'}
"

LIN_CLANG_PROFILE="
[settings]
build_type=Release
compiler=clang
compiler.cppstd=23
compiler.libcxx=libc++
compiler.version=18
os=Linux
[conf]
tools.build:compiler_executables={'c': 'clang', 'cpp': 'clang++'}
"

LIN_GNU_PROFILE="
[settings]
build_type=Release
compiler=gcc
compiler.cppstd=23
compiler.version=14
os=Linux
[conf]
tools.build:compiler_executables={'c': 'gcc-14', 'cpp': 'g++-14'}
"

ACTIVE_LIN_PROFILE="$LIN_GNU_PROFILE"

PROFILE_NAME_BASE="active_profile_base"
PROFILE_NAME="active_profile"

get_conan_profile_path() {
  profile_name="$1"
  os=$(detect_os)
  if [ "$os" = "Linux" ]; then
    conan profile path "$profile_name" 2>/dev/null
  elif [ "$os" = "Windows" ]; then
    cygpath.exe $(conan profile path "$profile_name" 2>/dev/null)
  fi
}

generate_conan_profile() {
  os=$(detect_os)
  if [ "$os" = "Windows" ]; then
    cat << EOF
include($PROFILE_NAME_BASE)
$ACTIVE_WIN_PROFILE
EOF
  elif [ "$os" = "Linux" ]; then
    cat << EOF
include($PROFILE_NAME_BASE)
$ACTIVE_LIN_PROFILE
EOF
  fi
}

create_conan_profile() {
  profile_name="$1"

  echo "Creation $profile_name"

  set +e
  conan profile detect --name=$profile_name --force 2>/dev/null
  set -e
  return
}

do_conan_install() {

  create_conan_profile $PROFILE_NAME_BASE

  create_conan_profile $PROFILE_NAME

  OUTPUT_FILE=$(generate_conan_profile)
  echo "$OUTPUT_FILE"
  echo "$OUTPUT_FILE" > "$(get_conan_profile_path $PROFILE_NAME)"

  conan install . \
      --output-folder=$BUILD_DIR \
      -pr:h $PROFILE_NAME \
      -pr:b $PROFILE_NAME \
      --build=missing \
      -s build_type=$BUILD_TYPE
}

get_conan_build_preset() {
  if [ "$BUILD_TYPE" = "Release" ]; then echo "--preset conan-release"; else echo "--preset conan-debug"; fi
}

get_conan_configure_preset() {
  os=$(detect_os)
  if [ "$os" = "Windows" ]; then
    if [ "$ACTIVE_WIN_PROFILE" = "$WIN_MSVC_VS_PROFILE" ]; then
      echo "--preset conan-default" #msvc's multi-configuration preset
      return
    fi
  fi
  get_conan_build_preset
}
#conan setup end

do_build_app() {
  # Étape CMake configure + build
  cd $BUILD_DIR
  cmake .. \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      $(get_conan_configure_preset)

  cmake --build . \
      $(get_conan_build_preset) \
      --parallel $(nproc)

  ctest

  cd ../
}

do_clang_tidy() {
  command="clang-tidy-18 -fix-errors -fix $(git status| grep -oE '[^[:space:]]+\.(hpp|cpp)' ) -p build"
  #command="clang-tidy-18 -fix-errors -fix $(git diff --name-only  HEAD~1...HEAD | grep -oE '[^[:space:]]+\.(hpp|cpp)') -p build"
  #command="clang-tidy-18 -fix-errors -fix $(find ./lib/* ./src/* -name "*.cpp" -o -name "*.hpp") -p build"
  echo "Running clang-tidy command"
  echo $command
  eval $command
}

if [[ "$DO_CLEAN" == "true" ]]; then
  do_clean
fi

if [[ "$CONAN_STEP" == "true" || ( ! -d $BUILD_DIR && "$DO_CLEAN" == "false" ) ]]; then
  do_conan_install
fi

if [[ "$DO_BUILD" == "true" ]]; then
  do_build_app
fi

if [[ "$DO_CLANG_TIDY" == "true" ]]; then
  do_clang_tidy
fi

if [[ "$DO_CLANG_FORMAT" == "true" ]]; then
  do_clang_format
fi

if [[ "$DO_UPDATE_RESOURCES" == "true" ]]; then
  do_update_resources
fi

if [[ "$DO_RUN_BIN" == "true" ]]; then
  do_run_bin
fi