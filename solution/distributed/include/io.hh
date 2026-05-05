#ifndef HEAT_IO_HH
#define HEAT_IO_HH

#include "include/state.hh"
#include <string>

namespace heat::io {

void write_csv(flecsi::exec::cpu s,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const std::string & prefix) noexcept;

std::pair<double, double> print_diag(
  flecsi::field<double>::accessor<flecsi::ro, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a) noexcept;

void output_print(state & s, flecsi::scheduler & sc, std::string prefix);

inline state::params
read_file(const char *) {
  return {};
}

} // namespace heat::io

#endif // HEAT_IO_HH
