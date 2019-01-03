#include <fstream>
#include "RSA_ZKPoS.h"
#define SECURITY_BITS 6

int min(int a, int b)
{
    return a<b?a:b;
}

int main()
{
    std::cout<<"RSA_ZKPoS test codes"<<std::endl;
    RSA_ZKPoS* rsa_test = new RSA_ZKPoS();
    rsa_test->keyGen(SECURITY_BITS);
    rsa_test->exportKeys();

    // rsa_test->dbg_importPk(5, 5, 163, 2, 1024, 4761);

    std::cout<<"File Read"<<std::endl;

    std::ifstream fin("../1.in", std::ios::binary);

    std::vector<safe_mpz> tags;
    std::vector<safe_mpz> fileBlocks;
    std::vector<safe_mpz> names;

    if (fin)
    {
        // file handle to vector<mpz_t> files with each mpz_t content of a block

        fin.seekg(0, fin.end);
        int length = (int)fin.tellg();
        fin.seekg(0, fin.beg);
        std::string buffer(BLOCKSIZE, '\0');
        //std::cout<<length<<std::endl;
        fileBlocks.resize(length%BLOCKSIZE==0?length/BLOCKSIZE:((int)(length/BLOCKSIZE)+1));

        //std::cout<<buffer.size()<<std::endl;
        int readSize = min(length,BLOCKSIZE);
        int count = 0, base = 1;
        //std::cout<<readSize<<std::endl;
        while (readSize > 0 && fin.read(&buffer[0], readSize))
        {
            mpz_set_ui(fileBlocks[count].z, 0);
            base = 1;
            //std::cout<< (int)buffer[0]<< " "<<count<<std::endl;
            for (int i = 0; i < readSize; i++){
                mpz_add_ui(fileBlocks[count].z, fileBlocks[count].z, base*(int)buffer[i]);
                base *= 256;
            }

            length -= BLOCKSIZE;
            readSize = min(length, BLOCKSIZE);
            count ++;
        }

        tags.resize(count);
        names.resize(count);

        // generate tags
        rsa_test->tagGen(fileBlocks, tags, names);
        printf("Tag gen:\n");
        for (int i = 0; i < count; i++)
            gmp_printf("%Zd %Zd %Zd\n", fileBlocks[i].z, tags[i].z, names[i].z);
    }
    fin.close(); // finish file handle

    mpz_t commitment;
    mpz_init(commitment);
    rsa_test->commit(commitment);

    gmp_printf("Commitment: %Zd\n", commitment);


    //simulate challenge
    std::vector<safe_mpz> coeff;
    mpz_t R;
    rsa_test->challenge(coeff, R, fileBlocks.size());
    gmp_printf("Challenge with R=%Zd:\n", R);
    for (int i = 0; i < fileBlocks.size(); i++)
        gmp_printf("%Zd \n", coeff[i].z);

    // generate proof
    Proof pi;
    rsa_test->prove(coeff, fileBlocks, R, tags, pi);

    gmp_printf("Proof: (t, u, sigma) = (%Zd, %Zd, %Zd)\n", pi.t, pi.u, pi.sigma);

    //simulate verify
    int res = rsa_test->verify(coeff, R, pi, fileBlocks, names, commitment);
    printf("111111\n");
    printf("Verify result is %d", res);

//    for(int i = 0; i < fileBlocks.size(); i++)
//        mpz_clear(fileBlocks[i]), mpz_clear(names[i]), mpz_clear(tags[i]), mpz_clear(coeff[i]);
    mpz_clear(R);
    mpz_clear(commitment);
    delete rsa_test;
    return 0;
}