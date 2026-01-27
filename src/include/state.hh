#ifndef HEAT_STATE_HH
#define HEAT_STATE_HH

#include <cmath>
#include <cstddef>
#include <vector>

namespace heat {

struct mesh {
  int Nx, Ny;
  double Lx, Ly;

  explicit mesh(int Nx, int Ny, double Lx, double Ly)
    : Nx(Nx), Ny(Ny), Lx(Lx), Ly(Ly) {}

  inline std::size_t idx(int i, int j) const {
    return i + j * Nx;
  }
};

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
    int Nx = 101, Ny = 101;
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
    } prg;

    std::vector<double> u;
    std::vector<double> rhs;
    mesh m;

    current(int Nx, int Ny, double Lx, double Ly) : m(Nx, Ny, Lx, Ly) {}
  } cur;

  explicit state(params par) : par(par), cur(par.Nx, par.Ny, par.Lx, par.Ly) {}
};

inline void
allocate(state &s) {
  const std::size_t N = static_cast<std::size_t>(s.cur.m.Nx * s.cur.m.Ny);
  // We do not rely on the initialization to 0
  s.cur.u.resize(N);
  s.cur.rhs.resize(N);
}

} // namespace heat

#endif // HEAT_STATE_HH
