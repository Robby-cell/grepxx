#include "options.h"

#include <fstream>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace grepxx {

inline namespace options {

Options Options::from_args(int argc, char **argv) {
  Options options;

  std::vector<std::string_view> opts_owned{argv + 1, argv + argc};

  for (auto it = opts_owned.begin(); it != opts_owned.end();) {
    using std::operator""sv;

    const auto s = *it;

    bool remove_arg = false;
    if (s == "-i"sv) {
      options.config.case_insensitive = true;
      remove_arg = true;
    }

    if (remove_arg) {
      it = opts_owned.erase(it);
    } else {
      ++it;
    }
  }

  if (opts_owned.size() < 2) {
    throw std::runtime_error(
        "Invalid args passed. Requires a pattern, and paths to search for.");
  }

  options.config.pattern = std::string(opts_owned.front());
  options.streams.reserve(opts_owned.size() - 1);
  for (auto it = opts_owned.begin() + 1; it != opts_owned.end(); ++it) {
    std::string name{*it};
    auto file = std::make_unique<std::ifstream>(name, std::ios::binary);

    options.streams.emplace_back(std::move(name), std::move(file));
  }

  return options;
}

} // namespace options

} // namespace grepxx
