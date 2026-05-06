#include "include/physics.hh"
#include "spec/control.hh"

bool
spec::cycle_check(heat::state & s, flecsi::scheduler & sc) {
  return sc
    .reduce<spec::control_policy::update_dt, flecsi::exec::fold::sum>(
      s.cur.prg(*s.cur.idx), s.par)
    .get();
}

namespace heat {

static bool
check_loop(flecsi::future<double> mr,
  flecsi::future<double> mb,
  flecsi::future<double> res,
  const state::params & p) noexcept {
  return !((std::max(mb.get(), mr.get()) < p.implicit_tol) &&
           (res.get() < p.residue_tol));
}

static void
advance(spec::control_policy & cp) {

  using namespace physics;
  auto & s = cp.state();
  auto & sc = cp.scheduler();

  sc.execute<physics::apply_dirichlet>(flecsi::exec::on,
    *s.cur.m,
    s.cur.u(*s.cur.m),
    s.cur.prg(*s.cur.idx),
    s.par.b);

  sc.execute<physics::initialize_rhs>(flecsi::exec::on,
    *s.cur.m,
    s.cur.rhs(*s.cur.m),
    s.cur.u(*s.cur.m),
    s.cur.prg(*s.cur.idx),
    s.par);

  flecsi::future<double> mr, mb, res;

  do {

    mr = sc.reduce<physics::red_black<true>, flecsi::exec::fold::sum>(
      flecsi::exec::on,
      *s.cur.m,
      s.cur.u(*s.cur.m),
      s.cur.rhs(*s.cur.m),
      s.par);
    sc.execute<physics::apply_dirichlet>(flecsi::exec::on,
      *s.cur.m,
      s.cur.u(*s.cur.m),
      s.cur.prg(*s.cur.idx),
      s.par.b);

    mb = sc.reduce<physics::red_black<false>, flecsi::exec::fold::sum>(
      flecsi::exec::on,
      *s.cur.m,
      s.cur.u(*s.cur.m),
      s.cur.rhs(*s.cur.m),
      s.par);
    sc.execute<physics::apply_dirichlet>(flecsi::exec::on,
      *s.cur.m,
      s.cur.u(*s.cur.m),
      s.cur.prg(*s.cur.idx),
      s.par.b);

    res =
      sc.reduce<physics::residual, flecsi::exec::fold::max>(flecsi::exec::on,
        *s.cur.m,
        s.cur.u(*s.cur.m),
        s.cur.rhs(*s.cur.m),
        s.par);

  } while(
    sc.reduce<check_loop, flecsi::exec::fold::sum>(mr, mb, res, s.par).get());
}

} // namespace heat

const spec::control::action<heat::advance, spec::cp::advance> advance_action;
