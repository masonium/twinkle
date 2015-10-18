function clamp(a, min_val, max_val)
   if a < min_val then
      return min_val
   elseif a > max_val then
      return max_val
   else
      return a
   end
end

function checker(u, v)
   local x = math.floor(u);
   local y = math.floor(v);
   if (x + y) % 2 == 0 then
      return 0, 0, 0
   else
      return 1, 1, 1
   end
end
