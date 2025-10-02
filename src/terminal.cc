#include "terminal.h"

#ifdef _WIN32
#include <io.h>
#include <stdio.h>
#define IS_TTY _isatty
#define STDOUT_FILENO _fileno(stdout)
#else
#include <unistd.h>
#define IS_TTY isatty
#endif

namespace grepxx {
bool is_piped() { return !IS_TTY(STDOUT_FILENO); }
} // namespace grepxx
