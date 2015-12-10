function scene()
   local q1 = geom.box({0.0}, {1.0})

   local shapes = {
      shape(q1, material.dielectric()),
      shape(geom.plane({0, 1, 0}, 0), material.diffuse(texture.grid({0.8}, {0.3}, 0.25, 0.05)))
   }

   local lights = { light.point({0, 2, 5}, {2.0}) }

   local pos = {2.0, 1.0, 2.3}
   local camera = camera.perspective(pos, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, math.pi/2)

   return {shapes = shapes; lights = lights; camera = camera}
end
