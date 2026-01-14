#include "include/physics.hh"
#include <cmath>
#include <vector>

namespace heat::physics {

void
initialize(state &s, const gaussianIC &ic) {
  for(int j = 0; j < s.cur.m.Ny; ++j)
    for(int i = 0; i < s.cur.m.Nx; ++i)
      s.cur.u[s.cur.m.idx(i, j)] = ic(i * s.par.dx(), j * s.par.dy());
}

void
initialize_rhs(state &s) {
  // Build RHS b = u^n + dt * s(x,y,t^{n+1})
  for(int j = 0; j < s.cur.m.Ny; ++j) {
    for(int i = 0; i < s.cur.m.Nx; ++i) {
      const int id = s.cur.m.idx(i, j);
      s.cur.rhs[id] =
        s.cur.u[id] +
        s.par.dt * s.par.s(i * s.par.dx(), j * s.par.dy(), s.par.dt);
    }
  }
}

// Apply Dirichlet boundary values to a target field
void
apply_dirichlet(state &s) {
  for(int j = 0; j < s.cur.m.Ny; ++j) {
    const double y = j * s.par.dy();
    s.cur.u[s.cur.m.idx(0, j)] = s.par.b.left(y, s.cur.prg.t);
    s.cur.u[s.cur.m.idx(s.cur.m.Nx - 1, j)] = s.par.b.right(y, s.cur.prg.t);
  }
  for(int i = 0; i < s.cur.m.Nx; ++i) {
    const double x = i * s.par.dx();
    s.cur.u[s.cur.m.idx(i, 0)] = s.par.b.bottom(x, s.cur.prg.t);
    s.cur.u[s.cur.m.idx(i, s.cur.m.Ny - 1)] = s.par.b.top(x, s.cur.prg.t);
  }
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

double
compute_error(const state &s, double xc, double yc) {
  const double alpha = s.par.alpha;
  double err2 = 0.0, ref2 = 0.0;

  for(int j = 0; j < s.cur.m.Ny; ++j)
    for(int i = 0; i < s.cur.m.Nx; ++i) {
      int id = s.cur.m.idx(i, j);
      double x = i * s.par.dx(), y = j * s.par.dy();
      double u_exact =
        analytical_gaussian(x, y, s.cur.prg.t, alpha, xc, yc, s.par.beta);
      double diff = s.cur.u[id] - u_exact;
      err2 += diff * diff;
      ref2 += u_exact * u_exact;
    }
  return std::sqrt(err2 / ref2); // relative L2 error
}

double
residual(state &s) {
  double rmax = 0.0;

  const double cdx2 = 1.0 / (s.par.dx() * s.par.dx());
  const double cdy2 = 1.0 / (s.par.dy() * s.par.dy());
  const double aE = s.par.alpha * s.par.dt * cdx2;
  const double aW = aE;
  const double aN = s.par.alpha * s.par.dt * cdy2;
  const double aS = aN;
  const double aP = 1.0 + 2.0 * (aE + aN);

  for(int j = 1; j < s.cur.m.Ny - 1; ++j) {
    for(int i = 1; i < s.cur.m.Nx - 1; ++i) {
      const int id = s.cur.m.idx(i, j);

      const double Au = aP * s.cur.u[id] - aE * s.cur.u[s.cur.m.idx(i + 1, j)] -
                        aW * s.cur.u[s.cur.m.idx(i - 1, j)] -
                        aN * s.cur.u[s.cur.m.idx(i, j + 1)] -
                        aS * s.cur.u[s.cur.m.idx(i, j - 1)];

      const double r = Au - s.cur.rhs[id];
      const double ar = std::fabs(r);
      if(ar > rmax)
        rmax = ar;
    }
  }
  return rmax;
}

} // namespace heat::physics
