with open('primes.txt','r') as primes:
    data = []
    for row in primes:
        data.append(int(row.strip()))

import itertools

def grouper(n, iterable):
    it = iter(iterable)
    while True:
       chunk = tuple(itertools.islice(it, n))
       if not chunk:
           return
       yield chunk

with open('primes2.txt', 'w') as primes:
    for chunk in grouper(6, data):
        primes.write("  ".join(str(x) for x in chunk) + "\n")