//
// Created by nclab on 1/3/19.
//

#ifndef RSA_ZKPOS_COMMON_H
#define RSA_ZKPOS_COMMON_H
#include <gmp.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>

void mp2bytes(const mpz_t z, std::vector<unsigned char>& bytes)
{
    mp_size_t n = mpz_size(z);
    const mp_limb_t* zp = mpz_limbs_read(z, n);
    //gmp_printf("%d %Zd\n", n, z);
}

void bytes2bits(const std::vector<unsigned char> bytes, std::vector<bool>& bits)
{

}

void mp2bits(const mpz_t z, std::vector<bool>& bits)
{
    unsigned long t;
    mp_size_t n = mpz_size(z);
    const mp_limb_t* zp = mpz_limbs_read(z, n); // a limb with 64 bits
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
}

void bits2string(const std::vector<bool> bits, std::string& bitString)
{

}

void mp2bitString(const mpz_t z, std::string& bitString)
{
    bitString.clear();
    std::vector<bool> tmp;
    mp2bits(z, tmp);
    len = tmp.size();
    for (int i = 0; i<len;i++) {
        bitString += std::string(tmp[i]+'0'); // 2^n bit store in bisString[n]
    }
}


#endif //RSA_ZKPOS_COMMON_H
