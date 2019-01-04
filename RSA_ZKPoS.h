#ifndef RSA_ZKPOS_RSA_ZKPOS_H
#define RSA_ZKPOS_RSA_ZKPOS_H
#define BLOCKSIZE 128
#include <gmp.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "common.h"

class Proof
{
public:
    mpz_t t, u, sigma;
    Proof() {mpz_init(t); mpz_init(u); mpz_init(sigma);}
    ~Proof() {mpz_clear(t); mpz_clear(u); mpz_clear(sigma);}
};

class safe_mpz
{// mpz_t has not a constructor, cannot use vector
public:
    mpz_t z;
    safe_mpz(){mpz_init(z);}
    ~safe_mpz(){//mpz_clear(z);
    }
};

class RSA_ZKPoS {
public:
    //int keyGen(mpz_t security_param);
    RSA_ZKPoS()
    {
        gmp_randinit_default(this->grt);
        mpz_init(this->p_plus); // initialize p and q
        mpz_init(this->q_plus);
        mpz_init(this->e);
        mpz_init(this->d);
        mpz_init(this->g1);
        mpz_init(this->g2);
        mpz_init(this->z1);
        mpz_init(this->z2);
        mpz_init(this->N);
        mpz_init(this->hash_value);

    }
    ~RSA_ZKPoS()
    {
        mpz_clear(this->p_plus); // clearialize p and q
        mpz_clear(this->q_plus);
        mpz_clear(this->e);
        mpz_clear(this->d);
        mpz_clear(this->g1);
        mpz_clear(this->g2);
        mpz_clear(this->z1);
        mpz_clear(this->z2);
        mpz_clear(this->N);
        mpz_clear(this->hash_value);
    }
    int keyGen(int k);// security param 1^k
    int tagGen(std::vector<safe_mpz>& file , std::vector<safe_mpz>& tags, std::vector<safe_mpz>& names, std::vector<safe_mpz>& r); //128bytes
    int prove(const std::vector<safe_mpz> coeff, const std::vector<safe_mpz> file, const mpz_t randomness, const std::vector<safe_mpz> tags, Proof& pi);
    int commit(mpz_t& commitment);
    int challenge(std::vector<safe_mpz>& coeff, mpz_t& R, int len);
    int verify(const std::vector<safe_mpz> coeff, const mpz_t R, const Proof pi, const std::vector<safe_mpz> files,const std::vector<safe_mpz> names, const mpz_t commitment_a);

    int dbg_importPk(int pp, int qp, int e, int d, int g1, int g2, int N)
    {
        mpz_set_ui(this->p_plus, pp);
        mpz_set_ui(this->q_plus, qp);
        mpz_set_ui(this->e, e);
        mpz_set_ui(this->d, d);
        mpz_set_ui(this->g1, g1);
        mpz_set_ui(this->g2, g2);
        mpz_set_ui(this->N, N);

    };
    int exportPk(std::string pkFileName = std::string("rsa_test.pk"));
    int exportSk(std::string skFileName = std::string("rsa_test.sk"));
    int exportKeys(); // both pk and sk to two files

private:
    bool isGoodNunber(mpz_t& a);
    int H(mpz_t z);
    mpz_t p_plus, q_plus, N, e, d, g1, g2, z1, z2;
    int k;
    mpz_t hash_value;
    gmp_randstate_t grt;
};


#endif //RSA_ZKPOS_RSA_ZKPOS_H
