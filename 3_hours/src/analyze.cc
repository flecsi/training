#include "include/heat.hh"
#include "include/io.hh"

namespace heat {

void
analyze(const state & s) {
  if(s.cur.prg.step % s.par.output_every == 0)
    io::output_print(s, "u_");
}

} // namespace heat
