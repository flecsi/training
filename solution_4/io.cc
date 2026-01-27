#include "include/io.hh"
#include <algorithm>
#include <flecsi/flog.hh>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

namespace heat::io {

void
write_csv(flecsi::exec::cpu s,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const std::string &prefix) noexcept {

  std::ostringstream oss;
  oss << prefix << std::setw(6) << std::setfill('0') << prg_a->step << '_'
      << s.launch().index << ".csv";

  std::ofstream file(oss.str());
  if(!file.is_open()) {
    flog_fatal("Error: cannot open file for writing: " << oss.str() << '\n');
  }

  file << std::fixed << std::setprecision(8);
  file << "x,y,u\n";

  auto u = m.mdspan<mesh::vertices>(u_a);
  for(auto j : m.axis<mesh::y_axis>().layout.logical()) {
    const double y = m.value<mesh::y_axis>(j);
    for(auto i : m.axis<mesh::x_axis>().layout.logical()) {
      const double x = m.value<mesh::x_axis>(i);
      file << x << ',' << y << ',' << u[j][i] << '\n';
    }
  }
}

struct minmax {
  using pair = std::pair<double, double>;
  static pair combine(pair p1, pair p2) {
    return std::make_pair(
      std::min(p1.first, p2.first), std::max(p1.second, p2.second));
  }
  template<typename>
  static constexpr pair identity = std::make_pair(1, 0);
};

std::pair<double, double>
print_diag(flecsi::exec::cpu,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a) noexcept {

  auto mm = std::minmax_element(u_a.span().begin(), u_a.span().end());
  const double umin = (mm.first != u_a.span().end()) ? *mm.first : 0.0;
  const double umax = (mm.second != u_a.span().end()) ? *mm.second : 0.0;

  flog(info) << "  step " << prg_a->step << "  t=" << std::fixed
             << std::setprecision(6) << prg_a->t << '\n';
  flecsi::flog::flush();

  return std::make_pair(umin, umax);
}

void
output_print(state &s, flecsi::scheduler &sc, std::string prefix) {

  sc.execute<write_csv>(flecsi::exec::on,
    *s.cur.m,
    s.cur.u(*s.cur.m),
    s.cur.prg(*s.cur.idx),
    prefix);
  auto uminmax = sc.reduce<print_diag, minmax>(
                     flecsi::exec::on, s.cur.u(*s.cur.m), s.cur.prg(*s.cur.idx))
                   .get();

  flog(info) << "  u[min,max]=[" << std::setprecision(6) << uminmax.first
             << ", " << uminmax.second << "]" << '\n';
  flecsi::flog::flush();
}

} // namespace heat::io
