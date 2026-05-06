#include "include/io.hh"
#include "spec/control.hh"

namespace heat {

static void
analyze(spec::control_policy & cp) {
  auto & s = cp.state();

  static int count = 0;
  if(++count % s.par.output_every == 0)
    io::output_print(s, cp.scheduler(), "u_");
}

} // namespace heat

const spec::control::action<heat::analyze, spec::cp::analyze> analyze_action;
