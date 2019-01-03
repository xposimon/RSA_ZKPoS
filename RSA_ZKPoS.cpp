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
    this->k = k;
    k = k/2; // generate k/2 bits of p and q
    std::cout<<"Key setup with p,q bits:"<<k<<std::endl;
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
    mpz_mul(a, a, a); // g = a^2
    mpz_set(this->g1, a);

    do
    {
        mpz_urandomm(a, this->grt, this->N);
    }
    while(isGoodNunber(a) and mpz_cmp(this->g1, a));
    mpz_mul(a, a, a);
    mpz_set(this->g2, a);

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(tmp);
    return 1; // finish tasks
}

int RSA_ZKPoS::tagGen(std::vector<mpz_t> &file, std::vector<mpz_t>& tags, std::vector<mpz_t>& names)
{
    int len = file.size();
    tags.resize(len);
    names.resize(len);
    gmp_randseed_ui(this->grt, time(NULL));

    mpz_t wasted_r, mptmp;
    mpz_init(wasted_r);
    mpz_init(mptmp);
    std::string t1, t2;
    for (int i = 0; i < len; i++)
    {
        mpz_init(tags[i]);
        mpz_init(names[i]);
        mpz_rrandomb(names[i], this->grt, this->k);
        mpz_set(mptmp, i);
        mp2bitString(name[i], t1);
        mp2bitString(mptmp, t2);
        t1 = t1+t2; // name||i bit concat
        mpz_set_str(mptmp, t1, 2);

        mpz_powm(wasted_r, this->g1, file[i], this->N);
        mpz_mul(wasted_r, this->H(mptmp));
        mpz_mul(tags[i], wasted_r, this->d, this->N);
        mpz_mod(tags[i], tags[i], this->N);
    }

    mpz_clear(wasted_r);
    mpz_clear(mptmp);
    return 1;
}

int RSA_ZKPoS::commit(mpz_t& commitment)
{
    mpz_t tmp;
    mpz_init(tmp);
    mpz_init(commitment);

    gmp_randseed_ui(this->grt, time(nullptr));
    mpz_rrandomb(this->z1, this->grt, k);
    mpz_rrandomb(this->z2, this->grt, k);
    mpz_powm(commitment, this->g1, this->z1, this->N);
    mpz_mul(tmp, this->e, this->z2);
    mpz_powm(tmp, this->g2, tmp, this->N);
    mpz_mul(commitment, commitment, tmp);
    mpz_mod(commitment, commitment, this->N);

    mpz_clear(tmp);
}

int RSA_ZKPoS::prove(const std::vector<mpz_t> c, const std::vector<mpz_t> files, const mpz_t randomness, const std::vector<mpz_t> tags, Proof& pi)
{
    mpz_t tmp, t2;
    mpz_init(tmp);
    mpz_init(t2);
    gmp_randseed_ui(this->grt, time(nullptr));
    mpz_set_ui(pi.sigma, this->z2);
    mpz_set_ui(pi.u, this->z1);
    mpz_rrandomb(pi.t, this->grt, this->k); // random sample p

    mpz_mul(tmp, randomness, pi.t);
    mpz_add(pi.sigma, pi.sigma, tmp); // sigma = z2+R*p

    mpz_powm(pi.t, this->g2, pi.t, this->N); // g2^p


    int len = file.size();

    for (int i = 0 ; i < len; i++)
    {
        mpz_powm(tmp, tags[i], c[i], this->N);
        mpz_mul(pi.t, pi.t, tmp);
        mpz_mod(pi.t, pi.t, this->N); // t = t* mul_i(ti^ci) mod N
    }

    mpz_set_ui(tmp, 0);
    for (int i = 0; i < len; i++)
    {
        mpz_mul(t2, c[i], files[i]);
        mpz_add(tmp, tmp, t2);
    }
    mpz_mul(tmp, tmp, randomness);
    mpz_add(pi.u, pi.u, tmp); // u = z1 + R*sum_i(ci*fi)

    mpz_clear(tmp);
    mpz_clear(t2);
    return 1;
}