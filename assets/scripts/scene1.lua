function scene ()
   local s = sphere({0.0, 0.0, 0.0}, 1.0)
   local pl = plane({0.0, 1.0, 0.0}, 1.0)
   local s2 = sphere({0.0, 2.0, 0.0}, 1.0)

   local shapes = {
      shape(s, material_color({0.2, 0.2, 0.8})),
      shape(pl, material_color({0.7, 0.7, 0.7})),
      shape(s2, material_color({0.8, 0.2, 0.2})),
      shape(plane({1.0, 0.0, 1.0}, math.sqrt(2)),
	    material_color({0.5, 0.5, 0.5}))
   }

   local camera = camera_perspective({0.0, 1.0, 6.0}, {0.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, math.pi / 2.0)
   local r = { shapes = shapes; camera = camera}
   return r
end
