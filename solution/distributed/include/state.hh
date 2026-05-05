#ifndef HEAT_STATE_HH
#define HEAT_STATE_HH

#include "spec/mesh.hh"
#include <cmath>
#include <cstddef>
#include <vector>

namespace heat {

// Initial condition u(x,y,0) = Gaussian
struct gaussianIC {
  double xc, yc, beta; // u0 = exp(-beta * ((x-xc)^2 + (y-yc)^2))
  double operator()(double x, double y) const {
    const double dx = x - xc, dy = y - yc;
    return std::exp(-beta * (dx * dx + dy * dy));
  }
};

// Source term s(x,y,t); default zero
struct zeroSource {
  double operator()(double, double, double) const {
    return 0.0;
  }
};

// Single concrete Dirichlet boundary container (constant values)
struct boundary {
  double Lc = 0.0, Rc = 0.0, Bc = 0.0,
         Tc = 0.0; // left/right/bottom/top constants
  double left(double /*y*/, double /*t*/) const {
    return Lc;
  }
  double right(double /*y*/, double /*t*/) const {
    return Rc;
  }
  double bottom(double /*x*/, double /*t*/) const {
    return Bc;
  }
  double top(double /*x*/, double /*t*/) const {
    return Tc;
  }
};

struct state {

  struct params {
    double t_final = 0.1;
    double dt = 1e-4;
    double alpha = 1e-3, beta = 200.;
    int output_every = 100;
    double implicit_tol = 1e-8;
    double residue_tol = 1e-8;
    double jacobi_omega = 0.8;
    double Lx = 1.0, Ly = 1.0;
    std::size_t Nx = 101, Ny = 101;
    boundary b{0.0, 0.0, 0.0, 0.0};
    zeroSource s;
    double dx() const {
      return Lx / (Nx - 1);
    }
    double dy() const {
      return Ly / (Ny - 1);
    }
  } par;

  struct current {
    struct progress {
      double t = 0.0;
      int step = 0;
    };
    static inline const flecsi::field<progress,
      flecsi::data::single>::definition<flecsi::topo::index>
      prg;
    static inline const flecsi::field<double>::definition<mesh> u, rhs;
    mesh::ptr m;
    flecsi::topo::index::ptr idx;
  } cur;

  explicit state(params par) : par(par) {}
};

inline void
allocate(state & s, flecsi::scheduler & sc) {
  sc.allocate(s.cur.idx, sc.runtime().processes());

  mesh::gcoord axis_extents{s.par.Nx, s.par.Ny};
  mesh::grect geometry = {{{0., s.par.Lx}, {0.0, s.par.Ly}}};
  sc.allocate(s.cur.m,
    mesh::mpi_coloring(sc, sc.runtime().processes(), axis_extents),
    geometry);
}

} // namespace heat

#endif // HEAT_STATE_HH
