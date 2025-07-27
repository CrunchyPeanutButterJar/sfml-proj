#!/usr/bin/bash
set -e

BUILD_DIR="build"
BUILD_TYPE="Release"
DO_CLEAN=false
CONAN_STEP=false

# 🔍 Analyse des options
while getopts ":dcf" opt; do
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
    \?)
      echo "❌ Option invalide: -$OPTARG"
      echo "Usage: $0 [-d] [-c] [-f](pour forcer l'étape Conan)"
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
elif [[ "$1" == "--debug" ]]; then
    BUILD_TYPE="Debug"
fi

echo "⚙️ Build en mode $BUILD_TYPE"

if [[ "$CONAN_STEP" == "true" || ! -d $BUILD_DIR ]]; then
  conan install . \
      --output-folder=$BUILD_DIR \
      --build=missing \
      -s build_type=$BUILD_TYPE
fi

# Étape CMake configure + build
cd $BUILD_DIR
cmake .. \
    -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE

cmake --build .
