function scene()
   local r = 0.75
   local s = geom.sphere({0.0}, r)


   local shapes = {
      shape(geom.translate(s, {-4.0, 0.0, 0.0}), material.mirror()),
      shape(geom.translate(s, {-2.0, 0.0, 0.0}), material.rough_mirror(0.01)),
      shape(geom.translate(s, {-0.0, 0.0, 0.0}), material.rough_mirror(0.05)),
      shape(geom.translate(s, {2.0, 0.0, 0.0}), material.rough_mirror(0.1)),
      --shape(geom.translate(s, {4.0, 0.0, 0.0}), material.rough_mirror(0.2)),
      shape(geom.plane({0.0, 0.0, 1.0}, r),
	    material.diffuse(texture.grid({1.0}, {0.1}, 1.0, 0.08)))

   }

   local lights = {
      light.environment(texture.color({3.0, 3.0, 3.0}))
   };

   local cam = camera.perspective({0.0, 4.0, r}, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, math.pi*0.5);

   return { shapes = shapes, lights = lights, camera = cam}
end
