#include "string_util.hpp"
#include <array>
#include <cctype>
#include <cmath>
#include <fmt/format.h>
#include <fstream>
#include <iterator>
#include <numeric>
#include <optional>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/reverse.hpp>
#include <string_view>
#include <system_error>

inline constexpr std::array dir_char{'>', 'v', '<', '^'};
inline constexpr std::array dir_vec{
  std::pair{1, 0},   // '>'
  std::pair{0, 1},   // 'v'
  std::pair{-1, 0},  // '<'
  std::pair{0, -1},  // '^'
};

inline constexpr auto to_idx       = [](auto&& t_idx) { return static_cast<std::size_t>(t_idx - 1); };
inline constexpr auto is_in_map    = [](auto const t_chr) { return t_chr != ' '; };
inline constexpr auto get_col_proj = [](std::size_t const t_x) {
  return [x = to_idx(t_x)](auto&& t_row) { return x >= t_row.size() ? ' ' : t_row[x]; };
};

void move(std::vector<std::string>& t_map, std::pair<std::size_t, std::size_t>& t_pos, int t_move, std::size_t& t_dir,
          auto&& t_check_wrap_around) {
  using ranges::views::reverse, ranges::find_if, ranges::distance;
  int curr_x = static_cast<int>(t_pos.first);
  int curr_y = static_cast<int>(t_pos.second);

  for (int i = 0; i < t_move; ++i) {
    auto const [inc_x, inc_y] = dir_vec[t_dir];

    auto prev_dir         = t_dir;
    auto x_tmp            = curr_x + inc_x;
    auto y_tmp            = curr_y + inc_y;
    auto&& [mov_x, mov_y] = t_check_wrap_around(t_map, curr_x, curr_y, x_tmp, y_tmp, t_dir);

    if (t_map[to_idx(mov_y)][to_idx(mov_x)] == '#') {
      // check wrap around won't check for obstacles, direction will still be changed even if we can't go there in this
      // function, so we need to change it to previous direction
      t_dir = prev_dir;
      break;
    }

    curr_x = static_cast<int>(mov_x);
    curr_y = static_cast<int>(mov_y);

    t_map[to_idx(curr_y)][to_idx(curr_x)] = dir_char[t_dir];
  }

  t_pos.first  = static_cast<std::size_t>(curr_x);
  t_pos.second = static_cast<std::size_t>(curr_y);
}

inline auto const part1_wraparound = [](auto const& t_map, auto /**/, auto /**/, auto t_x_pos, auto t_y_pos,
                                        auto& t_dir) {
  using ranges::views::reverse, ranges::find_if;

  std::pair<std::size_t, std::size_t> ret_val{t_x_pos, t_y_pos};

  auto const x_pos = static_cast<std::size_t>(t_x_pos);
  auto const y_pos = static_cast<std::size_t>(t_y_pos);

  auto const col_proj = get_col_proj(x_pos);
  if (t_dir == 1 and (y_pos >= t_map.size() + 1 or x_pos >= t_map[to_idx(y_pos)].size() + 1)) {
    ret_val.second = static_cast<std::size_t>(distance(t_map.begin(), find_if(t_map, is_in_map, col_proj)) + 1);
  } else if (t_dir == 3 and (y_pos == 0 or t_map[to_idx(y_pos)][to_idx(x_pos)] == ' ')) {
    auto res       = find_if(t_map | reverse, is_in_map, col_proj).base();
    ret_val.second = static_cast<std::size_t>(distance(t_map.begin(), res));
  } else if (t_dir == 0 and (x_pos >= t_map[to_idx(y_pos)].size() + 1)) {
    auto res      = distance(t_map[to_idx(y_pos)].begin(), find_if(t_map[to_idx(y_pos)], is_in_map));
    ret_val.first = static_cast<std::size_t>(res + 1);
  } else if (t_dir == 2 and (x_pos == 0 or t_map[to_idx(y_pos)][to_idx(x_pos)] == ' ')) {
    auto res      = find_if(t_map[to_idx(y_pos)] | reverse, is_in_map).base();
    ret_val.first = static_cast<std::size_t>(distance(t_map[to_idx(y_pos)].begin(), res));
  }

  return ret_val;
};

void part1() {
  using ranges::getlines, ranges::to_vector;

  std::fstream in((INPUT_FILE));
  auto const rng          = getlines(in) | to_vector;
  auto const& instruction = rng.back();
  auto map                = std::vector(rng.begin(), rng.end() - 2);

  auto curr_pos = std::pair{map.front().find('.') + 1, 1UL};

  std::size_t dir           = 0;
  auto const* const end_ptr = std::next(instruction.data(), static_cast<long>(instruction.size()));
  for (auto const* ptr = instruction.data(); ptr != end_ptr;) {
    int steps = 0;
    if (auto [non_matching_pos, ec]{ranges_from_chars(std::string_view(ptr, end_ptr), steps)};
        ec == std::errc::invalid_argument) {  // R or L
      if (*ptr == 'R') {
        dir = (dir + 1) % 4;
      } else {
        dir = (dir + 3) % 4;  // dir = 0 -> dir - 1 = 3; dir = 1 -> dir - 1 = 0
      }
      ptr = std::next(ptr);
    } else if (ec == std::errc()) {
      ptr = non_matching_pos.data();
    }

    move(map, curr_pos, steps, dir, part1_wraparound);
  }

  auto result = 1000 * curr_pos.second + 4 * curr_pos.first + dir;
  fmt::println("result: {}", result);
}

auto find_length_of_cube(std::vector<std::string> const& t_map) {
  using ranges::max;
  return std::gcd(t_map.size(), max(t_map, std::less<>{}, [](auto&& t_str) { return t_str.size(); }).size());
}

auto cube_labeller(int const t_size) {
  return [=](int const t_x, int const t_y) { return std::pair<int, int>{t_x / t_size, t_y / t_size}; };
}

void part2() {
  using ranges::getlines, ranges::to_vector;

  std::fstream in((INPUT_FILE));
  auto const rng          = getlines(in) | to_vector;
  auto const& instruction = rng.back();
  auto map                = std::vector(rng.begin(), rng.end() - 2);

  // hardcoded right now
  //
  //  cube network          labelled
  //
  //   0 1 2  --> x          0  1  2  --> x
  // 0   . .              0  x  0  1
  // 1   .                1  x  2  x
  // 2 . .            =>  2  3  4  x
  // 3 .                  3  5  x  x
  // |                    |
  // v                    v
  // y                    y
  static constexpr std::array cube_face{std::pair{std::pair{1, 0}, 0UL}, std::pair{std::pair{2, 0}, 1UL},
                                        std::pair{std::pair{1, 1}, 2UL}, std::pair{std::pair{0, 2}, 3UL},
                                        std::pair{std::pair{1, 2}, 4UL}, std::pair{std::pair{0, 3}, 5UL}};

  auto const face_length      = static_cast<int>(find_length_of_cube(map));
  auto const part2_wraparound = [&, cube_label = cube_labeller(face_length)](auto const& /*t_map*/, auto t_curr_x,
                                                                             auto t_curr_y, auto t_x_pos, auto t_y_pos,
                                                                             auto& t_dir) {
    using ranges::find;

    if (t_y_pos >= 1 and t_x_pos >= 1) {  // e.g. -1 / 50 = 0 instead of -1 => coor (-1, -1) has cube_label (0, 0)
      auto const current_face = cube_label(t_x_pos - 1, t_y_pos - 1);
      if (auto const res = find(cube_face, current_face, [](auto&& t_pair) { return t_pair.first; });
          res != cube_face.end()) {
        return std::pair{static_cast<std::size_t>(t_x_pos), static_cast<std::size_t>(t_y_pos)};
      }
    }

    // Falls out of edge
    //  - create face connection (coordinate transform) and direction change mapping (hardcoded atm), from example above
    //    e.g:  {1, 0} -> {2, 0} -> nothing changes (in 2D point of view)
    //                 -> {1, 1} -> nothing changes (in 2D point of view)
    //                 -> {0, 2} \__
    //                 -> {0, 3} /   need extra care
    //
    //    as one can see, those that are connected before folding, don't need to consider direction change even if they
    //    do change direction (from 3D point of view), this helps us reduce cases from 12 (total edges) to 7 (-5 because
    //    there are 5 internally connected edges for any kind of cube network)
    //
    //    what we should take care of, are those edges that are not connected before folding. This is slightly more
    //    difficult, the main difficulty is to tell which edge connects to which, once we have edge relationship,
    //    direction change is trivial, which is always facing "inward" of that face:
    //
    //    e.g. from the example, 3 and 5 have connected edge, if you want to leave 3 (and go to 5), you must have the
    //         direction "->", to enter 5, you need to face inward, according to the edge, it is "down". If you are at
    //         "A" facing right, then you will go to "B" facing down
    //
    //                            x
    //            ...#        x x 0 x
    //            .#..   => x 1 2 3 x
    //            #...        x x 4 5 x
    //            ....            x x
    //    ...#.......#
    //    ........#..A     (x means falls out of edge)
    //    ..#....#....
    //    .D........#.
    //            ...#..B.
    //            .....#..
    //            .#......
    //            ..C...#.
    //
    //    label edge for each face and map it to out direction:
    //
    //    +00+    0 + 4n (n % 4 == 0): up
    //    1  2    1 + 4n (n % 4 == 1): left
    //    1  2    2 + 4n (n % 4 == 2): right
    //    +33+    3 + 4n (n % 4 == 3): down
    //
    //    In order to work with multiple cube network, we would like to determine edge connection programatically. I'll
    //    hardcode this atm, and slowly work my way toward it.

    // NOTE: hardcode value only works for input, not for example

    // t_dir       : 0 -> right, 1 -> down, 2 -> left, 3 -> up
    // out_edge_idx: 2 -> right, 3 -> down, 1 -> left, 0 -> up
    // in_edge_idx : 1 -> right, 0 -> down, 2 -> left, 3 -> up
    static constexpr std::array out_dir_to_edge_idx{2UL, 3UL, 1UL, 0UL};
    static constexpr std::array edge_to_in_dir{1UL, 0UL, 2UL, 3UL};
    static constexpr std::array edge_connection{
      //  edge    0      1    2    3
      std::array{21UL, 13UL, 5UL, 8UL},   // face 0
      std::array{23UL, 2UL, 18UL, 10UL},  // face 1
      std::array{3UL, 12UL, 7UL, 16UL},   // face 2
      std::array{9UL, 1UL, 17UL, 20UL},   // face 3
      std::array{11UL, 14UL, 6UL, 22UL},  // face 4
      std::array{15UL, 0UL, 19UL, 4UL},   // face 5
    };

    auto const prev_face      = cube_label(t_curr_x - 1, t_curr_y - 1);
    auto const from_face_info = find(cube_face, prev_face, [](auto&& t_pair) { return t_pair.first; });
    auto const from_face      = from_face_info->second;
    auto const to_edge        = edge_connection[from_face][out_dir_to_edge_idx[t_dir]];
    auto const to_face        = to_edge / 4;
    auto const to_face_info   = find(cube_face, to_face, [](auto&& t_pair) { return t_pair.second; });
    auto const new_face       = to_face_info->first;

    // now we know the direction and face we will be at when we fall out of the edge, next step is to determine the
    // position. knowing that the "relative position" to the upper/left corner of the face should not change,
    // observing the pattern of edge connection (represent using edge index):
    //
    //                          edge pair   direction  relative position to the corner under 2D coor
    //                          ---------   ---------  ---------------------------------------------
    //  internal connection:    (3  ->  8)    (d, d)             not changed
    //                          (2  ->  5)    (l, l)             not changed
    //
    //  external connection:    (0  -> 21)    (u, l)             not changed
    //                          (7  -> 10)    (d, r)             not changed
    //                          (9  -> 12)    (l, d)             not changed
    //                          (6  -> 18)    (r, l)    side length - prev relative position to the corner
    //                          (1  -> 13)    (l, r)    side length - prev relative position to the corner
    //                          (4  -> 23)    (u, u)             not changed
    //
    // as one can see, relative position to the corner under 2D coor will not change if (1) the direction before and
    // after entering are the same or (2) the angle of the direction before and after entering is 90 degree. In other
    // word, if the angle of the direction before and after entering are 180 degree, then the coordinate transform is
    // needed. This happens when the edge indices are the same (6 % 4 == 18 % 4 == 2, 1 % 4 == 13 % 4 == 1).

    auto const get_edge_origin = [=](auto const t_face_idx, auto const t_edge_idx) {
      auto face_origin = std::pair{t_face_idx.first * face_length + 1, t_face_idx.second * face_length + 1};
      switch (t_edge_idx) {
        case 0:
          [[fallthrough]];
        case 1:
          return face_origin;
        case 2:
          // face_x_end - face_x_start + 1 = face_length
          return face_origin.first += (face_length - 1), face_origin;
        case 3:
          return face_origin.second += (face_length - 1), face_origin;
        default:
          std::unreachable();
      }
    };

    auto const prev_edge_origin    = get_edge_origin(prev_face, out_dir_to_edge_idx[t_dir]);
    auto const edge_dist_to_corner = std::max(t_curr_x - prev_edge_origin.first, t_curr_y - prev_edge_origin.second);
    auto const to_edge_idx         = to_edge % 4;
    auto const new_edge_origin     = get_edge_origin(new_face, to_edge_idx);
    auto const same_edge           = to_edge_idx == out_dir_to_edge_idx[t_dir];

    t_dir = edge_to_in_dir[to_edge_idx];  // change direction here
    if (to_edge_idx == 0 or to_edge_idx == 3) {
      auto const x = same_edge ? static_cast<std::size_t>(new_edge_origin.first + face_length - 1 - edge_dist_to_corner)
                               : static_cast<std::size_t>(new_edge_origin.first + edge_dist_to_corner);

      return std::pair{x, static_cast<std::size_t>(new_edge_origin.second)};
    }

    auto const y = same_edge ? static_cast<std::size_t>(new_edge_origin.second + face_length - 1 - edge_dist_to_corner)
                             : static_cast<std::size_t>(new_edge_origin.second + edge_dist_to_corner);
    return std::pair{static_cast<std::size_t>(new_edge_origin.first), y};
  };

  // map[0][0] <-> (1, 1)
  auto curr_pos             = std::pair{map.front().find('.') + 1, 1UL};
  std::size_t dir           = 0;
  auto const* const end_ptr = std::next(instruction.data(), static_cast<long>(instruction.size()));

  for (auto const* ptr = instruction.data(); ptr != end_ptr;) {
    int steps = 0;
    if (auto [non_matching_pos, ec]{ranges_from_chars(std::string_view(ptr, end_ptr), steps)};
        ec == std::errc::invalid_argument) {  // R or L
      if (*ptr == 'R') {
        dir = (dir + 1) % 4;
      } else {
        dir = (dir + 3) % 4;  // dir = 0 -> dir - 1 = 3; dir = 1 -> dir - 1 = 0
      }
      ptr = std::next(ptr);
    } else if (ec == std::errc()) {
      ptr = non_matching_pos.data();
    }

    move(map, curr_pos, steps, dir, part2_wraparound);
  }

  // for (auto&& row : map) {
  //   fmt::println("{}", row);
  // }

  auto result = 1000 * curr_pos.second + 4 * curr_pos.first + dir;
  fmt::println("result: {}", result);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
