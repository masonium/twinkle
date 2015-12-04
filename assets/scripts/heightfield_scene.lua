function heights(x, y)
   return 2 - math.pow(x - 1, 2) - math.pow(y - 1, 2) +
      0.3 * math.sin(4.1*x) * math.cos(3.1*y) +
      0.2 * math.sin(11.1*x - 5.6*y + 0.4) * math.cos(9.9 * (x + y) - 2)
end

function scene()
   local q1 = geom.heightfield(heights, 0, 2, 0, 2, 100, 100)

   local shapes = {
      shape(q1, material.diffuse(texture.grid({0.6, 0.6, 0.2}, {0.2, 0.2, 0.6}, 0.04, 0.05))),
      shape(geom.plane({0, 1, 0}, 0), material.diffuse(texture.grid({0.5}, {0.1}, 0.25, 0.05)))
   }

   local lights = { light.point({0, 2, 5}, {2.0}) }
   --local lights = {}

   local pos = {2.5, 3.0, 2.5}
   local camera = camera.perspective(pos, {1.0, 1.0, 1.0}, {0.0, 1.0, 0.0}, math.pi/2)

   return {shapes = shapes; lights = lights; camera = camera}
end
