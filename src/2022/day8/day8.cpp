#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <functional>
#include <range/v3/algorithm/count_if.hpp>
#include <range/v3/algorithm/max.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view.hpp>
#include <range/v3/view/any_view.hpp>
#include <span>
#include <stack>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

template <>
struct std::hash<std::pair<std::size_t, std::size_t>> {
  std::size_t operator()(std::pair<std::size_t, std::size_t> const& t_pair) const noexcept {
    std::size_t h1 = std::hash<std::size_t>{}(t_pair.first);
    std::size_t h2 = std::hash<std::size_t>{}(t_pair.second);
    return h1 ^ (h2 << 1U);
  }
};

struct VisibilityChecker {
  using LineView               = ranges::any_view<std::pair<std::size_t, std::size_t>>;
  using HeightMap              = std::vector<std::vector<std::size_t>>;
  using SceneryCalculationAlgo = void (VisibilityChecker::*)(LineView, HeightMap&) const;
  using VisibleSet             = std::unordered_set<std::pair<std::size_t, std::size_t>>;

  auto count_visible_tree(auto t_eyesight, VisibleSet& t_visible_trees) const {
    using ranges::count_if, ranges::views::drop_last;

    auto max_height = [&]() {
      auto&& [x, y] = *ranges::begin(t_eyesight);
      return this->view_[y][x];
    }();

    auto const res = count_if(t_eyesight | drop_last(1), [&](auto&& t_pair) {
      auto const [x, y] = t_pair;
      if (max_height >= this->view_[y][x]) {
        return false;
      }

      max_height = this->view_[y][x];
      if (t_visible_trees.contains(std::pair{x, y})) {
        return false;
      }

      t_visible_trees.emplace(x, y);
      return true;
    });

    return static_cast<std::size_t>(res) + 1;
  }

  static inline constexpr auto TREE_HEIGHT_COUNT = 10;

  // both way of calculating scenery can be used to count visible tree (part1)
  void count_viewable_height(LineView t_eyesight, HeightMap& t_height_map) const {
    using ranges::views::enumerate;
    std::array<std::size_t, TREE_HEIGHT_COUNT> viewable_height_count{};

    for (auto const [idx, coor] : t_eyesight | enumerate) {
      auto const [x, y]               = coor;
      auto const height               = static_cast<std::size_t>(this->view_[y][x] - '0');
      auto const cannot_reach_to_edge = static_cast<std::size_t>(viewable_height_count[height] != idx);
      t_height_map[y][x] *= (viewable_height_count[height] + cannot_reach_to_edge);

      for (std::size_t i = 0; i < TREE_HEIGHT_COUNT; ++i) {
        if (i > height) {
          ++viewable_height_count[i];
        } else {
          viewable_height_count[i] = 0;
        }
      }
    }
  }

  void next_greater_element(LineView t_eyesight, HeightMap& t_height_map) const {
    using ranges::views::enumerate, ranges::distance;

    struct Coor {
      std::size_t idx_{};
      std::pair<std::size_t, std::size_t> coor_;
    };

    std::stack<Coor> monotonic;
    for (auto&& [idx, coor] : enumerate(t_eyesight)) {
      auto const [x, y]         = coor;
      auto const current_height = this->view_[y][x];

      while (not monotonic.empty()) {
        auto const [top_idx, top_coor] = monotonic.top();
        auto const [top_x, top_y]      = top_coor;
        if (this->view_[top_y][top_x] > current_height) {
          break;
        }

        monotonic.pop();
        t_height_map[top_y][top_x] *= (idx - top_idx);
      }

      monotonic.push(Coor{.idx_ = static_cast<std::size_t>(idx), .coor_ = coor});
    }

    while (not monotonic.empty()) {  // those that are still on the stack are not blocked in this eyesight
      auto const [idx, coor] = monotonic.top();
      auto const [x, y]      = coor;
      t_height_map[y][x] *= (static_cast<std::size_t>(distance(t_eyesight)) - (idx + 1));
      monotonic.pop();
    }
  }

  std::span<std::string const> view_;

  explicit VisibilityChecker(std::vector<std::string> const& t_map) : view_(t_map) {}

  [[nodiscard]] auto get_visible_tree_count() const {
    VisibleSet visible_tree;
    std::size_t ret_val = 0;

    for (std::size_t i = 1; i + 1 < this->view_.size(); ++i) {
      using ranges::views::zip, ranges::views::repeat, ranges::views::indices, ranges::views::reverse;

      auto row_idx_view = zip(indices(this->view_[i].size()), repeat(i));
      auto col_idx_view = zip(repeat(i), indices(this->view_.size()));

      ret_val += this->count_visible_tree(row_idx_view, visible_tree);
      ret_val += this->count_visible_tree(row_idx_view | reverse, visible_tree);

      ret_val += this->count_visible_tree(col_idx_view, visible_tree);
      ret_val += this->count_visible_tree(col_idx_view | reverse, visible_tree);
    }

    return ret_val + 4;
  }

  [[nodiscard]] auto get_highest_scenery_value(SceneryCalculationAlgo t_ptr) const {
    using ranges::max, ranges::views::join;

    std::vector height_map(this->view_.size(), std::vector(this->view_[0].size(), 1UL));

    for (std::size_t i = 0; i + 1 < this->view_.size(); ++i) {
      using ranges::views::zip, ranges::views::repeat, ranges::views::indices, ranges::views::reverse;

      auto row_idx_view = zip(indices(this->view_[i].size()), repeat(i));
      auto col_idx_view = zip(repeat(i), indices(this->view_.size()));

      (this->*t_ptr)(row_idx_view, height_map);
      (this->*t_ptr)(row_idx_view | reverse, height_map);
      (this->*t_ptr)(col_idx_view, height_map);
      (this->*t_ptr)(col_idx_view | reverse, height_map);
    }

    return max(height_map | join);
  }
};

void part1() {
  using ranges::getlines, ranges::to_vector;

  std::fstream map((INPUT_FILE));

  auto const map_str = getlines(map) | to_vector;
  auto const checker = VisibilityChecker{map_str};

  fmt::println("visible tree count: {}", checker.get_visible_tree_count());
}

void part2() {
  using ranges::getlines, ranges::to_vector;

  std::fstream map((INPUT_FILE));

  auto const map_str = getlines(map) | to_vector;
  auto const checker = VisibilityChecker{map_str};

  fmt::println("highest scenic score by count_viewable_height: {}",
               checker.get_highest_scenery_value(&VisibilityChecker::count_viewable_height));

  fmt::println("highest scenic score by next_greater_element: {}",
               checker.get_highest_scenery_value(&VisibilityChecker::next_greater_element));
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}