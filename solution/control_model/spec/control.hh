#ifndef SPEC_CONTROL_HH
#define SPEC_CONTROL_HH

#include "include/io.hh"
#include <flecsi/execution.hh>
#include <flecsi/flog.hh>
#include <flecsi/run/control.hh>

#include <fstream>

namespace spec {

bool cycle_check(heat::state & s);

/// Control Points.
enum class cp { initialize, advance, analyze, finalize };

inline const char *
operator*(cp control_point) {
  switch(control_point) {
    case cp::initialize:
      return "initialize";
    case cp::advance:
      return "advance";
    case cp::analyze:
      return "analyze";
    case cp::finalize:
      return "finalize";
  }
  flog_fatal("invalid control point");
}

struct control_policy : flecsi::run::control_base {

  using control_points_enum = cp;

  control_policy(const char * c) : state_(heat::io::read_file(c)) {}

  heat::state & state() {
    return state_;
  }

  static bool cycle_control(control_policy & cp) {
    return cycle_check(cp.state());
  }

  using control_points = list<point<cp::initialize>,
    cycle<cycle_control, point<cp::advance>, point<cp::analyze>>,
    point<cp::finalize>>;

private:
  heat::state state_;
}; // struct control_policy

using control = flecsi::run::control<spec::control_policy>;

} // namespace spec

#endif // CONTROL_HH
