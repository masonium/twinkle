function sphere(x, y)
   R = 1.0
   phi = 2 * math.pi * x
   theta = math.pi * y
   return {R * math.sin(theta) * math.cos(phi), 1.2 + R * math.sin(theta) * math.sin(phi), R * math.cos(theta)}
end

function torus(x, y)
   R = 1.0
   r = 0.2
   u = 2*math.pi*y;
   v = 2*math.pi*x;
   s = R + r * math.cos(v);
   return {s * math.cos(u), 1.2 + s * math.sin(u), r * math.sin(v)}
end

function square(x, y)
   return {x, y, 0}
end

function scene()
   local q1 = geom.parametric(torus, 0, 1, 0, 1, 40, 40)

   local shapes = {
--      shape(geom.sphere({0.0}, 1.0), material.diffuse(texture.gradient())),
      shape(q1, material.diffuse(texture.grid({0.1}, {0.9}, 1/10., 1/30., 0.1))),
      shape(geom.plane({0, 1, 0}, 0), material.diffuse(texture.grid({0.8}, {0.3}, 0.25, 0.05)))
   }

   local lights = { light.point({0, 2, 5}, {2.0}) }
   --local lights = {}

   local pos = {2.0, 1.0, 2.3}
   local camera = camera.perspective(pos, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, math.pi/2)

   return {shapes = shapes; lights = lights; camera = camera}
end
