<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc/generate-toc again -->
**Table of Contents**

- [twinkle](#twinkle)
	- [Building](#building)
	- [Testing](#testing)
	- [Current Features](#current-features)
	- [Planned Features](#planned-features)

<!-- markdown-toc end -->


# twinkle

Twinkle (tentative name) is path tracer written C++. It will emphasize tracing implicit surfaces using physically realistic BRDFs in a distributed setting.

## Building
`make` should create the twinkle executable, `./twinkle`.

## Testing
`make test` will run an assorted, disorganized collection of unit tests.

## Current Features
Shapes
* Sphere
* Plane
* Implicit Surface
* KD-tree-based triangle mesh
*  - quad (based on mesh)
Lights
* Point
* Directional
* Shape-based
* Environmental (only one allowed)
Surface integrators
* Basic path tracing
* intersection testing
BRDFs
* Diffuse (Lambertian)
* Mirror (perfect specular)
* Oren Nayer
* Fresenel reflection (glass)
Output Formats
* ASCII(Console)
* PPM
Multithreading
ToneMaps
* RSSF (local and global)
Textures
* Grid
* Checker
* solid
* perlin noise


##  Planned Features
Shapes
* Various built-in shapes based on implicits (torus, cylinder)
* generic quadrics...?

Surface integrators
* Bidirectional path tracing

BRDFs
* Cook-Torrance
* BRDF-blending

Textures
* From image
* arbitrary function-based
* worley noise


## Priorities
box primitive
heightmap primitive
lua integration (basic)
- scene
- camera
- integrator (path tracer)
- shapes (sphere, box, transformations)
Task abstraction (films, lua state)


new sky texture
other procedural textures
building scene
heightmap scene
rough glass material
multi-dome
random box building
Small library of rendered scenes
bi-directional path tracing

