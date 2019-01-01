#ifndef RSA_ZKPOS_RSA_ZKPOS_H
#define RSA_ZKPOS_RSA_ZKPOS_H
#include <gmp.h>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <string>

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
    }
    ~RSA_ZKPoS()
    {
        mpz_clear(this->p_plus); // clearialize p and q
        mpz_clear(this->q_plus);
        mpz_clear(this->e);
        mpz_clear(this->d);
        mpz_clear(this->g1);
        mpz_clear(this->g2);
    }
    int keyGen(int k);// security param 1^k
    int tagGen(std::vector<unsigned char>& file_bytes);
    int commit();
    int challenge();
    int exportPk(std::string pkFileName = std::string("rsa_test.pk"));
    int exportSk(std::string skFileName = std::string("rsa_test.sk"));
    int exportKeys(); // both pk and sk to two files

private:
    bool isGoodNunber(mpz_t& a);
    mpz_t p_plus, q_plus, N, e, d, g1, g2;
    int k; // TODO debug use; delete later
    gmp_randstate_t grt;
};


#endif //RSA_ZKPOS_RSA_ZKPOS_H
