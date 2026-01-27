#include "include/heat.hh"
#include "include/io.hh"
#include "include/physics.hh"
#include <flecsi/flog.hh>
#include <iostream>

namespace heat {

struct red_error {
  using pair = std::pair<double, double>;
  static pair combine(pair p1, pair p2) {
    p1.first += p2.first;
    p1.second += p2.second;
    return p1;
  }
  template<typename>
  static constexpr pair identity = std::make_pair(0, 0);
};

void
finalize(spec::control_policy &cp) {

  auto &s = cp.state();
  auto &sc = cp.scheduler();

  auto err = sc.reduce<physics::compute_error, red_error>(flecsi::exec::on,
                 *s.cur.m,
                 s.cur.u(*s.cur.m),
                 s.cur.rhs(*s.cur.m),
                 s.cur.prg(*s.cur.idx),
                 s.par)
               .get();
  flog(info) << "final relative L2 error=" << std::sqrt(err.first / err.second)
             << std::endl;
  io::output_print(s, sc, "u_");
}

} // namespace heat

const control::action<heat::finalize, spec::cp::finalize> finalize_action;
