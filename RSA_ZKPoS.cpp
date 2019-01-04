#include "RSA_ZKPoS.h"

bool RSA_ZKPoS::isGoodNunber(mpz_t& a)
{
    mpz_t tmp;
    mpz_init(tmp);
    if (!mpz_cmp_ui(a, 0))
        return false;
    // gcd(a+1, N) = 1, gcd(a-1, N) = 1, gcd(a, N) = 1
    mpz_set(tmp, a);
    mpz_gcd(tmp, tmp, this->N);
    if(mpz_cmp_ui(tmp, 1))
        return false;

    mpz_add_ui(tmp, a, 1);
    mpz_gcd(tmp, tmp, this->N);
    if(mpz_cmp_ui(tmp, 1))
        return false;

    mpz_sub_ui(tmp, a, 1);
    mpz_gcd(tmp, tmp, this->N);
    if(mpz_cmp_ui(tmp, 1))
        return false;

    return true;
}

int RSA_ZKPoS::H(mpz_t z)
{
    // TODO simple Hash, not secure, return g1^z mod N, which is QRn
    mpz_powm(this->hash_value, this->g1, z, this->N); // TODO not clear tmp, is there a destructor in mpz_t?
    return 1;
};

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

    }while(!mpz_cmp(p, q) || mpz_probab_prime_p(this->q_plus, 5)==0); // probability 4^(-5) not prime

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

    mpz_t a, group_order;
    mpz_init(a);
    mpz_init(group_order);
    mpz_mul(group_order, this->q_plus, this->p_plus); // a in p'q', a^2 mod N should be in Zn*

    do
    {
        mpz_urandomm(a, this->grt, this->N);
        mpz_mul(a, a, a);
        mpz_mod(a, a, this->N); // a is in QRn
    }
    while(!isGoodNunber(a));
    mpz_mul(a, a, a); // g = a^2
    mpz_set(this->g1, a);

    do
    {
        mpz_urandomm(a, this->grt, this->N);
        mpz_powm_ui(a, a, 2, this->N);
    }
    while(!isGoodNunber(a) || !mpz_cmp(this->g1, a));
    mpz_mul(a, a, a);
    mpz_set(this->g2, a);

    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(tmp);
    return 1; // finish tasks
}

int RSA_ZKPoS::tagGen(std::vector<safe_mpz> &file, std::vector<safe_mpz>& tags, std::vector<safe_mpz>& names, std::vector<safe_mpz>& r)
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
        mpz_rrandomb(names[i].z, this->grt, this->k);
        mpz_set_ui(mptmp, i);
        mp2bitString(names[i].z, t1);
        mp2bitString(mptmp, t2);
        t1 = t2+t1; // name||i bit concat
        std::reverse(t1.begin(), t1.end());

        mpz_set_str(mptmp, t1.c_str(), 2);
        mpz_powm(wasted_r, this->g1, file[i].z, this->N);
        this->H(mptmp);
        mpz_set(r[i].z, this->hash_value);
        mpz_mul(wasted_r, wasted_r, this->hash_value);
        mpz_powm(tags[i].z, wasted_r, this->d, this->N);
        mpz_mod(tags[i].z, tags[i].z, this->N);
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
    gmp_printf("z1: %Zd \nz2: %Zd \na: %Zd\n", this->z1, this->z2, commitment);
#ifdef DEBUG
    FILE *pFile=fopen("random.in", "w");
    fprintf(pFile, "z1:");
    mpz_out_str(pFile, 10, this->z1);
    fprintf(pFile, "\n");
    fprintf(pFile, "z2:");
    mpz_out_str(pFile, 10, this->z2);
    fprintf(pFile, "\n");
#endif

    mpz_clear(tmp);
}

int RSA_ZKPoS::challenge(std::vector<safe_mpz>& coeff, mpz_t& R, int len)
{
    gmp_randseed_ui(this->grt, time(nullptr));
    mpz_init(R);
    // simulate challenge
    coeff.resize(len);
    for( int i = 0; i < len; i++)
    {
        mpz_init(coeff[i].z);
        mpz_rrandomb(coeff[i].z, this->grt, this->k);
        //mpz_set_ui(coeff[i].z, 1);
    }
    mpz_rrandomb(R, this->grt, this->k);
    //mpz_set_ui(R, 1);
    return 1;
}

int RSA_ZKPoS::prove(const std::vector<safe_mpz> c, const std::vector<safe_mpz> files, const mpz_t randomness, const std::vector<safe_mpz> tags, Proof& pi)
{
    mpz_t tmp, t2;
    mpz_init(tmp);
    mpz_init(t2);
    gmp_randseed_ui(this->grt, time(nullptr));
    mpz_set(pi.sigma, this->z2);
    mpz_set(pi.u, this->z1);
#ifdef DEBUG
    mpz_rrandomb(pi.t, this->grt, this->k); // random sample p
    FILE *pFile=fopen("randomp.in", "w");
    fprintf(pFile, "p:");
    mpz_out_str(pFile, 10, this->z1);
    fprintf(pFile, "\n");
#endif
    mpz_mul(tmp, randomness, pi.t);
    mpz_add(pi.sigma, pi.sigma, tmp); // sigma = z2+R*p

    mpz_powm(pi.t, this->g2, pi.t, this->N); // g2^p


    int len = files.size();

    for (int i = 0 ; i < len; i++)
    {
        mpz_powm(tmp, tags[i].z, c[i].z, this->N);
        mpz_mul(pi.t, pi.t, tmp);
        mpz_mod(pi.t, pi.t, this->N); // t = t* mul_i(ti^ci) mod N
    }

    mpz_set_ui(tmp, 0);
    for (int i = 0; i < len; i++)
    {
        mpz_mul(t2, c[i].z, files[i].z);
        mpz_add(tmp, tmp, t2);
    }
    mpz_mul(tmp, tmp, randomness);
    mpz_add(pi.u, pi.u, tmp); // u = z1 + R*sum_i(ci*fi)

    mpz_clear(tmp);
    mpz_clear(t2);
    return 1;
}

int RSA_ZKPoS::verify(const std::vector<safe_mpz> coeff, const mpz_t R, const Proof pi, const std::vector<safe_mpz> files, const std::vector<safe_mpz> names, const mpz_t commitment_a)
{

    mpz_t left, right, t1, t2;
    mpz_init(left);
    mpz_init(right);
    mpz_init(t1);
    mpz_init(t2);
    mpz_mul(t1, this->e, R);
    mpz_powm(left, pi.t, t1, this->N);
    mpz_mul(left, left, commitment_a);
    mpz_mod(left, left, this->N); // left = a* pi^(eR) mod N

    mpz_mul(t1, this->e, pi.sigma);
    mpz_powm(t1, this->g2, t1, this->N);
    mpz_powm(right, this->g1, pi.u, this->N);
    mpz_mul(right, right, t1);
    mpz_mod(right, right, this->N);

    int len = names.size();
    std::string s1, s2;
    for (int i = 0; i < len; i++)
    {
        mpz_set_ui(t2, i);
        mp2bitString(names[i].z, s1);
        mp2bitString(t2, s2);
        s1 = s2+s1; // name||i bit concat
        std::reverse(s1.begin(), s1.end());
        mpz_set_str(t1, s1.c_str(), 2);
        this->H(t1);

        mpz_mul(t1, coeff[i].z, R);
        mpz_powm(t2, this->hash_value, t1, this->N); // ri^ci
        mpz_mul(right, right, t2);
        mpz_mod(right, right, this->N); // right = g1^u * g2^(e*sigma) * (mul_i(ri^ci)) mod N
    }


    gmp_printf("\nleft a*t^(eR) = %Zd\nright:g1^u*g2^(e*sigma)*mul_i(ri^(ci*R)) = %Zd\n", left, right);

    bool res = (mpz_cmp(left,right)==0);

    mpz_clear(left);
    mpz_clear(right);
    mpz_clear(t1);
    mpz_clear(t2);
    return res;
}