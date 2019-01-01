#include "RSA_ZKPoS.h"

bool RSA_ZKPoS::isGoodNunber(mpz_t& a)
{
    mpz_t tmp;
    mpz_init(tmp);

    // gcd(a+1, q') = 1, gcd(a-1, q') = 1, gcd(a, q') = 1
    mpz_set(tmp, a);
    if(!mpz_divisible_p(tmp, this->p_plus))
        return false;
    mpz_add_ui(tmp, a, 1);
    if(!mpz_divisible_p(tmp, this->p_plus))
        return false;
    mpz_sub_ui(tmp, a, 1);
    if(!mpz_divisible_p(tmp, this->p_plus))
        return false;

    // gcd(a+1, q') = 1, gcd(a-1, q') = 1, gcd(a, q') = 1
    mpz_set(tmp, a);
    if(!mpz_divisible_p(tmp, this->q_plus))
        return false;
    mpz_add_ui(tmp, a, 1);
    if(!mpz_divisible_p(tmp, this->q_plus))
        return false;
    mpz_sub_ui(tmp, a, 1);
    if(!mpz_divisible_p(tmp, this->q_plus))
        return false;

    return true;
}

int RSA_ZKPoS::exportPk(std::string pkFileName)
{
    FILE *pFile;
    pFile = fopen(pkFileName.c_str(), "w");
    /* not needed in the pk*/
    fprintf(pFile, "p':");
    mpz_out_str(pFile, 10, this->p_plus);
    fprintf(pFile,"\nq':");
    mpz_out_str(pFile, 10, this->q_plus);
    fprintf(pFile,"\nk:%d", this->k);
    fprintf(pFile,"\ne:");
    /**/
    mpz_out_str(pFile, 10, this->e);
    fprintf(pFile,"\ng1:");
    mpz_out_str(pFile, 10, this->g1);
    fprintf(pFile,"\ng2:");
    mpz_out_str(pFile, 10, this->g2);
    fprintf(pFile,"\nN:");
    mpz_out_str(pFile, 10, this->N);
    fclose(pFile);
    return 1;
}

int RSA_ZKPoS::exportSk(std::string skFileName)
{
    FILE *pFile;
    pFile = fopen(skFileName.c_str(), "w");
    mpz_out_str(pFile, 10, this->d);
    fclose(pFile);
    return 1;
}

int RSA_ZKPoS::exportKeys()
{
    this->exportPk();
    this->exportSk();
}

int RSA_ZKPoS::keyGen(int k)
{
    this->k = 2*k; // TODO delete it

    std::cout<<"Key setup bits:"<<k<<std::endl;
    mpz_t p, q, tmp, pow;
    mpz_init(tmp);
    mpz_init(pow);
    mpz_init(p);
    mpz_init(q);

    gmp_randseed_ui(this->grt, time(NULL));

    do {
        mpz_rrandomb(p, this->grt, k + 1);
        mpz_nextprime(p, p);
        mpz_sub_ui(tmp, p, 1);
        mpz_divexact_ui(this->p_plus, tmp, 2);
    }while(mpz_probab_prime_p(this->p_plus, 5)==0);

    do
    {
        mpz_rrandomb(q, this->grt, k+1);
        mpz_nextprime(q, q);
        mpz_sub_ui(tmp, q, 1);
        mpz_divexact_ui(this->q_plus, tmp, 2);

    }while(mpz_probab_prime_p(this->q_plus, 5)==0); // probability 4^(-5) not prime

    gmp_printf("p:%Zd \nq:%Zd\n", p, q);
    mpz_mul(this->N, p, q);


    // the e is between [2^(k+1), 2^(2k)+2^(k+1)]
    int e_bit = rand()%(4*k);

    mpz_set_ui(this->e, 1);
    mpz_mul_2exp(this->e, this->e, 2*k+1);
    mpz_urandomb(tmp, this->grt, e_bit);
    mpz_add(this->e, this->e, tmp);
    mpz_nextprime(this->e, this->e);

    // compute d
    mpz_mul(tmp, this->p_plus, this->q_plus);
    mpz_invert(this->d, this->e, tmp);

    mpz_t a;
    mpz_init(a);

    do
    {
        mpz_urandomm(a, this->grt, this->N);
    }
    while(isGoodNunber(a));
    mpz_set(this->g1, a);

    do
    {
        mpz_urandomm(a, this->grt, this->N);
    }
    while(isGoodNunber(a) and mpz_cmp(this->g1, a));
    mpz_set(this->g2, a);

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(tmp);
    return 1; // finish tasks
}

int RSA_ZKPoS::tagGen(std::vector<mpz_t> &file, std::vector<mpz_t>& tags)
{
    
}