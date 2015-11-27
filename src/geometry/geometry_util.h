#pragma once

#include <memory>
#include "bounds.h"
#include "geometry.h"

using std::unique_ptr;

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
