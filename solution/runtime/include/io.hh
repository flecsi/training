#ifndef HEAT_IO_HH
#define HEAT_IO_HH

#include "include/state.hh"
#include <string>

namespace heat::io {

void write_csv(const state & s, const std::string & fname);

void print_diag(const state & s);

void output_print(const state & s, std::string prefix);

inline state::params
read_file(const char *) {
  return {};
}

} // namespace heat::io

#endif // HEAT_IO_HH
