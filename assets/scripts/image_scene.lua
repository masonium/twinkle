function scene()
   local q1 = geom.quad({1.0, 0.0, 0.0}, {0.0, 1.0, 0.0})
   --local q1 = geom.sphere({0.0}, 1.0);

   local shapes = { shape(q1, material.diffuse(texture.image("assets/images/whitemarble.jpg"))) }

   local pos = {0.0, 0.0, 5.0}

   local lights = { light.point(pos, {2.0}) }

   local camera = camera.perspective(pos, {0.0}, {0.0, 1.0, 0.0}, math.pi/2)

   return {shapes = shapes; lights = lights; camera = camera}
end
