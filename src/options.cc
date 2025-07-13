#include "options.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace grepxx {

inline namespace options {

namespace fs = ::std::filesystem;

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
    } else if (s == "-r"sv) {
      options.config.recurse_dirs = true;
      remove_arg = true;
    } else if (s == "-t"sv) {
      if (std::next(it) == opts_owned.end())
        throw std::runtime_error("-t requires an argument");
      options.config.literal_text = std::string(*std::next(it));
    }

    if (remove_arg) {
      it = opts_owned.erase(it);
    } else {
      ++it;
    }
  }

  if (opts_owned.empty() && !options.config.literal_text) {
    throw std::runtime_error("No pattern supplied");
  }

  options.config.pattern = std::string(opts_owned.front());
  opts_owned.erase(opts_owned.begin());

  if (options.config.literal_text) {
    auto ss =
        std::make_unique<std::istringstream>(*options.config.literal_text);
    options.streams.emplace_back("<text>", std::move(ss));
    return options;
  }

  /* collect every file (or files under directories) */
  auto collect = [&](const fs::path &p) {
    if (fs::is_directory(p)) {
      if (!options.config.recurse_dirs)
        throw std::runtime_error("directory given without -r: " + p.string());

      for (const auto &entry : fs::recursive_directory_iterator(p)) {
        if (entry.is_regular_file()) {
          std::string name = entry.path().string();
          auto file = std::make_unique<std::ifstream>(name, std::ios::binary);
          if (!*file)
            throw std::runtime_error("cannot open: " + name);
          options.streams.emplace_back(std::move(name), std::move(file));
        }
      }
    } else {
      std::string name = p.string();
      auto file = std::make_unique<std::ifstream>(name, std::ios::binary);
      if (!*file)
        throw std::runtime_error("cannot open: " + name);
      options.streams.emplace_back(std::move(name), std::move(file));
    }
  };

  for (const auto &arg : opts_owned)
    collect(arg);

  if (options.streams.empty())
    throw std::runtime_error("no input to search");

  return options;
}

} // namespace options

} // namespace grepxx
