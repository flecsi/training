#include "include/heat.hh"
#include "include/io.hh"
#include <flecsi/runtime.hh>

using namespace heat;

int
simulation(const char *f) {
  state s(io::read_file(f));

  initialize(s);
  while(cycle_check(s)) {
    advance(s);
    analyze(s);
  }
  finalize(s);
  return 0;
}

int
main() {
  const flecsi::run::dependencies_guard dg;
  flecsi::runtime run;
  flecsi::flog::add_output_stream("clog", std::clog, true);
  return run.control<flecsi::run::call>(
    [f = "(input_file)"]() { return simulation(f); });
}
