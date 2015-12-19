function scene()
   local pos = {0.0, 5.0, 5.0}
   local camera = camera.perspective(pos, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, math.pi/2)

   return {shapes = {}; lights = {}; camera = camera}
end
