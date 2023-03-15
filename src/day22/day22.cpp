#include <array>
#include <cctype>
#include <charconv>
#include <cmath>
#include <fmt/format.h>
#include <fstream>
#include <map>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/reverse.hpp>
#include <string_view>
#include <system_error>
#include <variant>

inline constexpr std::array dir_ascii{'>', 'v', '<', '^'};
inline constexpr std::array dir_vec{std::pair{1, 0}, std::pair{0, 1}, std::pair{-1, 0}, std::pair{0, -1}};

inline constexpr auto to_idx       = [](auto&& t_idx) { return static_cast<std::size_t>(t_idx - 1); };
inline constexpr auto is_in_map    = [](auto const t_chr) { return t_chr != ' '; };
inline constexpr auto get_col_proj = [](std::size_t const t_x) {
  return [x = to_idx(t_x)](auto&& t_row) { return x >= t_row.size() ? ' ' : t_row[x]; };
};

void move(std::vector<std::string>& t_map, std::pair<std::size_t, std::size_t>& t_pos, int t_move, std::size_t& t_dir,
          [[maybe_unused]] auto&& t_check_wrap_around) {
  using ranges::views::reverse, ranges::find_if, ranges::distance;
  int curr_x = static_cast<int>(t_pos.first);
  int curr_y = static_cast<int>(t_pos.second);

  for (int i = 0; i < t_move; ++i) {
    auto const [inc_x, inc_y] = dir_vec[t_dir];

    auto x_tmp            = static_cast<std::size_t>(curr_x + inc_x);
    auto y_tmp            = static_cast<std::size_t>(curr_y + inc_y);
    auto&& [mov_x, mov_y] = t_check_wrap_around(t_map, x_tmp, y_tmp, t_dir);

    if (t_map[to_idx(mov_y)][to_idx(mov_x)] == '#') {
      break;
    } else {
      curr_x = static_cast<int>(mov_x);
      curr_y = static_cast<int>(mov_y);
    }
  }

  t_pos.first  = static_cast<std::size_t>(curr_x);
  t_pos.second = static_cast<std::size_t>(curr_y);
}

inline auto const part1_wraparound = [](auto const& t_map, auto t_x_pos, auto t_y_pos, auto& t_dir) {
  using ranges::views::reverse, ranges::find_if;

  std::pair<std::size_t, std::size_t> ret_val{t_x_pos, t_y_pos};

  auto const col_proj = get_col_proj(t_x_pos);
  if (t_dir == 1 and (t_y_pos >= t_map.size() + 1 or t_x_pos >= t_map[to_idx(t_y_pos)].size() + 1)) {
    ret_val.second = static_cast<std::size_t>(distance(t_map.begin(), find_if(t_map, is_in_map, col_proj)) + 1);
  } else if (t_dir == 3 and (t_y_pos == 0 or t_map[to_idx(t_y_pos)][to_idx(t_x_pos)] == ' ')) {
    auto res       = find_if(t_map | reverse, is_in_map, col_proj).base();
    ret_val.second = static_cast<std::size_t>(distance(t_map.begin(), res));
  } else if (t_dir == 0 and (t_x_pos >= t_map[to_idx(t_y_pos)].size() + 1)) {
    auto res      = distance(t_map[to_idx(t_y_pos)].begin(), find_if(t_map[to_idx(t_y_pos)], is_in_map));
    ret_val.first = static_cast<std::size_t>(res + 1);
  } else if (t_dir == 2 and (t_x_pos == 0 or t_map[to_idx(t_y_pos)][to_idx(t_x_pos)] == ' ')) {
    auto res      = find_if(t_map[to_idx(t_y_pos)] | reverse, is_in_map).base();
    ret_val.first = static_cast<std::size_t>(distance(t_map[to_idx(t_y_pos)].begin(), res));
  }

  return ret_val;
};

// struct Instructor {
//   std::string_view::const_pointer pos_;
//   std::string_view::const_pointer end_ptr_;
//   std::string_view instruction_;

//   auto operator()() noexcept {
//     std::variant<char, int> ret_val;

//     int res = 0;
//     auto [non_matching_pos, ec]{std::from_chars(this->pos_, this->end_ptr_, res)};
//     if (ec == std::errc::invalid_argument) {  // R or L
//       ret_val = *this->pos_;
//       ++this->pos_;
//       return ret_val;
//     } else if (ec == std::errc()) {
//       this->pos_ = non_matching_pos;
//     }

//     return ret_val;
//   }

//   auto begin() { return this->operator()(); }

//   auto end() { return this->end_ptr_; }
// };

void part1() {
  using ranges::getlines, ranges::to_vector;

  std::fstream in((INPUT_FILE));
  auto const rng          = getlines(in) | to_vector;
  auto const& instruction = rng.back();
  auto map                = std::vector(rng.begin(), rng.end() - 2);

  auto curr_pos = std::pair{map.front().find('.') + 1, 1UL};

  std::size_t dir = 0;
  for (auto* ptr = instruction.data(); ptr != instruction.data() + instruction.size();) {
    int steps = 0;
    auto [non_matching_pos, ec]{std::from_chars(ptr, instruction.data() + instruction.size(), steps)};
    if (ec == std::errc::invalid_argument) {  // R or L
      if (*ptr == 'R') {
        dir = (dir + 1) % 4;
      } else {
        dir = (dir + 3) % 4;  // dir = 0 -> dir - 1 = 3; dir = 1 -> dir - 1 = 0
      }
      ++ptr;
    } else if (ec == std::errc()) {
      ptr = non_matching_pos;
    }

    move(map, curr_pos, steps, dir, part1_wraparound);
  }

  auto result = 1000 * curr_pos.second + 4 * curr_pos.first + dir;
  fmt::print("result: {}\n", result);
}

inline auto const part2_wraparound = [](auto const& /*t_map*/, auto /*t_x_pos*/, auto /*t_y_pos*/, auto& /*t_dir*/) {
  return std::pair{1, 1};
};

inline constexpr auto side = 50;

void part2() {
  using ranges::getlines, ranges::to_vector;

  std::fstream in((INPUT_FILE));
  auto const rng          = getlines(in) | to_vector;
  auto const& instruction = rng.back();
  auto map                = std::vector(rng.begin(), rng.end() - 2);

  auto curr_pos   = std::pair{map.front().find('.') + 1, 1UL};
  std::size_t dir = 0;
  for (auto* ptr = instruction.data(); ptr != instruction.data() + instruction.size();) {
    int steps = 0;
    auto [non_matching_pos, ec]{std::from_chars(ptr, instruction.data() + instruction.size(), steps)};
    if (ec == std::errc::invalid_argument) {  // R or L
      if (*ptr == 'R') {
        dir = (dir + 1) % 4;
      } else {
        dir = (dir + 3) % 4;  // dir = 0 -> dir - 1 = 3; dir = 1 -> dir - 1 = 0
      }
      ++ptr;
    } else if (ec == std::errc()) {
      ptr = non_matching_pos;
    }

    move(map, curr_pos, steps, dir, part2_wraparound);
  }

  auto result = 1000 * curr_pos.second + 4 * curr_pos.first + dir;
  fmt::print("result: {}\n", result);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
