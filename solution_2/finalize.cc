#include "include/heat.hh"
#include "include/io.hh"
#include "include/physics.hh"
#include <flecsi/execution.hh>
#include <flecsi/flog.hh>
#include <iostream>

namespace heat {

void
finalize(spec::control_policy &cp) {

  const auto &s = cp.state();

  double err = physics::compute_error(s, 0.5 * s.par.Lx, 0.5 * s.par.Ly);
  flog(info) << "t=" << s.cur.prg.t << "  relative L2 error=" << err << '\n';
  flecsi::flog::flush();
  io::output_print(s, "u_");
}

} // namespace heat

inline control::action<heat::finalize, spec::cp::finalize> finalize_action;
