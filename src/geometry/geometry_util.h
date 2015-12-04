#pragma once

#include <memory>
#include <vector>
#include "bounds.h"
#include "geometry.h"
#include "vec2.h"

using std::unique_ptr;
using std::vector;

/**
 * Load a model from file and return the mesh associated with that model.
 *
 * This function will automatically decide between a mesh and a kd-mesh,
 * returning whatever is appropriate for its size.
 */
unique_ptr<Geometry> load_mesh(const std::string& filename);


/**
 * Same as above, but also rescales the mesh in advance.
 */
unique_ptr<Geometry> load_mesh(const std::string& s, const bounds::AABB&);


/**
 * Returns a set of points for a surface function to be evaluated
 * at. *_mesh functions taking a list of points of values expect the points to
 * be in the same order as mesh_eval_points provides.
 */
vector<Vec2> mesh_eval_points(Vec2 xz_min, Vec2 xz_max,
                              uint num_x, uint num_z);

/**
 * Generate a mesh from a set of points evaluated in mesh.
 */
unique_ptr<Geometry> parametric_mesh(std::function<Vec3(scalar, scalar)> points,
                                     Vec2 uv_min, Vec2 uv_max,
                                     uint ud, uint vd);

unique_ptr<Geometry> parametric_mesh(const Vec3* points,
                                    Vec2 uv_min, Vec2 uv_max,
                                    uint ud, uint vd);


/**
 * Generate a mesh from a heightmap. The heights are given as a 1d-array of
 * (xd + 1) * (yd + 1) values. The first values is at (x_min, y_min), and the
 * second value is at (x_min + dx, y_min).
 */
unique_ptr<Geometry> heightfield_mesh(const scalar* heights,
                                      Vec2 xz_min, Vec2 xz_max,
                                      uint xd, uint yd);

unique_ptr<Geometry> heightfield_mesh(std::function<scalar(scalar, scalar)>,
                                      Vec2 xz_min, Vec2 xz_max,
                                      uint xd, uint yd);
