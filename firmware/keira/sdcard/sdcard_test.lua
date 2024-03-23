local file_dir = sdcard.dlist("/")

print(#file_dir)

for i = 0, #file_dir do 
    print(file_dir[i])
end

file = sdcard.fopen("test.txt", "rb+") 

sdcard.fwrite(file, "HELLOWORLD") 

text = sdcard.fread(file, 5) 

print(text) 

size = sdcard.fsize(file)
print(size)

sdcard.fclose(file)

