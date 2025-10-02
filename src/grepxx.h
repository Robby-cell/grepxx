#ifndef GREPXX_GREPXX_H
#define GREPXX_GREPXX_H

#include <cstdio>
#include <memory>
#include <print>
#include <utility>

#include "terminal.h"

namespace grepxx {

inline namespace options {
struct Options;
}

class OutputLocation {
public:
  virtual ~OutputLocation() = default;
  virtual std::FILE *match_stream() const = 0;
  virtual std::FILE *dbg_stream() const = 0;
  virtual bool is_printing_to_terminal() const = 0;

protected:
  OutputLocation() = default;

private:
  OutputLocation(const OutputLocation &) = delete;
  void operator=(const OutputLocation &) = delete;
  OutputLocation(OutputLocation &&) = delete;
  void operator=(OutputLocation &&) = delete;
};

class TerminalOutput : public OutputLocation {
public:
  TerminalOutput() = default;
  ~TerminalOutput() override = default;
  std::FILE *match_stream() const override { return stdout; }
  std::FILE *dbg_stream() const override { return stderr; }
  bool is_printing_to_terminal() const override { return !is_piped(); }
};

class FileOutput : public OutputLocation {
  struct Deleter {
    inline void operator()(std::FILE *fp) { std::fclose(fp); }
  };
  using ManagedFile = std::unique_ptr<std::FILE, Deleter>;

public:
  FileOutput(ManagedFile file) : file_(std::move(file)) {}
  std::FILE *match_stream() const override { return file_.get(); }
  std::FILE *dbg_stream() const override { return nullptr; }
  bool is_printing_to_terminal() const override { return false; }

private:
  ManagedFile file_;
};

class Output {
public:
  constexpr Output(OutputLocation *output) : output_(output) {}

  template <int &...ExplicitBarrier, class... Args>
  void print_match(std::format_string<Args...> format, Args &&...args) const {
    std::print(output_->match_stream(), format, std::forward<Args>(args)...);
  }

  template <int &...ExplicitBarrier, class... Args>
  void print_dbg(std::format_string<Args...> format, Args &&...args) const {
    const auto fp = output_->dbg_stream();
    if (fp) {
      std::print(fp, format, std::forward<Args>(args)...);
    }
  }

  inline bool is_printing_to_terminal() const {
    return output_->is_printing_to_terminal();
  }

private:
  OutputLocation *output_;
};

void grep(Output output, options::Options &opts);

} // namespace grepxx

#endif // GREPXX_GREPXX_H
