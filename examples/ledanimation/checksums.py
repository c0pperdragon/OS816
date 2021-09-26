def checksum(inc,base):
    v = base
    z1 = 0
    z2 = 0
    for x in range(32768):
        z1 = (z1+v)  & 0xFFFF
        z2 = (z2+z1) & 0xFFFF
        v =  (v+inc) & 0xFFFF
    print(inc,base, (z1<<16) + z2)


checksum(9,47)
checksum(7,99)
checksum(13,200)
checksum(21,77)
checksum(32074,1235)
checksum(6786,105)
checksum(3274,1412)



