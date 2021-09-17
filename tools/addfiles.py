import sys

def hextobytes(line):
    b = []
    if len(line)>=5 and line[0]==':':
        for i in range ((len(line)-3)>>1):
            b.append(int(line[1+2*i:3+2*i],16))
    return b        

def printhex(f, data):
    chk = 0
    print (":", end="", file=f)
    for b in data:
        print ('{:02X}'.format(b), end="", file=f)
        chk = (chk + b) & 0xFF
    print ('{:02X}'.format((256-chk)&0xFF), file=f)
        

def readhexfile(filename):
    rom = [None]*(1<<19)
    if filename.lower().endswith(".hex"):
        print ("Reading HEX",filename)
        f = open(filename, "r")
        segment = 0
        for line in f:
            b = hextobytes(line.strip())
            if len(b)>=6 and b[3]==2:
                segment = (b[4]*256+b[5]) * 16
            elif len(b)>4 and b[3]==0 and len(b)>=b[0]+4:
                a = b[1]*256 + b[2];
                for i in range(b[0]):
                    addr = segment+a+i
                    if rom[addr]!=None:
                        print ("Rom definition conflict:",addr)
                    else:
                        rom[addr] = b[4+i]
        f.close()
    else:
        print ("Can not read format: "+filename);
    return rom

def containanything(data, start, len):
    for a in range(start,start+len):
        if data[a]!=None:
            return True
    return False

def writehexfile(filename, data):
    f = open(filename, "w");
    currentbank = None
    for a in range(0, len(data), 32):
        if containanything(data, a, 32):
            bank = a // 65536
            if bank!=currentbank:
                printhex(f, [2, 0, 0, 0x02, bank*16, 0] )
                currentbank = bank
            run = []
            for i in range(0,32):
                if data[a+i]!=None:
                    while len(run)<i:
                        run.append(0xFF)
                    run.append(data[a+i])
            printhex (f, [len(run), (a>>8)&0xFF, a&0xFF, 0] + run )
    printhex (f, [0, 0, 0, 0x01] )
    f.close()
    
def firstfree(data):
    for i in range(len(data)-1, 0x1000, -1):
        if data[i]!=None:
            return i+1
    print("Cannot determine end of code range")
    return 0x10000

def injectbyte(rom,pos,b):
    if pos<0x10000 or pos>=0x8FC00:
        raise IndexError
    rom[pos & 0x7FFFF] = b

def inject(rom, pos, filename, data):
    header = [0x46, 0x49, 0x4C, 0x45, 0,0,0,0] + [ord(c) & 0xff for c in filename] + [0]
    l = len(header) + len(data)
    header[4] = l&0xff
    header[5] = (l>>8)&0xff
    header[6] = (l>>16)&0xff
    header[7] = (l>>24)&0xff
    for i in range(len(header)):
        injectbyte(rom,pos+i,header[i])
    for i in range(len(data)):
        injectbyte(rom,pos+len(header)+i,data[i])
    return pos+l

def trimfolder(filename):
    idx = filename.rfind('/')
    return filename[(idx+1):] if idx>=0 else filename


rom = readhexfile(sys.argv[1])
freearea = firstfree(rom)

pos = ((freearea+4095)//4096)*4096
for filename in sys.argv[2:]:
    print ("Add file: "+filename)
    f = open(filename, "rb")
    data = f.read()
    f.close()
    pos = inject(rom, pos, trimfolder(filename), data)

writehexfile(sys.argv[1], rom)    
