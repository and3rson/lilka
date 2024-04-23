local status = wifi.get_status()
print(status)

wifi.disconnect()

status = wifi.get_status()
print(status)

local scan = wifi.scan()

print(#scan)
for i = 1, #scan do -- This will cycle through and print each element of the array
    local rssi = wifi.get_rssi(i)
    local enc_type = wifi.get_encryption_type(i)
    print(scan[i], rssi, enc_type)
end
