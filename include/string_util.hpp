#include <charconv>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/split.hpp>
#include <range/v3/view/transform.hpp>
#include <ranges>
#include <string>
#include <vector>

[[nodiscard]] inline auto split_string(std::string const& t_str, char t_delim = ' ') {
  using ranges::views::transform, ranges::views::split, ranges::to_vector, ranges::distance;
  auto ret_val = t_str | split(t_delim) | transform([](auto&& t_rng) {
                   return std::string{&*t_rng.begin(), static_cast<std::size_t>(distance(t_rng))};
                 }) |
                 to_vector;

  return ret_val;
}

[[nodiscard]] inline auto split_string(std::string_view t_str, char t_delim = ' ') {
  using ranges::views::transform, ranges::views::split, ranges::to_vector, ranges::distance;
  auto ret_val = t_str | split(t_delim) | transform([](auto&& t_rng) {
                   return std::string_view{&*t_rng.begin(), static_cast<std::size_t>(distance(t_rng))};
                 }) |
                 to_vector;

  return ret_val;
}

template <typename StrType>
  requires(std::ranges::contiguous_range<StrType>)
inline constexpr auto ranges_from_chars(StrType&& t_rng, int& t_res, int const t_base = 10) {
  if constexpr (ranges::enable_view<std::remove_cvref_t<StrType>>) {
    auto const* const first = std::ranges::data(t_rng);
    auto const* const last  = first + std::ranges::size(t_rng);
    auto const [ptr, err]   = std::from_chars(first, last, t_res, t_base);
    return std::make_pair(std::span(ptr, last), err);
  } else {
    auto const* const first = std::ranges::data(std::forward<StrType>(t_rng));
    auto const* const last  = first + std::ranges::size(std::forward<StrType>(t_rng));
    auto const [ptr, err]   = std::from_chars(first, last, t_res, t_base);
    return std::make_pair(std::span(ptr, last), err);
  }
}
