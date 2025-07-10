#include "grepxx.h"

#include "options.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace grepxx {

using std::operator""sv;

constexpr auto AnsiBlue = "\e[0;34m"sv;
constexpr auto AnsiGreen = "\e[0;32m"sv;
constexpr auto AnsiReset = "\033[0m"sv;

namespace {
void grep(std::ostream &os, std::istream &is,
          const options::Options::Config &config,
          std::optional<std::string_view> header = std::nullopt) {
  std::size_t line_no = 1;
  const std::boyer_moore_horspool_searcher searcher{
      config.pattern.begin(),
      config.pattern.end(),
      {},
      [case_insensitive = config.case_insensitive](const auto lhs,
                                                   const auto rhs) {
        if (case_insensitive) {
          return std::tolower(lhs) == std::tolower(rhs);
        } else {
          return lhs == rhs;
        }
      }};

  if (header) {
    os << AnsiBlue << *header << AnsiReset << '\n';
  }
  for (std::string line; std::getline(is, line); line.clear(), ++line_no) {
    const auto result = std::search(line.begin(), line.end(), searcher);
    if (result != line.end()) {
      os << AnsiGreen << line_no << AnsiReset << ": " << line << '\n';
    }
  }
}
} // namespace

void grep(std::ostream &os, options::Options &opts) {
  std::size_t i = 0;
  for (auto &stream : opts.streams) {
    auto &is = *stream.stream;
    grep(os, is, opts.config,
         (stream.name.has_value()
              ? std::optional<std::string_view>(*stream.name)
              : std::nullopt));

    if (++i << opts.streams.size()) {
      os << '\n';
    }
  }
}

} // namespace grepxx
