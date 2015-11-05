function my_sphere(x, y, z)
   return math.pow(x*x*x*x + y*y*y*y + z*z*z*z, 0.25) - 1.0
end

function scene ()
   local s = geom.implicit("my_sphere", 1.0, bounds.bbox({-1.1}, {1.1}))
   --local s = geom.sphere({0.0}, 1.0)
   local pl = geom.plane({0.0, 1.0, 0.0}, 1.0)
   local s2 = geom.sphere({0.0, 2.0, 0.0}, 1.0)

   local shapes = {
      shape(s, material.color({0.2, 0.2, 0.8})),
      shape(pl, material.color({0.7, 0.7, 0.7})),
--      shape(s2, material.color({0.8, 0.2, 0.2})),
      shape(s2, material.mirror()),
      shape(geom.plane({1.0, 0.0, 1.0}, math.sqrt(2)),
	    material.color({0.5, 0.5, 0.5}))
   }

   local camera = camera.perspective({0.0, 1.0, 6.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, math.pi / 2.0)
   local r = { shapes = shapes; camera = camera}
   return r
end
