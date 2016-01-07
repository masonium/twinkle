#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_roots.h>

using std::pair;
using std::vector;

double cdf(double x, void* param)
{
  double target = *reinterpret_cast<double*>(param);
  double ad = 0.25 * (2.0 *x / (x*x + 1) + 2 * atan(x) + M_PI);

  return ad * 2.0 / M_PI - target;
}

double pdf(double x, void* param)
{
  double y = x*x + 1;
  return (1 / (y*y)) / (M_PI / 2.0);
}

void fdf(double x, void* param, double* f, double* df)
{
  *f = cdf(x, param);
  *df = pdf(x, param);
}

using data_vec = vector<pair<double, double>>;

int fit_r3(const gsl_vector* x, void* params, gsl_vector* f)
{
  double A = gsl_vector_get(x, 0);
  double B = gsl_vector_get(x, 1);
  double C = gsl_vector_get(x, 2);
  double D = gsl_vector_get(x, 3);
  double E = gsl_vector_get(x, 4);
  double F = gsl_vector_get(x, 5);
  double G = gsl_vector_get(x, 6);

  const auto& dv = *reinterpret_cast<data_vec*>(params);

  for (auto i = 0u; i < dv.size(); ++i)
  {
    double u = dv[i].first - 0.5;
    double t = u * (A + u * (B + u * C)) / (D + u * (E + u * (F + u * G)));
    gsl_vector_set(f, i, t - dv[i].second);
  }

  return GSL_SUCCESS;
}

int fit_r3_jacobian(const gsl_vector* x, void* params, gsl_matrix* J)
{
  // double A = gsl_vector_get(x, 0);
  // double B = gsl_vector_get(x, 1);
  // double C = gsl_vector_get(x, 2);
  double D = gsl_vector_get(x, 3);
  double E = gsl_vector_get(x, 4);
  double F = gsl_vector_get(x, 5);
  double G = gsl_vector_get(x, 6);

  const auto& dv = *reinterpret_cast<data_vec*>(params);

  for (auto i = 0u; i < dv.size(); ++i)
  {
    double u = dv[i].first - 0.5;
    const double denom = (D + u * (E + u * (F + u * G)));

    gsl_matrix_set(J, i, 0, u / denom); // df_i/dA = u / denom;
    gsl_matrix_set(J, i, 1, u*u / denom); // df_i/dB = u^2 / denom;
    gsl_matrix_set(J, i, 2, u*u*u / denom); // df_i/dC = u^3 / denom;

    double invd2 = 1 / (denom * denom);

    gsl_matrix_set(J, i, 3, -1 * invd2); // df_i/dD = -1 / denom^2;
    gsl_matrix_set(J, i, 4, -u * invd2); // df_i/dE = -u / denom^2;
    gsl_matrix_set(J, i, 5, -u*u * invd2); // df_i/dF = -u^2 / denom^2;
    gsl_matrix_set(J, i, 6, -u*u*u * invd2); // df_i/dG = -u^3 / denom^2;
  }

  return GSL_SUCCESS;
}

data_vec generate_data(void)
{
  auto T = gsl_root_fdfsolver_newton;
  auto solver = gsl_root_fdfsolver_alloc(T);

  gsl_function_fdf fdf_f;
  fdf_f.f = cdf;
  fdf_f.df = pdf;
  fdf_f.fdf = fdf;

  double x_guess = 0;

  int iter = 0;
  int max_iter = 1000;
  int status;

  int N = 1001;
  vector<pair<double, double>> data(N);

  double target = 0.7;
  fdf_f.params = &target;
  for (int i = 0; i < N; ++i)
  {
    target = i * 1.0 / (N - 1);

    gsl_root_fdfsolver_set(solver, &fdf_f, x_guess);

    double f_curr;
    status = GSL_CONTINUE;
    for (iter = 0; iter < max_iter && (status == GSL_CONTINUE); ++iter)
    {
      gsl_root_fdfsolver_iterate(solver);
      x_guess = gsl_root_fdfsolver_root(solver);
      f_curr = cdf(x_guess, fdf_f.params);
      status = gsl_root_test_residual(f_curr, 0.00001);
    }

    std::cout << iter << ": " << x_guess << ", " << target << ", " << f_curr << "\n";

    data.emplace_back(x_guess, target);
  }

  return data;
}

int main(int argc, char** args)
{
  auto dv = generate_data();

  const size_t n = dv.size();
  const size_t p = 7;

  auto J = gsl_matrix_alloc(n, p);
  auto covar = gsl_matrix_alloc(p, p);

  gsl_multifit_function_fdf f;
  f.f = fit_r3;
  f.df = fit_r3_jacobian;
  f.n = n;
  f.p = p;
  f.params = &dv;

  auto guess = gsl_vector_alloc(p);
  for (auto i = 0u; i < p; ++i)
    gsl_vector_set(guess, i, 0.5);

  auto s = gsl_multifit_fdfsolver_alloc(gsl_multifit_fdfsolver_lmsder, n, p);
  gsl_multifit_fdfsolver_set(s, &f, guess);


  const double xtol = 1e-8;
  const double gtol = 1e-8;
  const double ftol = 0.0;
  int status, info;
  gsl_multifit_fdfsolver_driver(s, 20, xtol, gtol, ftol, &info);

  gsl_multifit_fdfsolver_jac(s, J);
  gsl_multifit_covar (J, 0.0, covar);

  fprintf(stderr, "summary from method '%s'\n",
          gsl_multifit_fdfsolver_name(s));
  fprintf(stderr, "number of iterations: %zu\n",
          gsl_multifit_fdfsolver_niter(s));
  fprintf(stderr, "function evaluations: %zu\n", f.nevalf);
  fprintf(stderr, "Jacobian evaluations: %zu\n", f.nevaldf);
  fprintf(stderr, "reason for stopping: %s\n",
          (info == 1) ? "small step size" : "small gradient");
  fprintf(stderr, "initial |f(x)| = %g\n", chi0);
  fprintf(stderr, "final   |f(x)| = %g\n", chi);

}
