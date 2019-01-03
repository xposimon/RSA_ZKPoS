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
#include "common.h"
class pk
{
public:
    pk(){mpz_init(N); mpz_init(g1); mpz_init(g2); mpz_init(e);}
    ~pk(){mpz_clear(N); mpz_clear(g1); mpz_clear(g2); mpz_clear(e);}
    mpz_t N, g1, g2, e;
    //mpz_t H(std::vector<bool> bytes){} // random oracle
};

class sk
{
public:
    sk(){mpz_init(d);}
    ~sk(){mpz_clear(d);}
    mpz_t d;
};

class Proof
{
public:
    mpz_t t, u, sigma;
    Proof() {mpz_init(t); mpz_init(u); mpz_init(sigma);}
    ~Proof() {mpz_clear(t); mpz_clear(u); mpz_clear(sigma);}
};

class RSA_ZKPoS {
public:
    //int keyGen(mpz_t security_param);
    RSA_ZKPoS()
    {
        gmp_randinit_default(grt);
        mpz_init(this->p_plus); // initialize p and q
        mpz_init(this->q_plus);
        mpz_init(this->e);
        mpz_init(this->d);
        mpz_init(this->g1);
        mpz_init(this->g2);
        mpz_init(this->z1);
        mpz_init(this->z2);
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
    }
    int keyGen(int k);// security param 1^k
    int tagGen(std::vector<mpz_t>& file , std::vector<mpz_t>& tags, std::vector<mpz_t>& names); //128bytes
    int prove(const std::vector<mpz_t> coeff, const std::vector<mpz_t> file, const mpz_t randomness, const std::vector<mpz_t> tags, Proof& pi);
    int commit(mpz_t commitment);
    int challenge();

    int exportPk(std::string pkFileName = std::string("rsa_test.pk"));
    int exportSk(std::string skFileName = std::string("rsa_test.sk"));
    int exportKeys(); // both pk and sk to two files

private:
    bool isGoodNunber(mpz_t& a);
    inline mpz_t H(mpz_t z)
    {
        mpz_t tmp;
        mpz_init(tmp);
        // TODO simple Hash, not secure, return g1^z mod N, which is QRn
        mpz_powm(tmp, this->g1, tmp, this->N); // TODO not clear tmp, is there a destructor in mpz_t?
        return tmp;
    }
    mpz_t p_plus, q_plus, N, e, d, g1, g2, z1, z2;
    int k;
    gmp_randstate_t grt;
};


#endif //RSA_ZKPOS_RSA_ZKPOS_H
