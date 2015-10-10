#pragma once

#include "twinkle.h"
#include "scene.h"
#include "camera.h"

shared_ptr<Camera> empty_scene(Scene& scene, scalar aspect_ratio);
shared_ptr<Camera> single_sphere(Scene& scene, scalar aspect_ratio);
shared_ptr<Camera> box_scene(Scene& scene, scalar ar, scalar angle);
shared_ptr<Camera> many_sphere_scene(Scene& scene, scalar ar, int angle = 0);
shared_ptr<Camera> model_scene(Scene& scene, scalar aspect_ratio, bool invert);
shared_ptr<Camera> default_scene(Scene& scene, scalar aspect_ratio, int angle);
shared_ptr<Camera> showcase_scene(Scene& scene, scalar ar, int angle);

