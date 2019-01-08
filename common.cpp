//
// Created by nclab on 1/4/19.
//
#include "common.h"
int min(int a, int b)
{
    return a<b?a:b;
}

void mp2bits(const mpz_t z, std::vector<bool>& bits)
{
    if (!mpz_cmp_ui(z, 0))
    {
        bits.resize(1);
        bits[0] = 0;
        return;
    }

    unsigned long t;
    mp_size_t n = mpz_size(z);
    const mp_limb_t* zp = mpz_limbs_read(z); // a limb with 64 bits
    //gmp_printf("%d %Zd\n", n, z);
    bits.resize(n*64);
    for (int i = 0; i<n-1; i++)
    {
        t = zp[i];
        for (int j = 0; j < 64; j++)
        {
            bits[i*64+j] = (t&1);
            t >>= 1;
        }
    }
    t = zp[n-1];
    int count = 0;
    while (t > 0)
    {
        bits[(n-1)*64+count] = (t&1);
        t>>=1;
        count++;
    }
    bits.resize((n-1)*64+count);
}

void mp2bitString(const mpz_t z, std::string& bitString)
{
    bitString.clear();
    std::vector<bool> tmp;
    //gmp_printf("!!!%Zd\n", z);
    mp2bits(z, tmp);
    int len = tmp.size();
    char s[1] = "";
    for (int i = 0; i<len;i++) {
        s[0] = (char)((int)tmp[i]+(int)'0');
        bitString += std::string(s); // 2^n bit store in bisString[n]
    }
}

