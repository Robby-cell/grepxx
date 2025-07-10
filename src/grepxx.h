#ifndef GREPXX_GREPXX_H
#define GREPXX_GREPXX_H

#include <ostream>

namespace grepxx {

inline namespace options {
struct Options;
}

void grep(std::ostream &os, options::Options &opts);

} // namespace grepxx

#endif // GREPXX_GREPXX_H
