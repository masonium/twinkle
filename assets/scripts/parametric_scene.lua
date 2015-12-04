function torus(x, y)
   R = 1.0
   r = 0.2
   u = 2*math.pi*(y + 0.125);
   v = 2*math.pi*x;
   s = R + r * math.cos(v);
   return {s * math.cos(u), 1.0 + s * math.sin(u), r * math.sin(v)}
end

function square(x, y)
   return {x, y, 0.001*x}
end

function scene()
   local q1 = geom.parametric(square, 0, 1, 0, 1, 1, 1)

   local shapes = {
--      shape(geom.sphere({0.0}, 1.0), material.diffuse(texture.gradient())),
      shape(q1, material.diffuse(texture.gradient())),
      shape(geom.plane({0, 1, 0}, 0), material.diffuse(texture.grid({0.5}, {0.1}, 0.25, 0.05)))
   }

   -- local lights = { light.point({0, 2, 5}, {2.0}) }
   local lights = {}

   local pos = {2.0, 1.0, 2.3}
   local camera = camera.perspective(pos, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, math.pi/2)

   return {shapes = shapes; lights = lights; camera = camera}
end
