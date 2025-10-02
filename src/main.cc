#include "grepxx.h"
#include "options.h"
#include <exception>
#include <iostream>

int main(int argc, char *argv[]) try {
  auto options = grepxx::Options::from_args(argc, argv);
  grepxx::TerminalOutput output;
  grepxx::grep(&output, options);
} catch (const std::exception &e) {
  std::cerr << "Error: " << e.what() << std::endl;
}
