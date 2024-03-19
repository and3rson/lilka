local test = sdcard.listDir("/")

print(#test)

sdcard.removeFile("test.txt")

local test = sdcard.listDir("/")

print(#test)

sdcard.openFile("test.txt", "RW")

sdcard.writeFile("TEST, HELLO WORLD")

test = sdcard.readFile()

print(test)

local test = sdcard.listDir("/")

print(#test)

sdcard.close()