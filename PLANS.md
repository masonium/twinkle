## Visible Features
### Shapes
* box primitive
* heightmap (based on kd-mesh)
* implicit built-ins (torus, capsule, box)

### Materials
* rough glass material
* plastic (cook-torrance)

### Textures
* image-based
* function-basedd
* worley-noise

### Renders
* time-based debug renderer (intersection, shading?)

### Volume rendering !

## External Features
### Lua scripting(!!!)
* basics (renderer, camera, render-options, sphere, implicit, rough-color material)
* push luastate into generic renderer

## Internal advancements
* push scheduler as argument to renderer
* lua state into
* Mesh or KDMesh based on number of triangles
* condensing of nested transforms into transform shapes
* cache-coherent kd-tree (re)organization
* population monte-carlo for pixel-choice of dierct lighting


## scenes
* 'showcase-scene': all available shapes
* building scene (large tower made of random blocks, two or three large ones primary-colored, smaller in background
* water-scene (height map with caustics)
