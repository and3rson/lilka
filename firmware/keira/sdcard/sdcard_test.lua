local file_dir = sdcard.ls("/")

print(#file_dir)

for i = 0, #file_dir do 
    print(file_dir[i])
end

file = file("/test.txt", "a+") 

file:write("HELLOWORLD") 

text = file:read(5) 

print(text) 

size = file:size()
print(size)

