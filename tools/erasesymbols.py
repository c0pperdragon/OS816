import sys

def findfirst(data, pattern):
    for i in range(len(data)):
        if data[i] == pattern[0]:
            if data[i:i+len(pattern)] == pattern:
                return i
    return -1
    
f = open(sys.argv[1], "rb")
data = bytearray(f.read())
f.close()

for s in sys.argv[2:]:
    pattern = bytearray("~~"+s, 'utf-8')
    i = findfirst(data, pattern)
    if i>=0:
        data[i]=32    

f = open(sys.argv[1], "wb")
f.write(data)
f.close()
