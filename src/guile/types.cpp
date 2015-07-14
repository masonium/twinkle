#include "types.h"

scm_t_bits vec3_tag;

SCM make_scm_vec3(SCM x, SCM y, SCM z)
{
  SCM smob;
  Vec3* v = static_cast<Vec3*>(scm_gc_malloc_pointerless(sizeof(Vec3), "vec3"));
  v->x = scm_to_double(x);
  v->y = scm_to_double(y);
  v->z = scm_to_double(z);

  smob = scm_new_smob(vec3_tag, reinterpret_cast<scm_t_bits>(v));

  return smob;
}

Vec3* smob_to_vec3_ptr(SCM v)
{
  return reinterpret_cast<Vec3*>(SCM_SMOB_DATA(v));
}

Vec3 smob_to_vec3(SCM v)
{
  return *smob_to_vec3_ptr(v);
}


SCM scm_vec3_plus(SCM v1, SCM v2)
{
  auto r1 = smob_to_vec3_ptr(v1);
  auto r2 = smob_to_vec3_ptr(v2);
  SCM v = make_scm_vec3(scm_from_double(r1->x + r2->x),
                        scm_from_double(r1->y + r2->y),
                        scm_from_double(r1->z + r2->z));

  scm_remember_upto_here_1(v1);
  scm_remember_upto_here_1(v2);
  return v;
}

int print_scm_vec3(SCM obj, SCM port, scm_print_state* state)
{
  Vec3* v = reinterpret_cast<Vec3*>(SCM_SMOB_DATA(obj));

  scm_puts("#<v3(", port);
  scm_display(scm_from_double(v->x), port);
  scm_puts(", ", port);
  scm_display(scm_from_double(v->y), port);
  scm_puts(", ", port);
  scm_display(scm_from_double(v->z), port);
  scm_puts(")>", port);

  return 1;
}

extern "C"
{
  void init_vec3(void)
  {
    vec3_tag = scm_make_smob_type("scm_vec3", sizeof(scm_vec3));
    scm_set_smob_print(vec3_tag, print_scm_vec3);
    scm_c_define_gsubr("v3", 3, 0, 0, reinterpret_cast<void*>(make_scm_vec3));
    scm_c_define_gsubr("v+", 2, 0, 0, reinterpret_cast<void*>(scm_vec3_plus));
  }
}
