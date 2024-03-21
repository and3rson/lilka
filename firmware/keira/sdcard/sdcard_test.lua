test = sdcard.open_file("test.txt", "rb+") 

sdcard.write_file(test, "HELLOWORLD") 

text = sdcard.read_file(test, 5) 

print(text) 

sdcard.close_file(test)