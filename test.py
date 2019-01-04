#coding=utf-8
#python 2.7.15
from math import sqrt
checkList = {}
with open("./cmake-build-debug/rsa_test.pk") as f:
    for line in f.readlines():
        tmp = line.split(":")
        checkList[tmp[0]] = int(tmp[1].strip())

with open("./cmake-build-debug/rsa_test.sk") as f:
    checkList['d'] = int(f.read().strip())

if __name__ == "__main__":
    print checkList
    # N = (2p'+1)(2q'+1)
    assert ((2*checkList["p'"]+1) * (2*checkList["q'"]+1) == checkList['N']), "(2p'+1) * (2q'+1) != N"
    # p'q' in [2^(k-1), 2^k-1]
    assert ((checkList["p'"]) * (checkList["q'"]) >= pow(2, checkList["k"]-1)) and ((checkList["p'"]) * (checkList["q'"]) <= (pow(2, checkList["k"])-1)), "2^(k-1):%d<= p'*q':%d<=2^k-1:%d"%(pow(2, checkList["k"]-1), checkList["p'"]*checkList["q'"], pow(2, checkList["k"])-1)
    # e > 2^k
    assert (checkList["e"] > pow(2, checkList['k'])), "e<=2^k"
    # ed = 1 mod p'q'
    assert ((checkList['e']*checkList['d']) % (checkList["p'"]*checkList["q'"]) == 1), "ed:%d != 1(mod p'*q':%d)"%(checkList['e']*checkList['d'], checkList["p'"]*checkList["q'"])

    g1 = checkList['g1']
    g2 = checkList['g2']
    e = checkList['e']
    N = checkList['N']

    for i in range(2, N):
        if N % i == 0:
            print i

    i = 2
    record = {1:1}
    for i in range(1, N):
        record[i*i%N] = 1
    print i, len(record)

    # cyclic group check
    i = 1
    pi = g1
    while True:
        if (pi % N == 1):
            break
        assert (pi % N != 0), "g1^%d devide p'"%i
        pi = pi %N
        #print i
        #a = record[pi]
        #assert (pi not in record), "g1^%d: not in QRn"%i
        pi = (pi*g1) % N
        i += 1
    assert checkList["p'"]*checkList["q'"] == i, "order is p'q'"

    i = 1
    pi = g2
    while True:
        if (pi % N == 1):
            break
        assert (pi % N != 0), "g2^%d devide p'"%i
        #print record[pi]
        #assert (pi%N) not in record, "g2^%d not in QRn"%i
        pi = (pi*g2) % N
        i += 1
    assert checkList["p'"]*checkList["q'"] == i, "order is p'q'"

    with open("./cmake-build-debug/random.in") as f:
        for line in f.readlines():
            tmp = line.split(":")
            checkList[tmp[0]] = int(tmp[1].strip())

    with open("./cmake-build-debug/randoma.in") as f:
        for line in f.readlines():
            tmp = line.split(":")
            checkList[tmp[0]] = int(tmp[1].strip())

    with open("./cmake-build-debug/randomp.in") as f:
        for line in f.readlines():
            tmp = line.split(":")
            checkList[tmp[0]] = int(tmp[1].strip())

    with open("./cmake-build-debug/paraminfo.in") as f:
        for line in f.readlines():
            tmp = line.split(":")
            checkList[tmp[0]] = tmp[1].rstrip().split(" ")
            print tmp[1]
            for i in range(len(checkList[tmp[0]])):
                checkList[tmp[0]][i] = int(checkList[tmp[0]][i].strip())

    t, u, sigma = checkList['t'], checkList['u'], checkList['sigma']
    a = checkList['a']
    z1, z2 = checkList['z1'], checkList['z2']
    p = checkList['p']
    file = checkList['file']
    r = checkList['r']
    tags = checkList['tags']
    names = checkList['names']
    pp = checkList["p'"]
    qp = checkList["q'"]

    print checkList
    tmp = 1
    for i in range(z1):
        tmp = tmp * g1 % N
    tmpa = tmp

    tmp = 1
    cnt = z2 * e
    i = 0
    while i < cnt:
        tmp = tmp * g2 % N
        i+= 1
    tmpa *= tmp

    tmpa = tmpa % N
    assert tmpa == a

    # a correct, sigma correct,
    assert z2+p == sigma

    for i in range(len(file)):
        tmp = 1
        tmp = tmp * r[i] % N
        tmp = tmp *pow(g1, file[i], N)
        tmp = pow(tmp, checkList['d'], N)
        # print tmp, tags[i]
        assert tmp == tags[i]
    # tag correct

    tmp = 1
    for i in range(len(r)):
        flag =False
        for j in range(pp*qp):
            if (tmp == r[i]):
                flag = True
                break
            tmp = tmp * g1 % N
        assert flag
    # r correct in QRn

    tmp = pow(g2, p, N)
    for i in range(len(tags)):
        tmp = tmp*pow(tags[i], 1, N) % N# should be c[i], where is 1 now
    assert tmp == t
    # t correct

    tmp = 0
    for i in range(len(file)):
        tmp = 1 * file[i]# 1 is c[i]
    tmp = tmp * 1 # 1 is R
    tmp = (tmp + z1)
    assert tmp == u
    # u correct

    tmp = pow(t, e*1, N) # 1 is R
    tmp = a*tmp % N
    print (tmp)

    tmp = pow(g1, u, N)
    tmp = tmp * pow(g2, e*sigma, N) % N
    print(tmp)
    for i in range(len(r)):
        tmp = tmp*pow(r[i], 1, N) % N
    print (tmp)
