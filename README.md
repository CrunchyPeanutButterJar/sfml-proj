Prerequisites:
CMake
Conan
MSVC compiler and git bash if you re on windows
Clang or G++

check build.sh
you ll see the different compile profiles supported
WIN_MSVC_VS_PROFILE, WIN_MSVC_NINJA_PROFILE, WIN_CLANG_PROFILE, LIN_CLANG_PROFILE, LIN_GNU_PROFILE
locally, modify ACTIVE_WIN_PROFILE or ACTIVE_LIN_PROFILE ( if you re on linux ) variable in the script so that you can choose your build profile according to what suits you
