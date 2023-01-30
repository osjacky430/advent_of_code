#include <cstddef>
#include <ctre.hpp>
#include <fmt/format.h>
#include <fstream>
#include <map>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/getlines.hpp>
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
};

struct OreCount {
  int ore_      = 0;
  int clay_     = 0;
  int obsidian_ = 0;
  int geode_    = 0;

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

  auto operator+=(OreCount const& t_ore) const noexcept {
    this->ore_ += t_ore.ore_;
    this->clay_ += t_ore.clay_;
    this->obsidian_ += t_ore.obsidian_;
    this->geode_ += t_ore.geode_;
    return *this;
  }
};

struct MiningState {
  OreCount bot_;
  int time_remain_;
};

auto to_blueprint(std::string const& t_str) {
  auto const result = ctre::match<
    "Blueprint ([0-9]+):"
    "[a-zA-Z ]+([0-9]+)[a-zA-Z ]+."                    // ore robot
    "[a-zA-Z ]+([0-9]+)[a-zA-Z ]+."                    // clay robot
    "[a-zA-Z ]+([0-9]+)[a-zA-Z ]+([0-9]+)[a-zA-Z ]+."  // obsidian robot
    "[a-zA-Z ]+([0-9]+)[a-zA-Z ]+([0-9]+)[a-zA-Z ]+."  // geode robot
    >(t_str);                                          //

  auto const idx                 = result.get<1>().to_string();
  auto const ore_bot_cost        = result.get<2>().to_string();
  auto const clay_bot_cost       = result.get<3>().to_string();
  auto const obsidian_ore_cost   = result.get<4>().to_string();
  auto const obsidian_clay_cost  = result.get<5>().to_string();
  auto const geode_ore_cost      = result.get<6>().to_string();
  auto const geode_obsidian_cost = result.get<7>().to_string();

  return Blueprint{.idx_               = std::stoi(idx),
                   .ore_bot_cost_      = std::stoi(ore_bot_cost),
                   .clay_bot_cost_     = std::stoi(clay_bot_cost),
                   .obsidian_bot_cost_ = std::pair{std::stoi(obsidian_ore_cost), std::stoi(obsidian_clay_cost)},
                   .geode_bot_cost_    = std::pair{std::stoi(geode_ore_cost), std::stoi(geode_obsidian_cost)}};
};

void run_mining_sim(Blueprint const& t_bp, OreCount& t_bot_count, std::map<MiningState, OreCount>& t_mining_state,
                    int const t_time, OreCount& t_result) {
  auto const mine_state        = MiningState{t_bot_count, t_time};
  t_mining_sate.at(mine_state) = std::max(t_mining_state[mine_state], t_result);

  // do nothing, only mining
  // std::vector<std::pair<OreCount, OreCount>> possible_comb{{t_result + t_bot_count, t_bot_count}};
}

void part1() {
  using ranges::getlines, ranges::views::transform, ranges::to_vector, ranges::accumulate;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | transform(to_blueprint) | to_vector;

  int quality_sum = 0;
  for (auto&& bp : rng) {
    OreCount bot_count{};
    OreCount result{};
    std::map<MiningState, OreCount> dp;
    run_mining_sim(bp, bot_count, dp, 24, result);
    quality_sum += bp.idx_ * result;
  }

  fmt::print("quality sum: {}", qualiy_sum);
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
