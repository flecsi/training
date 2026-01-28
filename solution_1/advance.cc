#include "include/heat.hh"
#include "include/physics.hh"

namespace heat {

bool
cycle_check(state &s) {
  s.cur.prg.t += s.par.dt;
  s.cur.prg.step += 1;
  return s.cur.prg.t < s.par.t_final;
}

static bool
check_loop(const state &s, double mr, double mb, double res) {
  return !(
    (std::max(mb, mr) < s.par.implicit_tol) && (res < s.par.residue_tol));
}

void
advance(state &s) {

  using namespace physics;

  apply_dirichlet(s);
  initialize_rhs(s);

  double mr, mb, res;

  do {

    mr = red_black<true>(s);
    apply_dirichlet(s);

    mb = red_black<false>(s);
    apply_dirichlet(s);

    res = residual(s);

  } while(check_loop(s, mr, mb, res));
}

} // namespace heat
