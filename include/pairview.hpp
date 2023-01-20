#ifndef PAIRVIEW_HPP_
#define PAIRVIEW_HPP_

#include <cstddef>
#include <range/v3/detail/range_access.hpp>
#include <range/v3/view/facade.hpp>
#include <type_traits>

namespace range_helper {

// TODO: const version
template <typename T>
struct pairview : ranges::view_facade<pairview<T>> {
 private:
  friend struct ranges::range_access;

  T& container_;

  struct cursor {
    std::pair<typename T::iterator, typename T::iterator> iter_;
    using contiguous = std::true_type;

    cursor() = default;
    cursor(T::iterator t_first, T::iterator t_second) : iter_{t_first, t_second} {}

    std::pair<typename T::iterator, typename T::iterator> const& read() const { return this->iter_; }

    bool equal(cursor const& t_that) const { return this->iter_ == t_that.iter_; }

    void next() {
      ++this->iter_.first;
      ++this->iter_.second;
    }

    void prev() {
      --this->iter_.first;
      --this->iter.second;
    }

    std::ptrdiff_t distance_to(cursor const& t_that) const { return t_that.iter_.first - this->iter_.first; }

    void advance(std::ptrdiff_t const t_n) {
      this->iter_.first += t_n;
      this->iter_.second += t_n;
    }
  };

  cursor begin_cursor() const { return {begin(this->container_), begin(this->container_) + 1}; }
  cursor end_cursor() const { return {end(this->container_) - 1, end(this->container_)}; }

 public:
  pairview(T& t_v) : container_(t_v) {}
};

}  // namespace range_helper

#endif