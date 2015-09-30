#pragma once

#include <memory>
#include "shape.h"
#include "mesh.h"

std::shared_ptr<Mesh> make_quad();
std::shared_ptr<Mesh> make_quad(const Vec3& x, const Vec3& y);

RawModel make_quad_model(const Vec3& x, const Vec3& y);
