#include "control.hh"

namespace spec {

bool
control_policy::cycle_control(control_policy &cp) {
  auto &s = cp.state();
  auto &sc = cp.scheduler();

  auto t = sc.execute<update_dt>(s.cur.prg(*s.cur.idx), s.par);

  return t.all()[0] < s.par.t_final;
} // cycle_control

} // namespace spec
