#include <algorithm>
#include <cstddef>
#include <ctre.hpp>
#include <fmt/format.h>
#include <fstream>
#include <map>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <utility>
#include <vector>

struct Blueprint {
  int idx_;
  int ore_bot_cost_;
  int clay_bot_cost_;
  std::pair<int, int> obsidian_bot_cost_;
  std::pair<int, int> geode_bot_cost_;

  int max_ore_cost_;
};

struct OreCount {
  int ore_      = 0;
  int clay_     = 0;
  int obsidian_ = 0;
  int geode_    = 0;

  bool operator==(OreCount const&) const noexcept = default;

  bool operator<(OreCount const& t_ore) const noexcept {
    if (this->geode_ != t_ore.geode_) {
      return this->geode_ < t_ore.geode_;
    }

    if (this->obsidian_ != t_ore.obsidian_) {
      return this->obsidian_ < t_ore.obsidian_;
    }

    if (this->clay_ != t_ore.clay_) {
      return this->clay_ < t_ore.clay_;
    }

    return this->ore_ < t_ore.ore_;
  }

  auto operator+(OreCount const& t_ore) const noexcept {
    return OreCount{this->ore_ + t_ore.ore_, this->clay_ + t_ore.clay_, this->obsidian_ + t_ore.obsidian_,
                    this->geode_ + t_ore.geode_};
  }

  auto operator+=(OreCount const& t_ore) noexcept {
    this->ore_ += t_ore.ore_;
    this->clay_ += t_ore.clay_;
    this->obsidian_ += t_ore.obsidian_;
    this->geode_ += t_ore.geode_;
    return *this;
  }
};

template <std::size_t Time>
using Buffer = std::array<std::pair<OreCount, OreCount>, Time + 1>;

auto to_blueprint(std::string const& t_str) {
  auto const result = ctre::match<
    "Blueprint ([0-9]+):"
    "[a-zA-Z ]+([0-9]+)[a-zA-Z ]+."                    // ore robot
    "[a-zA-Z ]+([0-9]+)[a-zA-Z ]+."                    // clay robot
    "[a-zA-Z ]+([0-9]+)[a-zA-Z ]+([0-9]+)[a-zA-Z ]+."  // obsidian robot
    "[a-zA-Z ]+([0-9]+)[a-zA-Z ]+([0-9]+)[a-zA-Z ]+."  // geode robot
    >(t_str);                                          //

  auto const idx                 = std::stoi((result.get<1>().to_string()));
  auto const ore_bot_cost        = std::stoi(result.get<2>().to_string());
  auto const clay_bot_cost       = std::stoi(result.get<3>().to_string());
  auto const obsidian_ore_cost   = std::stoi(result.get<4>().to_string());
  auto const obsidian_clay_cost  = std::stoi(result.get<5>().to_string());
  auto const geode_ore_cost      = std::stoi(result.get<6>().to_string());
  auto const geode_obsidian_cost = std::stoi(result.get<7>().to_string());

  return Blueprint{.idx_               = idx,
                   .ore_bot_cost_      = ore_bot_cost,
                   .clay_bot_cost_     = clay_bot_cost,
                   .obsidian_bot_cost_ = std::pair{obsidian_ore_cost, obsidian_clay_cost},
                   .geode_bot_cost_    = std::pair{geode_ore_cost, geode_obsidian_cost},
                   .max_ore_cost_      = std::max({ore_bot_cost, clay_bot_cost, obsidian_ore_cost, geode_ore_cost})};
};

template <std::size_t T>
void run_mining_sim(Blueprint const& t_bp, OreCount& t_bot_count, Buffer<T>& t_mining_state, int const t_time,
                    OreCount const& t_result) {
  if (t_time == 0) {
    if (auto& [bot_count_at_time, ore_count_at_time] = t_mining_state.back(); ore_count_at_time < t_result) {
      bot_count_at_time = t_bot_count;
      ore_count_at_time = t_result;
    }

    return;
  }

  auto& [bot_count_at_time, ore_count_at_time] = t_mining_state[T - static_cast<std::size_t>(t_time)];
  if (bot_count_at_time.geode_ > t_bot_count.geode_ and ore_count_at_time.geode_ >= t_result.geode_) {
    return;
  }

  if (bot_count_at_time == t_bot_count and t_result < ore_count_at_time) {
    return;
  }

  bot_count_at_time = t_bot_count;
  ore_count_at_time = t_result;

  // we can only build one bot a time (face palm :P)
  if (auto const [geode_ore_cost, geode_obsidian_cost] = t_bp.geode_bot_cost_;
      t_result.ore_ >= geode_ore_cost and t_result.obsidian_ >= geode_obsidian_cost) {
    OreCount new_result = t_result + t_bot_count;
    new_result.ore_ -= geode_ore_cost;
    new_result.obsidian_ -= geode_obsidian_cost;

    OreCount new_bot_count = t_bot_count;
    ++new_bot_count.geode_;
    run_mining_sim<T>(t_bp, new_bot_count, t_mining_state, t_time - 1, new_result);

    return;  // since we are dfs-ing, if this happens in one branch, it must be the first to happen, then we don't need
             // to search for the rest because we are aiming to make geode bot ASAP
  }

  if (auto const [obsidian_ore_cost, obsidian_clay_cost] = t_bp.obsidian_bot_cost_;
      t_result.ore_ >= obsidian_ore_cost and t_result.clay_ >= obsidian_clay_cost and
      t_bot_count.obsidian_ < t_bp.geode_bot_cost_.second) {
    OreCount new_result = t_result + t_bot_count;
    new_result.ore_ -= obsidian_ore_cost;
    new_result.clay_ -= obsidian_clay_cost;

    OreCount new_bot_count = t_bot_count;
    ++new_bot_count.obsidian_;
    run_mining_sim<T>(t_bp, new_bot_count, t_mining_state, t_time - 1, new_result);
  }

  if (t_result.ore_ >= t_bp.clay_bot_cost_ and t_bot_count.clay_ < t_bp.obsidian_bot_cost_.second) {
    OreCount new_result = t_result + t_bot_count;
    new_result.ore_ -= t_bp.clay_bot_cost_;

    OreCount new_bot_count = t_bot_count;
    ++new_bot_count.clay_;
    run_mining_sim<T>(t_bp, new_bot_count, t_mining_state, t_time - 1, new_result);
  }

  if (t_result.ore_ >= t_bp.ore_bot_cost_ and t_bot_count.ore_ < t_bp.max_ore_cost_) {
    OreCount new_result = t_result + t_bot_count;
    new_result.ore_ -= t_bp.ore_bot_cost_;

    OreCount new_bot_count = t_bot_count;
    ++new_bot_count.ore_;
    run_mining_sim<T>(t_bp, new_bot_count, t_mining_state, t_time - 1, new_result);
  }

  run_mining_sim<T>(t_bp, t_bot_count, t_mining_state, t_time - 1, t_result + t_bot_count);
}

void part1() {
  using ranges::getlines, ranges::views::transform, ranges::to_vector, ranges::max;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | transform(to_blueprint) | to_vector;

  int quality_sum = 0;
  for (auto&& bp : rng) {
    OreCount bot_count{1};

    Buffer<24> dp;
    run_mining_sim<24>(bp, bot_count, dp, 24, OreCount{});

    quality_sum += bp.idx_ * dp[24].second.geode_;

    fmt::print("{}: {}\n", bp.idx_, dp[24].second.geode_);
  }

  fmt::print("quality sum: {}\n", quality_sum);
}

void part2() {
  using ranges::getlines, ranges::views::transform, ranges::to_vector, ranges::views::take;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | transform(to_blueprint) | to_vector;

  int result = 1;
  for (auto&& bp : rng | take(3)) {
    OreCount bot_count{1};

    Buffer<32> dp;
    run_mining_sim<32>(bp, bot_count, dp, 32, OreCount{});

    result *= dp[32].second.geode_;

    fmt::print("result: {}\n", dp[32].second.geode_);
  }

  fmt::print("quality sum: {}\n", result);
}

int main(int /**/, char** /**/) {
  // part1();
  part2();

  return EXIT_SUCCESS;
}
