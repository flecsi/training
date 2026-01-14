#ifndef HEAT_PHYSICS_HH
#define HEAT_PHYSICS_HH

#include "spec/control.hh"

namespace heat::physics {

void initialize(state &s, const gaussianIC &ic);

void initialize_rhs(state &s);

void apply_dirichlet(state &s);

template<bool RED>
auto
red_black_range(int j) {
  return ((j & 1) ^ (RED ? 1 : 0)) ? 1 : 2;
}

template<bool RED>
double
red_black(state &s) {
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

double analytical_gaussian(double x,
  double y,
  double t,
  double alpha,
  double xc,
  double yc,
  double beta);

double compute_error(const state &s, double xc, double yc);

double residual(state &s);

} // namespace heat::physics

#endif // HEAT_PHYSICS_HH
