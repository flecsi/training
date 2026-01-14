#include "include/heat.hh"
#include "include/io.hh"
#include <flecsi/runtime.hh>

using namespace heat;

int
main() {
  const flecsi::run::dependencies_guard dg;
  flecsi::runtime run;
  flecsi::flog::add_output_stream("clog", std::clog, true);
  return run.control<control>("(input-file)");
}
