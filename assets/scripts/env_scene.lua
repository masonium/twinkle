function scene()
   local shapes = { }
   local lights = { light.environment(texture.hoseksky({1.0}, 3.0)) }

   local camera = camera.spherical({0.0}, {0.0, 0.0, 1.0}, {1.0, 0.0, 0.0})

   return {shapes = shapes; lights = lights; camera = camera}
end
