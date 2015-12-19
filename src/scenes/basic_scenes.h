#pragma once

#include <string>

#include "twinkle.h"
#include "scene.h"
#include "camera.h"

using std::string;

Camera* lua_scene(Scene& scene, const string&);
