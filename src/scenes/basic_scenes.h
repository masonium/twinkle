#pragma once

#include <string>

#include "twinkle.h"
#include "scene.h"
#include "camera.h"

using std::string;

shared_ptr<Camera> empty_scene(Scene& scene);
shared_ptr<Camera> glass_scene(Scene& scene);
shared_ptr<Camera> box_scene(Scene& scene, scalar angle);
shared_ptr<Camera> many_sphere_scene(Scene& scene, int angle = 0);
shared_ptr<Camera> model_scene(Scene& scene, const string& filename,
                               bool invert);
shared_ptr<Camera> default_scene(Scene& scene, int angle);
shared_ptr<Camera> showcase_scene(Scene& scene, int angle);
shared_ptr<Camera> lua_scene(Scene& scene, const string&);
