readFrom = input("file to read\n")
writeTo = input("file to write\n")

textFileRead = open(readFrom, "r")
lines = textFileRead.readlines()
textFileRead.close()

TextFileWrite = open(writeTo, "w")

init = True
lastChar = ''



for i in lines:
    for j in i:
        if(not init):
            if (lastChar != '\n'):
                TextFileWrite.write(",")
        else:
            init = False
        TextFileWrite.write(j)
        lastChar = j

TextFileWrite.close()
