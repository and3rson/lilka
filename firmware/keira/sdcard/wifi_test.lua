wifi.disconnect()

local test = wifi.status()

print(test)

local scan = wifi.scan()

print(#scan)
for i = 1, #scan do -- This will cycle through and print each element of the array
    print(scan[i])
end