# FleCSI Training Material: 2-D Heat Equation Solver

This repository contains hands-on training material used in the FleCSI training.  
The exercises are based on a **2-D heat equation solver** that is incrementally extended as new concepts are introduced.

The implementation uses a **structured Cartesian grid**, **implicit Backward Euler time integration**, and a **Red–Black Gauss–Seidel (RBGS)** iterative solver.  
The code is written for clarity and instructional value.

---

## `3_hours` directory Layout

```
provided/
src/
solution_1/
solution_2/
solution_3/
solution_4/
solution_5/
```

- **provided/**  
  Material supplied to reduce boilerplate and setup complexity.

- **src/**  
  Primary working directory for the exercises.  
  Contains source files, headers, `CMakeLists.txt`, and the build directory.  
  Participants actively modify this directory during the training.

- **solution_X/**  
  Reference solutions corresponding to the end of each module.  
  `solution_1` matches Module 1, `solution_2` matches Module 2, etc.  
  Each solution builds independently and reflects the expected code state for that module.

---

## Problem Description

The application solves the **2-D heat equation** on a uniform Cartesian mesh:

```
∂u/∂t = α ∇²u + s(x, y, t)
```

Domain:

```
(x, y) ∈ [0, Lx] × [0, Ly]
```

Features:

- Fixed **Dirichlet boundary conditions** on all sides  
- **Gaussian initial temperature distribution**  
- Optional source term `s(x,y,t)`  
- Spatial discretization via **five-point finite-difference Laplacian**  
- Time integration using **implicit Backward Euler**  
- Linear system solved with **Red–Black Gauss–Seidel (RBGS)**  
- Convergence monitored via update norms and residuals  

An **analytical solution for the Gaussian initial condition** is used for verification and error analysis.

---

## Numerical Method

### Time Discretization

Backward Euler form:

```
(I − α Δt ∇²h) uⁿ⁺¹ = uⁿ + Δt sⁿ⁺¹
```

---

### Spatial Discretization (Five-Point Stencil)

On a uniform grid:

```
(∇²u)ᵢⱼ ≈
  (uᵢ₊₁ⱼ − 2uᵢⱼ + uᵢ₋₁ⱼ) / Δx²
+ (uᵢⱼ₊₁ − 2uᵢⱼ + uᵢⱼ₋₁) / Δy²
```

The resulting linear system for each interior node:

```
aP uᵢⱼ − aE uᵢ₊₁ⱼ − aW uᵢ₋₁ⱼ − aN uᵢⱼ₊₁ − aS uᵢⱼ₋₁ = bᵢⱼ
```

Coefficients:

```
aE = aW = α Δt / Δx²
aN = aS = α Δt / Δy²
aP = 1 + 2(aE + aN)
bᵢⱼ = uᵢⱼⁿ + Δt sᵢⱼⁿ⁺¹
```

---

## Solver: Red–Black Gauss–Seidel (RBGS)

- Grid colored as a checkerboard  
  - Red: `(i + j)` even  
  - Black: `(i + j)` odd  

- Iteration procedure:
  1. Red sweep: update all red nodes using black neighbors  
  2. Black sweep: update all black nodes using updated red values  
  3. Re-apply Dirichlet boundary conditions after each sweep  

Update formula:

```
uᵢⱼ(new) =
  (1 − ω) uᵢⱼ(old)
+ ω ( bᵢⱼ
      + aE uᵢ₊₁ⱼ
      + aW uᵢ₋₁ⱼ
      + aN uᵢⱼ₊₁
      + aS uᵢⱼ₋₁ ) / aP
```

`ω` is the relaxation factor, typically `0.7–1.0`.

---

## Source Organization (src/)

Initial layout inside `src/`:

```
CMakeLists.txt        Build configuration
main.cc               Program entry point
state.hh              Mesh, parameters, and state storage
physics.hh            Discretization, BCs, RBGS solver
advance.hh            Time integration loop
io.hh                 Output and diagnostics
visualization/viz.py  Python visualization script
```

Exact contents evolve across training modules.

---

## Outputs

- CSV snapshots of the temperature field:
  ```
  u_000000.csv
  u_000100.csv
  …
  u_final.csv
  ```
- Python-based visualization (PNG, GIF, MP4)
- Relative L2 error against the analytical solution

---

## Parameters

| Parameter | Meaning | Typical Default |
|---------|--------|----------------|
| `Nx, Ny` | Grid resolution | `101 × 101` |
| `Lx, Ly` | Domain size | `1.0 × 1.0` |
| `alpha` | Thermal diffusivity | `1e-3` |
| `dt` | Time step | `1e-4` |
| `t_final` | Final simulation time | `0.1` |
| `implicit_tol` | Solver tolerance | `1e-8` |
| `jacobi_omega` | RBGS relaxation factor | `0.8` |

---

## Visualization

A Python script in `visualization/` renders snapshots and animations.

- PNG frames
- GIF animations
- MP4 videos (if `ffmpeg` is available)

---

## Notes

- The code compiles at every module boundary.
- Full execution is intentionally incomplete in Module 3.
- Reference solutions allow rapid recovery and comparison.
- Emphasis is on numerical methods, data layout, and FleCSI concepts rather than performance tuning.
