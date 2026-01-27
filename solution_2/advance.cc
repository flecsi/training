#include "include/physics.hh"
#include "spec/control.hh"

namespace heat {

static bool
check_loop(const state &s, double mr, double mb, double res) {
  return !(
    (std::max(mb, mr) < s.par.implicit_tol) && (res < s.par.residue_tol));
}

static void
advance(spec::control_policy &cp) {

  using namespace physics;
  auto &s = cp.state();

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

const control::action<heat::advance, spec::cp::advance> advance_action;
