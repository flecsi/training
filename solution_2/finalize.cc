#include "include/io.hh"
#include "include/physics.hh"
#include "spec/control.hh"
#include <flecsi/flog.hh>
#include <iostream>

namespace heat {

void
finalize(spec::control_policy &cp) {

  const auto &s = cp.state();

  double err = physics::compute_error(s, 0.5 * s.par.Lx, 0.5 * s.par.Ly);
  flog(info) << "t=" << s.cur.prg.t << "  relative L2 error=" << err << '\n';
  io::output_print(s, "u_");
}

} // namespace heat

const control::action<heat::finalize, spec::cp::finalize> finalize_action;
