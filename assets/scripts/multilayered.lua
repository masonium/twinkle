function scene()
   local base = {0.3, 0.3, 0.9}

   local shapes = {
      shape(geom.sphere({-2.0, 0.0, 0.0}, 1.0), material.diffuse(texture.color(base))),
      shape(geom.sphere({2.0, 0.0, 0.0}, 1.0), material.glossy_paint(texture.color(base))),
      shape(geom.plane({0, 0, 1}, 1.0), material.diffuse(texture.grid({0.8}, {0.3}, 1.0, 0.1)))
   }

   local lights = {
      light.environment(texture.color({2.0})),
      light.point({0.0, 2.0, 3.0}, {10.0})
   }

   local pos = {0.0, 5.0, 0}
   local camera = camera.perspective(pos, {0.0, 0.0, 0.0}, {0.0, 0.0, 1.0}, math.pi/2)

   return {shapes = shapes; lights = lights; camera = camera}
end
