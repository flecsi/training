#include "include/io.hh"
#include "include/physics.hh"
#include "spec/control.hh"
#include <flecsi/flog.hh>
#include <iostream>

namespace heat {

void
finalize(spec::control_policy &cp) {

  auto &s = cp.state();
  auto &sc = cp.scheduler();

  auto err = sc.reduce<physics::compute_error, physics::pair_sum>(*s.cur.m,
                 s.cur.u(*s.cur.m),
                 s.cur.rhs(*s.cur.m),
                 s.cur.prg(*s.cur.idx),
                 s.par)
               .get();
  flog(info) << "final relative L2 error=" << std::sqrt(err.first / err.second)
             << std::endl;
  io::output_print(s, sc, "u_");
}

} // namespace heat

const control::action<heat::finalize, spec::cp::finalize> finalize_action;
