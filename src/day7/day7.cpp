#include "split_helper.hpp"

#include <charconv>
#include <cstddef>
#include <fmt/format.h>
#include <fstream>
#include <limits>
#include <memory>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/split_when.hpp>
#include <string>
#include <vector>

struct Node {
  std::string name_ = "/";
  Node* parent_     = nullptr;
  std::size_t size_ = 0;
  std::vector<std::unique_ptr<Node>> children_{};

  Node() = default;
  Node(std::string t_dir_name, Node* t_parent) : name_(std::move(t_dir_name)), parent_(t_parent) {}

  [[nodiscard]] bool is_dir() const noexcept { return not this->children_.empty(); }

  [[nodiscard]] bool is_file() const noexcept { return this->children_.empty(); }

  void walk_node(auto&& t_func) const noexcept {
    t_func(this);

    for (auto&& node : this->children_) {
      node->walk_node(t_func);
    }
  }
};

struct FileSystem {
 private:
  Node root_dir_;
  Node* current_dir_ = &root_dir_;

 public:
  FileSystem() = default;

  void cd(std::string_view t_path) noexcept {
    using ranges::find_if;

    if (t_path == "..") {
      this->current_dir_ = this->current_dir_->parent_;
    } else if (t_path == "/") {
      this->current_dir_ = &this->root_dir_;
    } else {
      // assume that the cd command will always go to dir that is shown in ls command previously
      auto iter          = find_if(current_dir_->children_, [=](auto&& t_dir) { return t_dir->name_ == t_path; });
      this->current_dir_ = iter->get();
    }
  }

  // assume the input doesn't go to same Node and ls, we just blindly append files and dirs
  void parse_ls_output(auto&& t_outputs) {
    auto* cwd = this->current_dir_;
    for (auto&& output : t_outputs) {
      auto const content = split_string(output);
      auto& node         = cwd->children_.emplace_back(std::make_unique<Node>(content[1], cwd));
      if (content[0] != "dir") {
        std::size_t result = 0;
        std::from_chars(content[0].data(), content[0].data() + content[0].length(), result);

        node->size_ = result;
        for (auto* dir = cwd; dir != nullptr; dir = dir->parent_) {
          dir->size_ += result;
        }
      }
    }
  }

  [[nodiscard]] auto get_used_disk_space() const noexcept { return this->root_dir_.size_; }

  [[nodiscard]] std::size_t size_ceil(std::size_t const t_size) const noexcept {
    std::size_t ret_val = std::numeric_limits<std::size_t>::max();

    this->root_dir_.walk_node([&](Node const* const t_n) {
      if (t_n->is_dir() and t_n->size_ >= t_size and t_n->size_ < ret_val) {
        ret_val = t_n->size_;
      }
    });

    return ret_val;
  }

  [[nodiscard]] std::size_t get_dir_size_sum_within_size(std::size_t const t_size) const noexcept {
    std::size_t ret_val = 0;

    this->root_dir_.walk_node([&](Node const* const t_n) {
      if (t_n->size_ <= t_size and t_n->is_dir()) {
        ret_val += t_n->size_;
      }
    });

    return ret_val;
  }
};

FileSystem parse_file_system() {
  using ranges::getlines, ranges::to_vector, ranges::views::split_when, ranges::views::drop, ranges::views::split;

  std::ifstream command_output((INPUT_FILE));
  auto rng = getlines(command_output) | to_vector;

  FileSystem fs;

  auto const group_cmd_and_result = [](auto&& t_str, auto&& /**/) { return std::pair{t_str->front() == '$', t_str}; };

  for (auto cmd_and_output : rng | split_when(group_cmd_and_result)) {
    auto cmd = split_string(*begin(cmd_and_output));

    if (cmd[1] == "cd") {
      fs.cd(cmd[2]);
    } else if (cmd[1] == "ls") {
      fs.parse_ls_output(cmd_and_output | drop(1));
    }
  }

  return fs;
}

void part1() {
  auto fs = parse_file_system();

  fmt::print("total size: {}\n", fs.get_dir_size_sum_within_size(100'000));
}

void part2() {
  static constexpr auto TOTAL_DISK_SPACE    = 70'000'000;
  static constexpr auto UNUSED_SPACE_NEEDED = 30'000'000;

  auto const fs = parse_file_system();

  auto const used_disk_space = fs.get_used_disk_space();
  auto const space_needed    = used_disk_space - (TOTAL_DISK_SPACE - UNUSED_SPACE_NEEDED);

  fmt::print("space needed: {}\n", space_needed);
  fmt::print("size of dir to delete: {}\n", fs.size_ceil(space_needed));
}

int main(int /*unused*/, char** /*unused*/) {
  part1();
  part2();
  return EXIT_SUCCESS;
}