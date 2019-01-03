#include <fstream>
#include "RSA_ZKPoS.h"
#define SECURITY_BITS 10

int main()
{
    std::cout<<"RSA_ZKPoS test codes"<<std::endl;
    RSA_ZKPoS* rsa_test = new RSA_ZKPoS();
    rsa_test->keyGen(SECURITY_BITS);
    rsa_test->exportKeys();

    std::cout<<"File Read"<<std::endl;
    std::ifstream fin("../12800bytes.in", std::ios::binary);
    std::ofstream fout;
    std::vector<mpz_t> tags;
    std::vector<mpz_t> fileBlocks;
    std::vector<mpz_t> names;

    if (fin)
    {
        // file handle to vector<mpz_t> files with each mpz_t content of a block
        fin.seekg(0, fin.end);
        int length = (int)fin.tellg();
        fin.seekg(0, fin.beg);
        std::string buffer(BLOCKSIZE, '\0');
        fileBlocks.resize(length%BLOCKSIZE==0?length/BLOCKSIZE:((int)(length/BLOCKSIZE)+1));
        fin.read(&buffer[0], length);
        FILE *pFile;
        //std::cout<<buffer.size()<<std::endl;
        int readSize = BLOCKSIZE;
        int count = 0;
        while (readSize > 0 && fin.read(&buffer[0], readSize))
        {
            readSize = max(length, BLOCKSIZE);
            length -= BLOCKSIZE;
            fout.open("blockData.tmp", std::ios::binary|std::ios::out, 0);
            fout.write(&buffer[0], readSize);
            fout.close();
            pFile = fopen("blockData.tmp", "r");
            mpz_init(fileBlocks[count]);
            mpz_inp_raw(fileBlocks[count], pFile);
            count ++;
        }
        tags.resize(length%BLOCKSIZE==0?length/BLOCKSIZE:((int)(length/BLOCKSIZE)+1));

        // generate tags
        rsa_test->tagGen(fileBlocks, tags, names);
    }
    fin.close(); // finish file handle

    mpz_t commitment;
    mpz_init(commitment);
    rsa_test->commit();

    //simulate challenge
    std::vector<mpz_t> coeff;
    mpz_t R;
    rsa_test->challenge(coeff, R);

    // generate proof
    Proof pi;
    rsa_test->prove(coeff, fileBlocks, R, tags, pi);

    //simulate verify
    printf("Verify result is %d", rsa_test->verify(coeff, R, pi, fileBlocks, names, commitment));

    for(int i = 0; i < fileBlocks.size(); i++)
        mpz_clear(fileBlocks[i]), mpz_clear(names[i]), mpz_clear(tags[i]), mpz_clear(coeff[i]);
    mpz_clear(R);
    mpz_clear(commitment);
    delete rsa_test;
    return 0;
}