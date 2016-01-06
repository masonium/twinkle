function scene()

   local m = material.dielectric()

   local shapes = {
      --shape(geom.sphere({3.0, 0.0, 3.0}, 0.3), material.emissive(texture.color({4.0}))),
      shape(geom.sphere({0.0}, 1.0), material.dielectric()),
      shape(geom.sphere({-1.0, -3.0, 0.0}, 1.0), material.diffuse(texture.color({0.8, 0.2, 0.3}))),
      shape(geom.plane({0, 0, 1}, 1.0), material.diffuse(texture.checker({0.8}, {0.3}, 0.25)))
   }

   local lights = { light.point({3, 0, 3}, {2.0}),
		    light.environment(texture.hoseksky({math.sin(40*math.pi/180), 0.0, math.cos(40*math.pi/180)}, 4.0))
      --light.environment(texture.color({2.0}))
   }

   local pos = {0.0, 4.0, 0.0}
   local lookat = {0.0, 0.0, 0.0}
   local camera = camera.perspective(pos, lookat, {0.0, 0.0, 1.0}, math.pi/2)

   return {shapes = shapes; lights = lights; camera = camera}
end
