#include "quad.h"
#include "mesh.h"

std::shared_ptr<Shape> make_quad()
{
  return make_quad( Vec3::x_axis, Vec3::y_axis );
}


std::shared_ptr<Shape> make_quad(const Vec3& x, const Vec3& y)
{

}
