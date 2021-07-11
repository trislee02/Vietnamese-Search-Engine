import os

#path = "C:/Users/Admin/source/repos/Project2/Project2/corpus"

path = "corpus\\"

#we shall store all the file names in this list
filelist = []

for root, dirs, files in os.walk(path):
	for file in files:
		filelist.append(os.path.join(root,file))
        #filelist.append(file)
        
f = open("i.txt", "w")
#print all the file names
for name in filelist:
    f.write(str(name) + '\n')
f.close()