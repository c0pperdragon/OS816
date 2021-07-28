
def gen(data,f):
    bl = 65536
    for start in range(0, len(data), bl):
        print("FILES", end="", file=f)
        print((start//bl), end="", file=f)
        print(" SECTION" , file=f)
        print("    ORG ",end="",file=f)
        print((0x840000+start), file=f)    
        for i in range(start, min(start+bl,len(data))):
            if i%16 == 0:
                print ("\n    DB ", end="", file=f)
            print (data[i], end="", file=f)
            if i%16 != 15:
                print(",", end="", file=f)
        print("", file=f)
        print("    ENDS", file=f)
    print("END", file=f)



f = open("games/wishbringer.z3", "rb")
data = f.read()
f.close()

l = len(data)
header = bytearray( [l&0xff, (l>>8)&0xff, (l>>16)&0xff, (l>>24)&0xff] )

f = open("files.asm", mode='w', encoding='utf-8')
gen(header+data,f)
f.close()
