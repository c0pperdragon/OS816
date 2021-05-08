def isprime(n):
    for f in range(2,n):
        if (n%f) == 0:
            return False
        if f*f>n:
            break
    return True        

print ("Verify results of the prime computation example")
count = 0
sum = 0
for i in range(2,50000):
    if isprime(i):
        count = count+1
        sum = sum+i

print ("Number of primes:",count,"Checksum",(sum%(1<<16)))
