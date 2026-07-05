#!/usr/bin/bash
set -e

BUILD_DIR="build"
BUILD_TYPE="Release"
DO_CLEAN=false
CONAN_STEP=false
DO_CLANG_TIDY=false

# 🔍 Analyse des options
while getopts ":dcfrt" opt; do
  case $opt in
    d)
      BUILD_TYPE="Debug"
      ;;
    c)
      DO_CLEAN=true
      ;;
    f)
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
    \?)
      echo "❌ Option invalide: -$OPTARG"
      echo "Usage: $0 [-d] [-c] [-f](pour forcer l'étape Conan) [-r](pour lancer le binaire)"
      exit 1
      ;;
  esac
done

# 🔍 Analyse des options
if $DO_CLEAN; then
    if [ -d $BUILD_DIR ]; then
        echo "🧹 Nettoyage du dossier $BUILD_DIR..."
        rm -rvf "$BUILD_DIR" && echo "✅ Dossier nettoyé."
    else
        echo "ℹ️ Le dossier a été supprimé. donc Rien à nettoyer."
    fi
    exit 0
fi

echo "⚙️ Build en mode $BUILD_TYPE"

echo "Creation clang_profile"

PROFILE_NAME="clang_profile"
PROFILE_PATH=$(conan profile path "$PROFILE_NAME" 2>/dev/null)

conan profile detect --name=$(PROFILE_NAME) --force

sed -i 's/compiler=.*/compiler=clang/' "$PROFILE_PATH"
sed -i 's/compiler.version=.*/compiler.version=18/' "$PROFILE_PATH"
sed -i 's/compiler.libcxx=.*/compiler.libcxx=libc++/' "$PROFILE_PATH"
sed -i 's/compiler.cppstd=.*/compiler.cppstd=23/' "$PROFILE_PATH"

if [[ "$CONAN_STEP" == "true" || ! -d $BUILD_DIR ]]; then
  CXX=clang++ CC=clang conan install . \
      --output-folder=$BUILD_DIR \
      --profile clang_profile\
      --build=missing \
      -s build_type=$BUILD_TYPE
fi

# Étape CMake configure + build
cd $BUILD_DIR
cmake .. \
    -DCMAKE_C_COMPILER=clang\
    -DCMAKE_CXX_COMPILER=clang++\
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON\
    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE

cmake --build .

ctest

cd ../

if $DO_CLANG_TIDY; then
  clang-tidy-18 -fix-errors -fix -header-filter=.* $(find ./lib/* ./src/* -name "*.cpp") -p build
fi