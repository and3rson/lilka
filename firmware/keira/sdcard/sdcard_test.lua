local file_dir = sdcard.ls("/")

print(#file_dir)
for i = 0, #file_dir do
    print('*', file_dir[i])
end

local file = sdcard.open("/test.txt", "a+")
file:write("HELLOWORLD")

local text = file:read(5)
print(text)

local size = file:size()
print(size)
