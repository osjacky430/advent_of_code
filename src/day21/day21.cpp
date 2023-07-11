#include "string_util.hpp"
#include <fmt/format.h>
#include <fstream>
#include <map>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/transform.hpp>

inline std::map<char, double (*)(double, double)> const operation{
  {'+', [](double l, double r) { return l + r; }},
  {'-', [](double l, double r) { return l - r; }},
  {'*', [](double l, double r) { return l * r; }},
  {'/', [](double l, double r) { return l / r; }},
};

void eval(std::map<std::string, double>& t_evaluated, std::map<std::string, std::vector<std::string>>& t_tokens,
          auto&& t_predicate) {
  while (t_predicate()) {
    for (auto&& [name, expr] : t_tokens) {
      if (t_evaluated.contains(name)) {
        continue;
      }

      if (expr.size() == 1) {
        t_evaluated.emplace(name, std::stod(expr[0]));
      } else {
        auto&& [lhs, rhs]{std::pair{expr[0], expr[2]}};

        if (t_evaluated.contains(expr[0]) and t_evaluated.contains(expr[2])) {
          t_evaluated.emplace(name, operation.at(expr[1][0])(t_evaluated.at(lhs), t_evaluated.at(rhs)));
        }
      }
    }
  }
}

void part1() {
  using ranges::getlines, ranges::views::transform, ranges::to;
  using namespace std::string_literals;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | transform([](auto&& t_str) {
               auto const delimiter = t_str.find(':');
               auto name            = t_str.substr(0, delimiter);
               auto expr            = split_string(t_str.substr(delimiter + 2));
               return std::pair{name, expr};
             }) |
             to<std::map>;

  std::map<std::string, double> evaluated;
  eval(evaluated, rng, [&]() { return not evaluated.contains("root"s); });

  fmt::println("root = {}", evaluated.at("root"s));
}

auto newton_ralphson(std::map<std::string, std::vector<std::string>>& t_tokens, double t_guess,
                     double t_epsilon = 1e-6) {
  using namespace std::string_literals;

  auto const& root_expr = t_tokens.at("root"s);

  std::map<std::string, double> pre_eval;  // pre eval those that are not dependent of "humn"
  eval(pre_eval, t_tokens, [&, size = 1UL]() mutable {
    bool const size_changed = pre_eval.size() == size;

    size = pre_eval.size();
    return not size_changed;
  });

  auto const f = [&](double t_v) {
    auto evaluated            = pre_eval;
    auto const root_evaluated = [&]() { return not evaluated.contains("root"s); };

    evaluated["humn"s] = t_v;

    eval(evaluated, t_tokens, root_evaluated);
    return evaluated.at(root_expr[0]) - evaluated.at(root_expr[2]);
  };

  static constexpr auto dx = 0.01;
  while (true) {
    double f_x1      = f(t_guess);
    auto const slope = (f(t_guess + dx) - f_x1) / dx;
    // fmt::println("iteration result: f({}) = {}, f'({}) ~= {}", t_guess, f_x1, t_guess, slope);

    if (std::abs(f_x1) < t_epsilon) {
      break;
    }

    t_guess -= f_x1 / slope;
  }

  return t_guess;
}

void part2() {
  using ranges::getlines, ranges::views::transform, ranges::to, ranges::views::filter;
  using namespace std::string_literals;

  std::fstream in((INPUT_FILE));
  auto rng = getlines(in) | filter([](auto&& t_str) { return not t_str.starts_with("humn"); }) |
             transform([](auto&& t_str) {
               auto const delimiter = t_str.find(':');
               auto name            = t_str.substr(0, delimiter);
               auto expr            = split_string(t_str.substr(delimiter + 2));
               return std::pair{name, expr};
             }) |
             to<std::map>;

  auto const result = newton_ralphson(rng, 1);
  fmt::println("I should yell: {}!", result);
}

int main(int /**/, char** /**/) {
  part1();
  part2();

  return EXIT_SUCCESS;
}
