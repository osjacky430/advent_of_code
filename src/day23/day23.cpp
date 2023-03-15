#include <cmath>
#include <cstddef>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <list>
#include <map>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/minmax.hpp>
#include <range/v3/algorithm/none_of.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/transform.hpp>
#include <set>

struct Elf;

using Coor         = std::pair<int, int>;
using DirPred      = std::pair<bool (*)(std::set<Coor> const&, Elf const&), Coor (*)(Coor)>;
using DirPredOrder = std::list<DirPred>;

struct Elf {
  bool conflicted_proposal_ = false;
  bool have_neighbor_       = false;
  DirPred predicate_        = {nullptr, nullptr};
  Coor coor_;

  Coor get_coor() const noexcept { return this->coor_; }

  bool operator==(Elf const& t_elf) const noexcept { return this->coor_ == t_elf.coor_; }

  Elf(Coor t_coor) : coor_(t_coor) {}
};

template <char Dir>
struct Adjacent {
  auto operator()(Coor const& t_to_check) const noexcept {
    //    N
    //  W + E
    //    S

    auto&& [x_check, y_check] = t_to_check;
    if constexpr (Dir == 'N') {
      return std::vector{Coor{x_check - 1, y_check - 1}, Coor{x_check, y_check - 1}, Coor{x_check + 1, y_check - 1}};
    } else if constexpr (Dir == 'S') {
      return std::vector{Coor{x_check - 1, y_check + 1}, Coor{x_check, y_check + 1}, Coor{x_check + 1, y_check + 1}};
    } else if constexpr (Dir == 'W') {
      return std::vector{Coor{x_check - 1, y_check - 1}, Coor{x_check - 1, y_check}, Coor{x_check - 1, y_check + 1}};
    } else if constexpr (Dir == 'E') {
      return std::vector{Coor{x_check + 1, y_check - 1}, Coor{x_check + 1, y_check}, Coor{x_check + 1, y_check + 1}};
    }
  }
};

inline constexpr std::array<DirPred, 4> MOVE_ORDER = {
  DirPred{[](std::set<Coor> const& t_elves, Elf const& t_elf_to_check) {
            return ranges::none_of(Adjacent<'N'>{}(t_elf_to_check.coor_),
                                   [&](auto&& t_coor) { return t_elves.contains(t_coor); });
          },
          [](Coor t_v) { return --t_v.second, t_v; }},  // abomination
  DirPred{[](std::set<Coor> const& t_elves, Elf const& t_elf_to_check) {
            return ranges::none_of(Adjacent<'S'>{}(t_elf_to_check.coor_),
                                   [&](auto&& t_coor) { return t_elves.contains(t_coor); });
          },
          [](Coor t_v) { return ++t_v.second, t_v; }},
  DirPred{[](std::set<Coor> const& t_elves, Elf const& t_elf_to_check) {
            return ranges::none_of(Adjacent<'W'>{}(t_elf_to_check.coor_),
                                   [&](auto&& t_coor) { return t_elves.contains(t_coor); });
          },
          [](Coor t_v) { return --t_v.first, t_v; }},
  DirPred{[](std::set<Coor> const& t_elves, Elf const& t_elf_to_check) {
            return ranges::none_of(Adjacent<'E'>{}(t_elf_to_check.coor_),
                                   [&](auto&& t_coor) { return t_elves.contains(t_coor); });
          },
          [](Coor t_v) { return ++t_v.first, t_v; }},
};

void debug_print(std::vector<Elf> const& t_elves, std::size_t const t_round) {
  using ranges::minmax;

  auto&& [min_x, max_x] = minmax(t_elves, std::less<>{}, [](auto const& t_elf) { return t_elf.get_coor().first; });
  auto&& [min_y, max_y] = minmax(t_elves, std::less<>{}, [](auto const& t_elf) { return t_elf.get_coor().second; });

  auto const x_size = static_cast<std::size_t>(max_x.coor_.first - min_x.coor_.first + 1);
  auto const y_size = static_cast<std::size_t>(max_y.coor_.second - min_y.coor_.second + 1);

  std::vector<std::string> map(y_size, std::string(x_size, '.'));
  for (auto&& elf : t_elves) {
    auto&& [elf_x, elf_y] = elf.get_coor();
    auto const x_idx      = static_cast<std::size_t>(elf_x - min_x.coor_.first);
    auto const y_idx      = static_cast<std::size_t>(elf_y - min_y.coor_.second);

    map[y_idx][x_idx] = '#';
  }

  for (auto&& row : map) {
    fmt::print("{}\n", row);
  }

  fmt::print("== End of Round {} == \n", t_round);
}

void move(std::vector<Elf>& t_elves, auto&& t_direction_predicate) {
  using ranges::find_if, ranges::views::transform, ranges::to;

  auto const elf_position = t_elves | transform(&Elf::get_coor) | to<std::set>;
  std::map<Coor, Elf&> proposed_position;

  for (auto&& elf : t_elves) {
    auto const have_neighbor = [&](auto&& t_pred) { return not(*t_pred.first)(elf_position, elf); };

    elf.conflicted_proposal_ = false;
    auto const move_result   = [&]() {
      // iteration starts with first considered direction, if first considered direction is invalid (have_neighbor
      // returns true), then continue until first considered direction is false, which means find_if returns value other
      // than first considered direction (second "if"), or one pass last consider direction (first "if").
      for (auto considered_dir = t_direction_predicate;
           static_cast<std::size_t>(considered_dir - t_direction_predicate) < MOVE_ORDER.size(); ++considered_dir) {
        auto const consider_dir_end  = considered_dir + MOVE_ORDER.size();
        auto const first_invalid_dir = find_if(considered_dir, consider_dir_end, have_neighbor);
        if (first_invalid_dir == considered_dir + MOVE_ORDER.size()) {  // no neighbor
          return DirPred{nullptr, nullptr};
        }

        if (first_invalid_dir != considered_dir) {
          return *considered_dir;
        }
      }

      return DirPred{nullptr, nullptr};  // neighbor everywhere
    }();

    elf.predicate_          = move_result;
    auto&& [checker, mover] = move_result;
    if (checker == nullptr and mover == nullptr) {
      continue;
    }

    if (auto coor = (*mover)(elf.coor_); not proposed_position.contains(coor)) {
      proposed_position.emplace(coor, elf);
    } else {
      auto& same_proposal_elf                = proposed_position.at(coor);
      same_proposal_elf.conflicted_proposal_ = true;
      elf.conflicted_proposal_               = true;
    }
  }

  for (auto&& elf : t_elves) {
    if (auto&& [_, mover] = elf.predicate_; not elf.conflicted_proposal_ and _ != nullptr and mover != nullptr) {
      elf.coor_ = (*mover)(elf.coor_);
    }
  }
}

auto get_elves_coor_from_map(auto&& t_map) {
  std::vector<Elf> ret_val;

  ret_val.reserve(t_map.size() * t_map[0].size());
  for (std::size_t i = 0; i < t_map.size(); ++i) {
    auto& row = t_map[i];
    for (std::size_t j = 0; j < t_map[i].size(); ++j) {
      if (row[j] == '#') {
        ret_val.emplace_back(std::pair{static_cast<int>(j), static_cast<int>(i)});
      }
    }
  }

  return ret_val;
}

void part1() {
  using ranges::getlines, ranges::to_vector, ranges::minmax, ranges::views::cycle;

  std::fstream in((INPUT_FILE));
  auto const rng = getlines(in) | to_vector;

  auto elves           = get_elves_coor_from_map(rng);
  auto direction_cycle = cycle(MOVE_ORDER);
  auto iter            = begin(direction_cycle);
  for (std::size_t i = 0; i < 10; ++i) {
    move(elves, iter);
    ++iter;
  }

  auto&& [min_x, max_x] = minmax(elves, std::less<>{}, [](auto const& t_elf) { return t_elf.get_coor().first; });
  auto&& [min_y, max_y] = minmax(elves, std::less<>{}, [](auto const& t_elf) { return t_elf.get_coor().second; });

  auto const total_tile = (max_x.coor_.first - min_x.coor_.first + 1) * (max_y.coor_.second - min_y.coor_.second + 1);
  fmt::print("min x: {}, max x: {}, min y: {}, max y: {}\n", min_x.coor_.first, max_x.coor_.first, min_y.coor_.second,
             max_y.coor_.second);
  fmt::print("empty ground tiles: {}\n", total_tile - static_cast<int>(elves.size()));
}

void part2() {
  using ranges::getlines, ranges::to_vector, ranges::views::cycle;

  std::fstream in((INPUT_FILE));
  auto const rng = getlines(in) | to_vector;

  auto elves           = get_elves_coor_from_map(rng);
  auto direction_cycle = cycle(MOVE_ORDER);
  auto iter            = begin(direction_cycle);

  std::vector<Elf> prev_result = elves;
  for (std::size_t round = 0;; prev_result = elves, ++iter) {
    move(elves, iter);
    ++round;

    if (prev_result == elves) {
      fmt::print("After {} of rounds, no elves moves for the first time\n", round);
      break;
    }
  }
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
