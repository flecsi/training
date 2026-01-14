#include "control.hh"

namespace spec {

bool
control_policy::cycle_control(control_policy &cp) {

  auto &s = cp.state();

  s.cur.prg.t += std::min(s.par.dt, s.par.t_final - s.cur.prg.t);

  if(s.cur.prg.t >= s.par.t_final) {
    flog(info) << "step: " << s.cur.prg.step << " time: " << s.cur.prg.t
               << " dt: " << s.par.dt << std::endl;
    flecsi::flog::flush();
  } // if

  ++s.cur.prg.step;
  return s.cur.prg.t < s.par.t_final;
} // cycle_control

} // namespace spec
