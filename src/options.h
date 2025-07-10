#ifndef GREPXX_OPTIONS_H
#define GREPXX_OPTIONS_H

#include <istream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace grepxx {

inline namespace options {

struct Stream {
  std::optional<std::string> name = std::nullopt;
  std::unique_ptr<std::istream> stream;
};

struct Options {
  std::vector<Stream> streams;
  struct Config {
    std::string pattern;
    bool case_insensitive = false;
  };
  Config config;

  static Options from_args(int argc, char **argv);
};

} // namespace options

} // namespace grepxx

#endif // GREPXX_OPTIONS_H
