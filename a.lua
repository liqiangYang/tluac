local a = require("t")
local na = a.new()
print(na)
local t = getmetatable(na)
print(t)
for k,v in pairs(t) do
	print(k,v)
end

local i = 100000000
while i < 999999999 do
local tmp = na:push("xxxxxxxxxx" .. i)
collectgarbage("collect")

local k,m,n = na:shift("zzzzzzzzz")
print(k)
i = i + 1
end
