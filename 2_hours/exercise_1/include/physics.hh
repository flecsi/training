#ifndef HEAT_PHYSICS_HH
#define HEAT_PHYSICS_HH

#include "include/state.hh"

namespace heat::physics {

struct pair_sum {
  using pair = std::pair<double, double>;
  static pair combine(pair p1, pair p2) {
    p1.first += p2.first;
    p1.second += p2.second;
    return p1;
  }
  template<typename>
  static constexpr pair identity = std::make_pair(0, 0);
};

// =============================================================================
// Exercise 1 — Declare initialize as a FleCSI task
// =============================================================================
// See the plain C++ version below.
// Look at initialize.cc to see how it is called and what arguments are passed.
// The implementation goes in physics.cc.
// =============================================================================

void initialize(state & s, const gaussianIC & ic);

// =============================================================================

void initialize_rhs(mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::wo, flecsi::na> rhs_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const state::params & p) noexcept;

void apply_dirichlet(mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::rw, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const boundary & bc) noexcept;

template<bool RED>
auto
red_black_range(mesh::accessor<flecsi::ro> m, const int & j) {
  if constexpr(RED)
    return m.red<mesh::x_axis>(j);
  else
    return m.black<mesh::x_axis>(j);
}

// =============================================================================
// Exercise 1 — Declare and implement red_black as a FleCSI task
// =============================================================================
// Look at advance.cc to see how it is called as a FleCSI reduction task and
// what arguments are passed. Here is the main part:
//
//    flecsi::future<double> mr, mb, res;
//
//    [...]
//    mr = sc.reduce<physics::red_black<true>, flecsi::exec::fold::sum>(
//        *s.cur.m, s.cur.u(*s.cur.m), s.cur.rhs(*s.cur.m), s.par);
//    [...]
//    mb = sc.reduce<physics::red_black<false>, flecsi::exec::fold::sum>(
//        *s.cur.m, s.cur.u(*s.cur.m), s.cur.rhs(*s.cur.m), s.par);
//
// The function template signature to fill in:
//   template<bool RED>
//   inline double red_black(/* TODO */ ) noexcept { /* TODO */ }
// =============================================================================

// Original version
template<bool RED>
double
red_black(state & s) {
  double max_change = 0.0;

  const double cdx2 = 1.0 / (s.par.dx() * s.par.dx());
  const double cdy2 = 1.0 / (s.par.dy() * s.par.dy());
  const double aE = s.par.alpha * s.par.dt * cdx2;
  const double aW = aE;
  const double aN = s.par.alpha * s.par.dt * cdy2;
  const double aS = aN;
  const double aP = 1.0 + 2.0 * (aE + aN);

  for(int j = 1; j < s.cur.m.Ny - 1; ++j) {
    for(int i = red_black_range<RED>(j); i < s.cur.m.Nx - 1; i += 2) {

      const int id = s.cur.m.idx(i, j);
      const double accum = s.cur.rhs[id] + aE * s.cur.u[s.cur.m.idx(i + 1, j)] +
                           aW * s.cur.u[s.cur.m.idx(i - 1, j)] +
                           aN * s.cur.u[s.cur.m.idx(i, j + 1)] +
                           aS * s.cur.u[s.cur.m.idx(i, j - 1)];

      const double u_star = accum / aP;
      const double u_old = s.cur.u[id];
      const double u_new =
        (1.0 - s.par.jacobi_omega) * u_old + s.par.jacobi_omega * u_star;

      s.cur.u[id] = u_new;
      const double diff = std::fabs(u_new - u_old);
      if(diff > max_change)
        max_change = diff;
    }
  }
  return max_change;
}

// =============================================================================

double analytical_gaussian(double x,
  double y,
  double t,
  double alpha,
  double xc,
  double yc,
  double beta);

std::pair<double, double> compute_error(mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::rw, flecsi::rw> u_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::ro> rhs_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const state::params & p) noexcept;

double residual(mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::rw, flecsi::rw> u_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::ro> rhs_a,
  const state::params & p) noexcept;

} // namespace heat::physics

#endif // HEAT_PHYSICS_HH
