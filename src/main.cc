#include "include/heat.hh"
#include "include/io.hh"

using namespace heat;

int
simulation(const char *f) {
  state s(io::read_file(f));

  initialize(s);
  while(s.cur.prg.t < s.par.t_final) {
    advance(s);
    analyze(s);
  }
  finalize(s);
  return 0;
}

int
main() {
  return simulation("(input file)");
}
