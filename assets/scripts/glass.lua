function scene()
   --local glass_plate = geom.box({-1.0, -1.0, -0.1}, {1.0, 1.0, 0.1})
   local glass_plate = geom.sphere({0.5, 0.5, 0.5}, 1.0)
   glass_plate = geom.translate(geom.rotate_aa(glass_plate, {1.0, 0.0, 0.0}, -math.pi/5.0), {0.0, 3.0, 3.0});

   --local m = material.rough_dielectric(0.01);
   local m = material.dielectric();

   local cube = geom.box({0.0}, {0.4});

   local shapes = {
      shape(glass_plate, m),
      shape(geom.translate(cube, {2.5, 2.5, 0.0}), material.diffuse(texture.color({0.2, 0.2, 0.8}))),
      shape(geom.translate(cube, {0.0, 2.5, 0.0}), material.diffuse(texture.color({0.8, 0.2, 0.3}))),
      shape(geom.plane({0, 0, 1}, 0), material.diffuse(texture.grid({0.7,0.7,0.9}, {0.3}, 0.75, 0.05)))
   }

   local lights = { --light.point({0, 2, 5}, {2.0}),
      light.environment(texture.color({3.0, 3.0, 3.0}))
   }

   local pos = {3.0, 4.0, 4.0}
   local camera = camera.perspective(pos, {0.5, 0.0, 0.0}, {0.0, 0.0, 1.0}, math.pi/2)

   return {shapes = shapes; lights = lights; camera = camera}
end
