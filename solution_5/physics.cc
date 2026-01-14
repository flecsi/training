#include "include/physics.hh"
#include <cmath>
#include <vector>

namespace heat::physics {

void
initialize(flecsi::exec::accelerator s,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::wo, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::wo> prg_a,
  const gaussianIC &ic) noexcept {
  *prg_a = {0., 0};
  auto u = m.mdspan<mesh::vertices>(u_a);
  s.executor()
    .named("initialize")
    .forall(j, m.axis<mesh::y_axis>().layout.logical()) {
    const double y = m.value<mesh::y_axis>(j);
    for(auto i : m.axis<mesh::x_axis>().layout.logical()) {
      const double x = m.value<mesh::x_axis>(i);
      u[j][i] = ic(y, x);
    }
  };
}

void
initialize_rhs(flecsi::exec::accelerator s,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::wo, flecsi::na> rhs_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const state::params &p) noexcept {
  auto u = m.mdspan<mesh::vertices>(u_a);
  auto rhs = m.mdspan<mesh::vertices>(rhs_a);
  // Build RHS b = u^n + dt * s(x,y,t^{n+1})
  s.executor()
    .named("initialize_rhs")
    .forall(j, m.axis<mesh::y_axis>().layout.logical()) {
    const double y = m.value<mesh::y_axis>(j);
    for(auto i : m.axis<mesh::x_axis>().layout.logical()) {
      const double x = m.value<mesh::x_axis>(i);
      rhs[j][i] = u[j][i] + p.dt * p.s(x, y, prg_a->t + p.dt);
    }
  };
}

// Apply Dirichlet boundary values to a target field
void
apply_dirichlet(flecsi::exec::accelerator s,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::rw, flecsi::na> u_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const boundary &bc) noexcept {
  auto u = m.mdspan<mesh::vertices>(u_a);
  const auto &t = prg_a->t;
  s.executor()
    .named("apply_dirichlet")
    .forall(j, m.axis<mesh::y_axis>().layout.logical()) {
    const double y = m.value<mesh::y_axis>(j);
    for(auto i : m.axis<mesh::x_axis>().layout.logical()) {
      const double x = m.value<mesh::x_axis>(i);
      if(y == 0)
        u[0][i] = bc.bottom(x, t);
      if(y == m.axis<mesh::y_axis>().layout.logical().size())
        u[0][i] = bc.top(x, t);
      if(x == 0)
        u[0][i] = bc.left(x, t);
      if(x == m.axis<mesh::x_axis>().layout.logical().size())
        u[0][i] = bc.right(x, t);
    }
  };
}

double
analytical_gaussian(double x,
  double y,
  double t,
  double alpha,
  double xc,
  double yc,
  double beta) {
  double denom = 1.0 + 4.0 * alpha * beta * t;
  double dx = x - xc, dy = y - yc;
  double exponent = -beta * (dx * dx + dy * dy) / denom;
  return (1.0 / denom) * std::exp(exponent);
}

struct pairmax {
  using pair = std::pair<double, double>;
  static pair combine(pair p1, pair p2) {
    return std::make_pair(
      std::max(p1.first, p2.first), std::max(p1.second, p2.second));
  }
  template<typename>
  static constexpr pair identity{-std::numeric_limits<double>::infinity(),
    -std::numeric_limits<double>::infinity()};
};

std::pair<double, double>
compute_error(flecsi::exec::accelerator s,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::rw, flecsi::rw> u_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::ro> rhs_a,
  flecsi::field<state::current::progress,
    flecsi::data::single>::accessor<flecsi::ro> prg_a,
  const state::params &p) noexcept {
  const auto &t = prg_a->t;
  auto u = m.mdspan<mesh::vertices>(u_a);

  const double xc = p.Lx / 2;
  const double yc = p.Ly / 2;

  return s.executor()
    .named("compute_error")
    .template reduce<pairmax, std::pair<double, double>>(
      m.axis<mesh::y_axis>().layout.logical(),
      FLECSI_LAMBDA(auto j, auto err_ref) {
        const double y = m.value<mesh::y_axis>(j);
        for(auto i : m.axis<mesh::x_axis>().layout.logical()) {
          const double x = m.value<mesh::x_axis>(i);
          double u_exact =
            analytical_gaussian(x, y, t, p.alpha, xc, yc, p.beta);
          double diff = u[j][i] - u_exact;
          err_ref(std::make_pair(diff * diff, u_exact * u_exact));
        }
      });
}

double
residual(flecsi::exec::accelerator s,
  mesh::accessor<flecsi::ro> m,
  flecsi::field<double>::accessor<flecsi::rw, flecsi::rw> u_a,
  flecsi::field<double>::accessor<flecsi::ro, flecsi::ro> rhs_a,
  const state::params &p) noexcept {

  auto u = m.mdspan<mesh::vertices>(u_a);
  auto rhs = m.mdspan<mesh::vertices>(rhs_a);
  double dx = p.Lx / (p.Nx - 1);
  double dy = p.Lx / (p.Nx - 1);

  const double cdx2 = 1.0 / (dx * dx);
  const double cdy2 = 1.0 / (dy * dy);
  const double aE = p.alpha * p.dt * cdx2;
  const double aW = aE;
  const double aN = p.alpha * p.dt * cdy2;
  const double aS = aN;
  const double aP = 1.0 + 2.0 * (aE + aN);

  auto rmax =
    s.executor()
      .named("residual")
      .reduceall(
        j, rm, m.vertices<mesh::y_axis>(), flecsi::exec::fold::max, double) {
    for(auto i : m.vertices<mesh::x_axis>()) {
      const double r =
        std::fabs(aP * u[j][i] - aE * u[j][i + 1] - aW * u[j][i - 1] -
                  aN * u[j + 1][i] - aS * u[j - 1][i] - rhs[j][i]);
      rm(r);
    }
  };
  return rmax;
}

} // namespace heat::physics
