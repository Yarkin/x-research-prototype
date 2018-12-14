function entries(arg) -- iterator
	return function()
	       return arg:pop();
         end
end

for i in entries(the_list) do
  io.write("From LUA:  ", i, "\n")
end

for i=1,10 do
	the_list:push(50+i*100);
end

for i=1,10 do
	the_container:add(50+i*100);
end