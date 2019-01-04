//
// Created by nclab on 1/3/19.
//
//#define DEBUG

#ifndef RSA_ZKPOS_COMMON_H
#define RSA_ZKPOS_COMMON_H
#include <gmp.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>


void mp2bits(const mpz_t z, std::vector<bool>& bits);

void mp2bitString(const mpz_t z, std::string& bitString);

#endif //RSA_ZKPOS_COMMON_H
