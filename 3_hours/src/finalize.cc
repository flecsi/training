#include "include/heat.hh"
#include "include/io.hh"
#include "include/physics.hh"
#include <iostream>

namespace heat {

void
finalize(const state & s) {

  double err = physics::compute_error(s, 0.5 * s.par.Lx, 0.5 * s.par.Ly);
  std::cout << "t=" << s.cur.prg.t << "  relative L2 error=" << err << '\n';
  io::output_print(s, "u_");
}

} // namespace heat
