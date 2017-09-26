function on_connect(fd)
	print("on_connect", fd)
end
function on_message(fd, msg)
	print("on_message", fd, msg)
	
	tluac:send(fd, msg .. " ---- OK")
end
function on_close(fd)
	print("on_close", fd)
end

print("-----------------------")
tluac:on(tluac.CONNECT, "on_connect")
tluac:on(tluac.MESSAGE, "on_message")
tluac:on(tluac.CLOSE, "on_close")
print("=========================")
