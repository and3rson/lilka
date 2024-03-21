local file_dir = sdcard.listDir("/")

print(#file_dir)

for i = 0, #file_dir do 
    print(file_dir[i])
end

file = sdcard.open_file("test.txt", "rb+") 

sdcard.write_file(file, "HELLOWORLD") 

text = sdcard.read_file(file, 5) 

print(text) 

size = sdcard.file_size(file)
print(size)

sdcard.close_file(file)

