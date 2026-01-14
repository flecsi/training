#include "include/heat.hh"
#include "include/io.hh"

namespace heat {

void
analyse(spec::control_policy &cp) {
  const auto &s = cp.state();
  if(s.cur.prg.step % s.par.output_every == 0)
    io::output_print(s, "u_");
}

} // namespace heat

inline control::action<heat::analyse, spec::cp::analyze> analyze_action;
