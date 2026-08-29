Prerequisites:
CMake
Conan
MSVC compiler and git bash if you re on windows
Clang or G++

check build.sh
you ll see the different compile profiles supported
WIN_MSVC_PROFILE, WIN_CLANG_PROFILE, LIN_CLANG_PROFILE, LIN_GNU_PROFILE
locally, modify generate_conan_clang_profile function in the script so that you can choose your compiler accordingly
