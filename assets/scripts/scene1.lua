function my_sphere(x, y, z)
   return math.pow(x*x*x*x + y*y*y*y + z*z*z*z, 0.25) - 1.0
end

function my_sphere_grad(x, y, z)
   return x*x*x, y*y*y, z*z*z
end


local implicit_funcs = {}

function implicit_funcs.intersection(f, g)
   function int_fg(x, y, z)
      return max(f(x, y, z), g(x, y, z))
   end
   return int_fg
end

function implicit_funcs.subtraction(f, g)
   function sub_fg(x, y, z)
      return max(-f(x, y, z), g(x, y, z))
   end
   return sub_fg
end

function implicit_funcs.repeat_x(f, r)
   function rx(x, y, z)
      return f(mod(x, r) - 0.5 * r, y, z)
   end
   return rx
end

function implicit_funcs.translate(f, px, py, pz)
   function tr(x, y, z)
      return f(x - px, y - py, z - pz)
   end
   return tr
end

mstr = {-2.0, 0.0, 2.0}
my_sphere_1 = implicit_funcs.translate(my_sphere, mstr[1], mstr[2], mstr[3])

function scene ()
   local s = geom.implicit("my_sphere", "my_sphere_grad", 1.0, bounds.bbox({-1.1}, {1.1}))
   local st = geom.implicit("my_sphere_1", 1.0, bounds.bbox({-10.1}, {10.1}))
   local m = geom.mesh("assets/models/cessna.obj", bounds.bbox({0}, {1.5}));
   local pl = geom.plane({0.0, 1.0, 0.0}, 1.0)
   local s2 = geom.sphere({0.0, 1.0, 0.0}, 1.0)

   local shapes = {
      shape(m, material.diffuse(texture.color({0.2, 0.2, 0.8}))),
      shape(pl, material.diffuse( texture.grid({0.7}, {0.1}, 1.0, 0.05))),
      --shape(pl, material.diffuse( texture.image("assets/images/whitemarble.jpg")))
      -- shape(geom.plane({1.0, 0.0, 1.0}, math.sqrt(2)),
      --                 material.color({0.5, 0.5, 0.5}))
   }

   local lights = {
      light.point({0.0, 3.0, 3.0}, {1.0}),
      light.point({0.0, 2.9, -3.1}, {1.0}),
      light.point({3.0, 2.8, -3.4}, {1.0})
   }

   local camera = camera.perspective({1.5, 1.5, 1.5}, {0.75, 0.3, 0.75}, {0.0, 1.0, 0.0}, math.pi / 2.0)

   local r = {shapes = shapes; lights = lights; camera = camera}
   return r
end
