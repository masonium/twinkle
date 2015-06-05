<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc/generate-toc again -->
**Table of Contents**

- [twinkle](#twinkle)
    - [Building](#building)
    - [Testing](#testing)
    - [Current Features](#current-features)

<!-- markdown-toc end -->


# twinkle

Twinkle (tentative name) is path tracer written C++. It will emphasize tracing implicit surfaces using physically realistic BRDFs in a distributed setting.

## Building
`make` should create the twinkle executable.

## Testing
`make test` will run an assorted, disorganized collection of tests.


## Current Features
Shapes
* Sphere
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
Output Formats
* ASCII(Console)
* PPM
Multithreading
ToneMaps
* RSSF
Textures
* Grid


##  Planned Features
Shapes
* Plane
* Implicit Surface

Scripting system (probably lua)

Surface integrators
* Bidirectional path tracing
* ERPT

Full material system (incorp

BRDFs
* Oren-Nayer
* Fresnel reflection/transmission
* Cook-Torrance
* BRDF-blending

Textures
* From image
* perlin/simplex noise
* arbitrary function-based

Automatic Differentiation for implicit surface rendering
