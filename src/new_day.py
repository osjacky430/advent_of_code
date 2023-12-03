#! /usr/bin/env python3.11

import argparse
from os import curdir, listdir, makedirs
from os.path import isdir, join

PARSER = argparse.ArgumentParser()
PARSER.add_argument('date')

CMAKELIST_TEXT = '''\
add_executable({day} {day}.cpp)
target_link_libraries({day} PRIVATE advent_of_code_2022 project_option project_warning fmt::fmt)
target_compile_definitions({day} PRIVATE INPUT_FILE="${{CMAKE_CURRENT_SOURCE_DIR}}/{day}.txt")
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


def _main():
    res, _ = PARSER.parse_known_args()

    op_dir = join(curdir, res.date)
    dirs = [files for files in listdir(op_dir) if isdir(join(op_dir, files))]
    days = [int(day[3:]) for day in dirs]
    days.sort()

    new_day = 'day' + str((days[-1] if len(days) != 0 else 0) + 1)
    new_day_dir = join(op_dir, new_day)
    makedirs(new_day_dir)

    with open(join(op_dir, 'CMakeLists.txt'), 'a', encoding='utf-8') as file:
        file.write(f'add_subdirectory({new_day})\n')

    with open(join(new_day_dir, 'CMakeLists.txt'), 'w', encoding='utf-8') as file:
        file.write(CMAKELIST_TEXT.format(day=new_day))

    with open(join(new_day_dir, f'{new_day}.cpp'), 'w', encoding='utf-8') as file:
        file.write(TEMPLATE_CPP_FILE)

    with open(join(new_day_dir, f'{new_day}.txt'), 'w', encoding='utf-8') as file:
        file.write('')


if __name__ == '__main__':
    _main()
