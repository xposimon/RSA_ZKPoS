#include <fstream>
#include "RSA_ZKPoS.h"
#define SECURITY_BITS 10

int main()
{
    std::cout<<"RSA_ZKPoS test codes"<<std::endl;
    RSA_ZKPoS* rsa_test = new RSA_ZKPoS();
    rsa_test->keyGen(SECURITY_BITS/2);
    rsa_test->exportKeys();

    std::cout<<"File Read"<<std::endl;
    std::ifstream fin("../12800bytes.in", std::ios::binary);
    std::ofstream fout;
    std::vector<mpz_t> tags;
    std::vector<mpz_t> fileBlocks;

    if (fin)
    {
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
            fout.open("blockData.tmp", ios::binary|ios::out, 0);
            fout.write(&buffer[0], readSize);
            fout.close();
            pFile = fopen("blockData.tmp", "r");
            mpz_inp_raw(fileBlocks[count++], pFile);
        }
        tags.resize(length%BLOCKSIZE==0?length/BLOCKSIZE:((int)(length/BLOCKSIZE)+1));
        rsa_test->tagGen(fileBlocks, tags);
    }

    fin.close();

    return 0;
}