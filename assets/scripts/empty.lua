function scene()
   local pos = {0.0, 0.0, 0.0}
   local camera = camera.spherical(pos, {1.0, 0.0, 0.0}, {0.0, 0.0, 1.0})

   local lights = { light.environment(texture.hoseksky({4.0, 0.0, 1.0}, 3.0)); }

   return {shapes = {}; lights = lights; camera = camera}
end
