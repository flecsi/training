#include "include/io.hh"
#include "spec/control.hh"
#include <flecsi/runtime.hh>

int
main() {
  const flecsi::run::dependencies_guard dg;
  flecsi::runtime run;
  flecsi::flog::add_output_stream("clog", std::clog, true);
  return run.control<spec::control>("(input-file)");
}
