#pragma once

#include <memory>
#include "shape.h"
#include "mesh.h"

std::shared_ptr<Mesh> make_quad();
std::shared_ptr<Mesh> make_quad(const Vec3& x, const Vec3& y);
