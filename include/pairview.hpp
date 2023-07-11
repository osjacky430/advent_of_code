#ifndef PAIRVIEW_HPP_
#define PAIRVIEW_HPP_

#include <concepts>
#include <cstddef>
#include <functional>
#include <range/v3/all.hpp>
#include <range/v3/functional/bind_back.hpp>
#include <range/v3/utility/semiregular_box.hpp>
#include <range/v3/view/view.hpp>
#include <type_traits>
#include <utility>

namespace ranges::views::detail {

struct indirect_pair_fn {
  template <typename It>
    requires std::indirectly_readable<It>
  std::pair<std::iter_reference_t<It>, std::iter_reference_t<It>> operator()(It const& t_first, It const& t_sec) const {
    return {*t_first, *t_sec};
  }
};

}  // namespace ranges::views::detail

namespace ranges::views {

// TODO: const version
template <typename Rng>
struct pairview : ranges::view_facade<pairview<Rng>> {
 private:
  friend struct ranges::range_access;

  ranges::views::all_t<Rng> container_;

  struct cursor {
    ranges::semiregular_box_t<detail::indirect_pair_fn> fun_{};
    std::pair<ranges::iterator_t<Rng>, ranges::iterator_t<Rng>> iter_;
    using rng_value_t = std::iter_reference_t<ranges::iterator_t<Rng>>;
    using contiguous  = std::true_type;

    cursor() = default;
    cursor(ranges::iterator_t<Rng> t_first, ranges::iterator_t<Rng> t_second) : iter_{t_first, t_second} {}

    [[nodiscard]] auto read() const noexcept { return std::apply(this->fun_, this->iter_); }

    [[nodiscard]] bool equal(cursor const& t_that) const { return this->iter_ == t_that.iter_; }

    void next() {
      ++this->iter_.first;
      ++this->iter_.second;
    }

    void prev() {
      --this->iter_.first;
      --this->iter.second;
    }

    [[nodiscard]] std::ptrdiff_t distance_to(cursor const& t_that) const {
      return t_that.iter_.first - this->iter_.first;
    }

    void advance(std::ptrdiff_t const t_n) {
      this->iter_.first += t_n;
      this->iter_.second += t_n;
    }
  };

  [[nodiscard]] cursor begin_cursor() const {
    return {ranges::begin(this->container_), ranges::begin(this->container_) + 1};
  }

  [[nodiscard]] cursor end_cursor() const { return {ranges::end(this->container_) - 1, ranges::end(this->container_)}; }

 public:
  explicit pairview(Rng&& t_v) : container_(ranges::views::all(std::forward<Rng>(t_v))) {}

  using ranges::view_facade<pairview<Rng>>::begin;
  using ranges::view_facade<pairview<Rng>>::end;
};

template <typename Rng>
pairview(Rng&&) -> pairview<ranges::views::all_t<Rng>>;

struct pairview_fn {
  template <typename Rng>
  constexpr auto operator()(Rng&& t_rng) const {
    return pairview(std::forward<Rng>(t_rng));
  }
};

inline constexpr view_closure<pairview_fn> pairviewer;

}  // namespace ranges::views

template <typename Rng>
inline constexpr bool ranges::enable_borrowed_range<ranges::views::pairview<Rng>> = true;

#endif