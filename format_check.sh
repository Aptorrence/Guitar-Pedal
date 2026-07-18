# This script verifies the formatting in the target directories.
find app common -iname '*.h' -o -iname '*.c' -o -iname '*.cc' -o -iname '*.cpp' | xargs clang-format --dry-run -Werror
