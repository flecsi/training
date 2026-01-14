#include "include/heat.hh"
#include "include/io.hh"

namespace heat {

void
analyse(spec::control_policy &cp) {
  auto &s = cp.state();

  // Replace with predicated task
  // if(s.c.step % s.p.output_every == 0)

  static int count = 0;
  if(++count % s.par.output_every == 0)
    io::output_print(s, cp.scheduler(), "u_");
}

} // namespace heat

inline control::action<heat::analyse, spec::cp::analyze> analyze_action;
