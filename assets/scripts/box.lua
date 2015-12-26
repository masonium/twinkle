function scene()
   local q1 = geom.box({0.0}, {1.0})

   local m = material.dielectric()

   local shapes = {
      shape(geom.translate(q1, {2.5, 0.0, 0.0}),
		 material.glossy_paint(texture.color({0.1, 0.2, 0.6}))),
      shape(geom.translate(q1, {2.5, 2.5, 0.0}), material.mirror()),
      shape(geom.translate(q1, {0.0, 2.5, 0.0}), material.diffuse(texture.color({0.8, 0.2, 0.3}))),
      shape(geom.plane({0, 1, 0}, 0), material.diffuse(texture.grid({0.8}, {0.3}, 0.25, 0.05)))
   }

   local lights = { light.point({0, 2, 5}, {2.0}),
		    light.environment(texture.color({0.1}))
   }

   local pos = {4.0, 1.5, 4.3}
   local camera = camera.perspective(pos, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, math.pi/2)

   return {shapes = shapes; lights = lights; camera = camera}
end
