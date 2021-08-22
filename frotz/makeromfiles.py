
def gen(data,f):
    bl = 65536
    for start in range(0, len(data), bl):
        stop = min(start+bl,len(data))
        print("ROMFS", end="", file=f)
        print((start//bl), end="", file=f)
        print(" SECTION" , file=f)
        print("    ORG ",end="",file=f)
        print((0x810000+start), end="", file=f)
        for i in range(start,stop):
            if (i-start)%16 == 0:
                print ("\n    DB ", end="", file=f)
            print (data[i], end="", file=f)
            if (i-start)%16 != 15 and i!=stop-1:
                print(",", end="", file=f)
        print("", file=f)
        print("    ENDS", file=f)
    print("END", file=f)

def append(data, filename, shortname):
    f = open(filename, "rb")
    bytes = f.read()
    f.close()
    l = 9 + len(shortname) + len(bytes) 
    header = bytearray(
        [ 70, 73, 76, 69,                               # magic code: "FILE"
          l&0xff, (l>>8)&0xff, (l>>16)&0xff, (l>>24)&0xff  # total length
        ]
    )
    for i in range(len(shortname)):
        header.append(ord(shortname[i]))
    header.append(0)
    return data + header + bytes

data = bytearray();
data = append(data, "games/zork1.z3", "zork1");
data = append(data, "games/hhgg.z3", "hitchhiker");
data = append(data, "games/zork1.sav", "story.sav");

f = open("romfiles.asm", mode='w', encoding='utf-8')
gen(data,f)
f.close()
