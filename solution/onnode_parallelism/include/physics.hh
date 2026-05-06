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

void initialize(flecsi::exec::accelerator,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::wo, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::wo> prg_a,
  const gaussianIC & ic) noexcept;

void initialize_rhs(flecsi::exec::accelerator,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::wo, flecsi::na> rhs_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const state::params & p) noexcept;

void apply_dirichlet(flecsi::exec::accelerator,
  mesh::accessor<flecsi::ro> m,
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

template<bool RED>
inline double
red_black(flecsi::exec::accelerator s,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::rw, flecsi::rw> u_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::ro> rhs_a,
  const state::params & p) noexcept {

  auto u = m.mdspan<mesh::vertices>(u_a);
  auto rhs = m.mdspan<mesh::vertices>(rhs_a);

  const double cdx2 = 1.0 / (p.dx() * p.dx());
  const double cdy2 = 1.0 / (p.dy() * p.dy());
  const double aE = p.alpha * p.dt * cdx2;
  const double aW = aE;
  const double aN = p.alpha * p.dt * cdy2;
  const double aS = aN;
  const double aP = 1.0 + 2.0 * (aE + aN);

  return s.executor().reduceall(
    j, mc, m.vertices<mesh::y_axis>(), flecsi::exec::fold::max, double) {
    for(auto i : red_black_range<RED>(m, j)) {

      const double accum = rhs[j][i] + aE * u[j][i + 1] + aW * u[j][i - 1] +
                           aN * u[j + 1][i] + aS * u[j - 1][i];

      const double u_star = accum / aP;
      const double u_old = u[j][i];
      const double u_v =
        (1.0 - p.jacobi_omega) * u_old + p.jacobi_omega * u_star;

      u[j][i] = u_v;
      const double diff = std::fabs(u_v - u_old);
      mc(diff);
    }
  };
}

double analytical_gaussian(double x,
  double y,
  double t,
  double alpha,
  double xc,
  double yc,
  double beta);

std::pair<double, double> compute_error(flecsi::exec::accelerator,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::rw, flecsi::rw> u_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::ro> rhs_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const state::params & p) noexcept;

double residual(flecsi::exec::accelerator,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::rw, flecsi::rw> u_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::ro> rhs_a,
  const state::params & p) noexcept;

} // namespace heat::physics

#endif // HEAT_PHYSICS_HH
