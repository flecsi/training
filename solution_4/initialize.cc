#include "include/io.hh"
#include "include/physics.hh"
#include "spec/control.hh"

namespace heat {

static void
initialize(spec::control_policy &cp) {

  auto &s = cp.state();
  auto &sc = cp.scheduler();

  allocate(s, sc);

  gaussianIC ic{0.5 * s.par.Lx, 0.5 * s.par.Ly, 200.0};

  sc.execute<physics::initialize>(
    *s.cur.m, s.cur.u(*s.cur.m), s.cur.prg(*s.cur.idx), ic);

  io::output_print(s, sc, "u_");
}

} // namespace heat

const control::action<heat::initialize, spec::cp::initialize> initialize_action;
