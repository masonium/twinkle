#pragma once

#include <string>

#include "twinkle.h"
#include "scene.h"
#include "camera.h"

using std::string;

shared_ptr<Camera> empty_scene(Scene& scene, scalar aspect_ratio);
shared_ptr<Camera> glass_scene(Scene& scene, scalar aspect_ratio);
shared_ptr<Camera> box_scene(Scene& scene, scalar ar, scalar angle);
shared_ptr<Camera> many_sphere_scene(Scene& scene, scalar ar, int angle = 0);
shared_ptr<Camera> model_scene(Scene& scene, scalar aspect_ratio,
                               const string& filename,
                               bool invert);
shared_ptr<Camera> default_scene(Scene& scene, scalar aspect_ratio, int angle);
shared_ptr<Camera> showcase_scene(Scene& scene, scalar ar, int angle);
shared_ptr<Camera> lua_scene(Scene& scene, scalar ar, const string&);
