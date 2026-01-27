#include "control.hh"

namespace spec {

bool
control_policy::cycle_control(control_policy &cp) {

  auto &s = cp.state();

  s.cur.prg.t += s.par.dt;
  s.cur.prg.step += 1;

  return s.cur.prg.t < s.par.t_final;
} // cycle_control

} // namespace spec
