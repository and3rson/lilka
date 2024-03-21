status = wifi.get_status()
print(status)

wifi.disconnect()

status = wifi.get_status()
print(status)

scan = wifi.scan()

print(#scan)
for i = 1, #scan do -- This will cycle through and print each element of the array
    print(scan[i])
    rssi = wifi.get_rssi(i)
    print(rssi)
    rssi = wifi.get_encryption_type(i)
    print(rssi)
end
