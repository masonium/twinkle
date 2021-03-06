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

