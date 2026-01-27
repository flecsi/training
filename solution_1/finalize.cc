#include "include/heat.hh"
#include "include/io.hh"
#include "include/physics.hh"
#include <flecsi/flog.hh>
#include <iostream>

namespace heat {

void
finalize(const state &s) {

  double err = physics::compute_error(s, 0.5 * s.par.Lx, 0.5 * s.par.Ly);
  flog(info) << "t=" << s.cur.prg.t << "  relative L2 error=" << err << '\n';
  flecsi::flog::flush();
  io::output_print(s, "u_");
}

} // namespace heat
