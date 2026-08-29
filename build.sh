#!/usr/bin/bash
set -e

BUILD_DIR="build"
BUILD_TYPE="Release"
DO_CLEAN=false
CONAN_STEP=false
DO_CLANG_TIDY=false
DO_CLANG_FORMAT=false
DO_UPDATE_RESOURCES=false

# 🔍 Analyse des options
while getopts ":dcxfrtp" opt; do
  case $opt in
    d)
      BUILD_TYPE="Debug"
      ;;
    c)
      DO_CLEAN=true
      ;;
    x)
      echo "🔄 Étape Conan forcée."
      CONAN_STEP=true
      ;;
    r)
      echo "Lancement du binaire"
      cd ./$BUILD_DIR
      ./sfml-app
      exit 0
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
      echo "Usage: $0 [-d] [-c] [-x](pour forcer l'étape Conan) [-r](pour lancer le binaire) [-t] (pour lancer le linter) [-f] (pour lancer clang-format) [-p] (pour mettre a jour les fichiers resources et config)"
      exit 1
      ;;
  esac
done

if $DO_UPDATE_RESOURCES; then
  rm -rf ./build/config/*
  rm -rf ./build/resources/*
  cp -r ./config/* ./build/config/
  cp -r ./resources/* ./build/resources/
  exit 0
fi


if $DO_CLANG_FORMAT; then
  clang-format-18 -i $(find ./lib/* ./src/* -name "*.cpp" -o -name "*.hpp")
  exit 0
fi

if $DO_CLEAN; then
    if [ -d $BUILD_DIR ]; then
        echo "🧹 Nettoyage du dossier $BUILD_DIR..."
        rm -rvf "$BUILD_DIR" && echo "✅ Dossier nettoyé."
    else
        echo "ℹ️ Le dossier a été supprimé. donc Rien à nettoyer."
    fi
    exit 0
fi

detect_os() {
  case "$(uname -s)" in
    Linux*) echo "Linux" ;;
    *) echo "Windows" ;;
  esac
}

PROFILE_NAME_BASE="clang_profile_base"
PROFILE_NAME="clang_profile"

get_conan_profile_path() {
  profile_name="$1"
  os=$(detect_os)
  if [ "$os" = "Linux" ]; then
    conan profile path "$profile_name" 2>/dev/null
  elif [ "$os" = "Windows" ]; then
    cygpath.exe $(conan profile path "$profile_name" 2>/dev/null)
  fi
}

WIN_MSVC_PROFILE="
[settings]
build_type=Release
os=Windows
compiler=msvc
compiler.cppstd=23
[conf]
tools.build:compiler_executables={'c': 'cl', 'cpp': 'cl'}
"

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

generate_conan_clang_profile() {
  os=$(detect_os)
  if [ "$os" = "Windows" ]; then
    cat << EOF
include($PROFILE_NAME_BASE)
$WIN_MSVC_PROFILE
EOF
  elif [ "$os" = "Linux" ]; then
    cat << EOF
include($PROFILE_NAME_BASE)
$LIN_GNU_PROFILE
EOF
  fi
}

echo "Creation clang_profile_base"

set +e
conan profile detect --name=$PROFILE_NAME_BASE --force 2>/dev/null
set -e

echo "Creation clang_profile"
OUTPUT_FILE=$(generate_conan_clang_profile)
echo "$OUTPUT_FILE"
echo "$OUTPUT_FILE" > "$(get_conan_profile_path $PROFILE_NAME)"

echo

if [[ "$CONAN_STEP" == "true" || ! -d $BUILD_DIR ]]; then
  conan install . \
      --output-folder=$BUILD_DIR \
      -pr:h $PROFILE_NAME \
      -pr:b $PROFILE_NAME \
      --build=missing \
      -s build_type=$BUILD_TYPE
fi

get_conan_build_preset() {
  if [ "$BUILD_TYPE" = "Release" ]; then echo "--preset conan-release"; else echo "--preset conan-debug"; fi
}

get_conan_configure_preset() {
  os=$(detect_os)
  if [ "$os" = "Windows" ]; then
    echo "--preset conan-default" #msvc's multi-configuration preset
  elif [ "$os" = "Linux" ]; then
    get_conan_build_preset
  fi
}

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

if $DO_CLANG_TIDY; then
  command="clang-tidy-18 -fix-errors -fix $(git status| grep -oE '[^[:space:]]+\.(hpp|cpp)' ) -p build"
  #command="clang-tidy-18 -fix-errors -fix $(git diff --name-only  HEAD~1...HEAD | grep -oE '[^[:space:]]+\.(hpp|cpp)') -p build"
  #command="clang-tidy-18 -fix-errors -fix $(find ./lib/* ./src/* -name "*.cpp" -o -name "*.hpp") -p build"
  echo "Running clang-tidy command"
  echo $command
  eval $command
fi