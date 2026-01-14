#include "include/io.hh"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

namespace heat::io {

void
write_csv(const state &s, const std::string &fname) {
  std::ofstream file(fname);
  if(!file.is_open()) {
    std::cerr << "Error: cannot open file for writing: " << fname << '\n';
    return;
  }

  file << std::fixed << std::setprecision(8);
  file << "x,y,u\n";

  for(int j = 0; j < s.cur.m.Ny; ++j) {
    for(int i = 0; i < s.cur.m.Nx; ++i) {
      const int id = s.cur.m.idx(i, j);
      const double x = i * s.par.dx();
      const double y = j * s.par.dy();
      const double u = s.cur.u[id];
      file << x << ',' << y << ',' << u << '\n';
    }
  }
}

void
print_diag(const state &s) {
  auto mm = std::minmax_element(s.cur.u.begin(), s.cur.u.end());
  const double umin = (mm.first != s.cur.u.end()) ? *mm.first : 0.0;
  const double umax = (mm.second != s.cur.u.end()) ? *mm.second : 0.0;

  std::cout << "step " << s.cur.prg.step << "  t=" << std::fixed
            << std::setprecision(6) << s.cur.prg.t << "  u[min,max]=["
            << std::setprecision(6) << umin << ", " << umax << "]" << '\n';
}

void
output_print(const state &s, std::string prefix) {
  std::ostringstream oss;
  oss << prefix << std::setw(6) << std::setfill('0') << s.cur.prg.step
      << ".csv";
  write_csv(s, oss.str());
  print_diag(s);
}

} // namespace heat::io
