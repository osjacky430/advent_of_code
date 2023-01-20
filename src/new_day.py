#! /usr/bin/env python3.8

from os import listdir, curdir, makedirs
from os.path import isdir, join

CMAKELIST_TEXT = '''\
add_executable({day} {day}.cpp)
target_link_libraries({day} PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions({day} PRIVATE INPUT_FILE="${{CMAKE_CURRENT_SOURCE_DIR}}/{day}.txt")
set_property(TARGET {day} PROPERTY BUILD_RPATH "/opt/gcc-11.1.0/lib64")
'''

TEMPLATE_CPP_FILE = '''\
#include <fmt/format.h>
#include <fstream>
#include <range/v3/view/getlines.hpp>

void part1() {
  using ranges::getlines;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);
}

void part2() {
  using ranges::getlines;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
'''


def main():
    dirs = [files for files in listdir(curdir) if isdir(files)]
    days = [int(day[3:]) for day in dirs]
    days.sort()

    new_day = 'day' + str(days[-1] + 1)
    new_day_dir = join(curdir, new_day)
    makedirs(new_day_dir)

    with open(join(curdir, 'CMakeLists.txt'), 'a') as file:
        file.write('add_subdirectory({})\n'.format(new_day))

    with open(join(new_day_dir, 'CMakeLists.txt'), 'w') as file:
        file.write(CMAKELIST_TEXT.format(day=new_day))

    with open(join(new_day_dir, '{}.cpp'.format(new_day)), 'w') as file:
        file.write(TEMPLATE_CPP_FILE)

    with open(join(new_day_dir, '{}.txt'.format(new_day)), 'w') as file:
        file.write('')


if __name__ == '__main__':
    main()
