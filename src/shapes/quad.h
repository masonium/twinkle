#pragma once

#include <memory>
#include "shape.h"

std::shared_ptr<Shape> make_quad();
std::shared_ptr<Shape> make_quad(const Vec3& x, const Vec3& y);
