#include "include/heat.hh"
#include "include/io.hh"
#include "include/physics.hh"

namespace heat {

void
initialize(spec::control_policy &cp) {

  auto &s = cp.state();
  auto &sc = cp.scheduler();

  allocate(s, sc);

  gaussianIC ic{0.5 * s.par.Lx, 0.5 * s.par.Ly, 200.0};
  physics::initialize(s, ic);

  io::output_print(s, "u_");
}

} // namespace heat

inline control::action<heat::initialize, spec::cp::initialize>
  initialize_action;
