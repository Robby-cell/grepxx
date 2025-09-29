#include "grepxx.h"

#include "options.h"

#include <cstddef>
#include <optional>
#include <regex>
#include <string>
#include <string_view>

namespace grepxx {

using std::operator""sv;

constexpr inline auto AnsiBlue = "\e[0;34m"sv;
constexpr inline auto AnsiGreen = "\e[0;32m"sv;
constexpr inline auto AnsiReset = "\033[0m"sv;
constexpr inline auto AnsiRed = "\033[31m"sv;

namespace {
size_t grep(std::ostream &os, std::istream &is,
          const options::Options::Config &config,
          std::optional<std::string_view> header = std::nullopt) {
  std::size_t line_no = 1;

  auto flags = std::regex_constants::ECMAScript;
  if (config.case_insensitive) {
    flags |= std::regex_constants::icase;
  }
  std::regex searcher{config.pattern, flags};

  bool printed_header = false;
  size_t matches = 0;

  for (std::string line; std::getline(is, line); line.clear(), ++line_no) {
    if (std::regex_search(line, searcher)) {
      if (!matches) {
        if (header) {
          os << AnsiBlue << *header << AnsiReset << '\n';
        }
      }
 
      os << AnsiGreen << line_no << AnsiReset << ": ";

      /* ---- print line with matches in red ---- */
      std::sregex_iterator it(line.begin(), line.end(), searcher);
      std::sregex_iterator end;

      std::size_t last = 0; // where the previous match ended
      for (; it != end; ++it) {
        const std::smatch &m = *it;
        os.write(line.data() + last, m.position() - last); // text before match
        os << AnsiRed << m.str() << AnsiReset;             // the match itself
        last = m.position() + m.length();
      }
      os.write(line.data() + last, line.size() - last); // tail of the line
      os << '\n';
      /* ---------------------------------------- */
      ++matches;
    }
  }
  return matches;
}
} // namespace

void grep(std::ostream &os, options::Options &opts) {
  std::size_t i = 0;
  const auto stream_count = opts.streams.size();
  for (auto &stream : opts.streams) {
    auto &is = *stream.stream;
    const auto matches = grep(os, is, opts.config,
         (stream.name.has_value()
              ? std::optional<std::string_view>(*stream.name)
              : std::nullopt));

    if (++i < stream_count && matches) {
      os << '\n';
    }
  }
}

} // namespace grepxx
