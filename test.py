#coding=utf-8
#python 2.7.15

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
    N = checkList['N']

    # cyclic group check
    i = 1
    pi = g1
    while True:
        if (pi % N == 1):
            break
        assert (pi % N != 0), "g1^%d devide p'"%i
        pi = (pi*g1) % N
        i += 1

    i = 1
    pi = g2
    while True:
        if (pi % N == 1):
            break
        assert (pi % N != 0), "g2^%d devide p'"%i
        pi = (pi*g2) % N
        i += 1