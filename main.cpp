#include <fstream>
#include "RSA_ZKPoS.h"
#define SECURITY_BITS 2048

int main()
{
    //std::cout<<"RSA_ZKPoS test codes"<<std::endl;
    RSA_ZKPoS* rsa_test = new RSA_ZKPoS();
#ifdef TEST
    clock_t startTime, endTime;
    std::vector<double> stepTime;
    int file_length;
    startTime = clock();
#endif
    rsa_test->keyGen(SECURITY_BITS);
#ifdef TEST
    endTime = clock();
    stepTime.push_back((double)(endTime-startTime)/CLOCKS_PER_SEC);
#endif
    rsa_test->exportKeys();

    // rsa_test->dbg_importPk(5, 5, 163, 2, 1024, 4761);

    std::cout<<"\nReading File: "<<std::endl;
    std::string filepath = std::string("../inputdata/10MB.in");
    std::ifstream fin(filepath.c_str(), std::ios::binary);

    std::vector<safe_mpz> tags;
    std::vector<safe_mpz> fileBlocks;
    std::vector<safe_mpz> names;
    std::vector<safe_mpz> r;
    std::vector<int> index;

    if (fin)
    {
        // file handle to vector<mpz_t> files with each mpz_t content of a block

        fin.seekg(0, fin.end);
        int length = (int)fin.tellg();
        file_length = length;
        std::cout<<"File length: "<<length<<std::endl;
        fin.seekg(0, fin.beg);
        std::string buffer(BLOCKSIZE, '\0');
        fileBlocks.resize(length%BLOCKSIZE==0?length/BLOCKSIZE:((int)(length/BLOCKSIZE)+1));

        int readSize = min(length,BLOCKSIZE);
        int count = 0, base = 1;

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
        r.resize(count);

        // generate tags
#ifdef TEST
        std::cout<<"Start tag generation"<<std::endl;
        startTime = clock();
#endif
        rsa_test->tagGen(fileBlocks, tags, names, r);
#ifdef TEST
        endTime = clock();
        stepTime.push_back((double)(endTime-startTime)/CLOCKS_PER_SEC);
#endif
    }
    fin.close(); // finish file handle

    mpz_t commitment;
    mpz_init(commitment);
    rsa_test->commit(commitment);

#ifdef DEBUG
    FILE *pFile=fopen("randoma.in", "w");
    fprintf(pFile, "a:");
    mpz_out_str(pFile, 10, commitment);
    fprintf(pFile, "\n");
#endif
    //simulate challenge
    std::vector<safe_mpz> coeff;
    mpz_t R;
    rsa_test->challenge(index, coeff, R, fileBlocks.size());
    gmp_printf("\nChallenge with R=%Zd:\n", R);
#ifdef TEST
    startTime = clock();
#endif
    // generate proof
    Proof pi;
    rsa_test->prove(index, coeff, fileBlocks, R, tags, pi);
#ifdef TEST
    endTime = clock();
    stepTime.push_back((double)(endTime-startTime)/CLOCKS_PER_SEC);
#endif

    gmp_printf("Proof: (t, u, sigma) = (%Zd, %Zd, %Zd)\n", pi.t, pi.u, pi.sigma);
#ifdef DEBUG
    fprintf(pFile, "t:");
    mpz_out_str(pFile, 10, pi.t);
    fprintf(pFile, "\n");
    fprintf(pFile, "sigma:");
    mpz_out_str(pFile, 10, pi.sigma);
    fprintf(pFile, "\n");
    fprintf(pFile, "u:");
    mpz_out_str(pFile, 10, pi.u);
    fprintf(pFile, "\n");
    fprintf(pFile, "R:");
    mpz_out_str(pFile, 10, R);
    fprintf(pFile, "\n");


    pFile=fopen("paraminfo.in", "w");
    fprintf(pFile, "file:");
    for (int i = 0; i < fileBlocks.size(); i++)
        mpz_out_str(pFile, 10, fileBlocks[i].z), fprintf(pFile, " ");
    fprintf(pFile,"\n");

    fprintf(pFile, "tags:");
    for (int i = 0; i < fileBlocks.size(); i++)
        mpz_out_str(pFile, 10, tags[i].z), fprintf(pFile, " ");
    fprintf(pFile,"\n");

    fprintf(pFile, "names:");
    for (int i = 0; i < fileBlocks.size(); i++)
        mpz_out_str(pFile, 10, names[i].z), fprintf(pFile, " ");
    fprintf(pFile,"\n");

    fprintf(pFile, "r:");
    for (int i = 0; i < fileBlocks.size(); i++)
        mpz_out_str(pFile, 10, r[i].z), fprintf(pFile, " ");
    fprintf(pFile,"\n");

    fprintf(pFile, "coeff:");
    for (int i = 0; i < fileBlocks.size(); i++)
        mpz_out_str(pFile, 10, coeff[i].z), fprintf(pFile, " ");
    fprintf(pFile,"\n");
#endif

#ifdef TEST
    startTime = clock();
#endif
    //simulate verify
    int res = rsa_test->verify(index, coeff, R, pi, names, commitment);
#ifdef TEST
    endTime = clock();
    stepTime.push_back((double)(endTime-startTime)/CLOCKS_PER_SEC);
#endif
    std::cout<<"\nVerify result is "<<(bool)res<<std::endl;
#ifdef TEST
    std::cout<<"\nTime analysis:"<<std::endl;
    std::cout<<"File size:"<<file_length<<" bytes"<<std::endl;
    std::cout<<"Key generation time:"<<stepTime[0]<<" s"<<std::endl;
    std::cout<<"Tag generation time:"<<stepTime[1]<<" s"<<std::endl;
    std::cout<<"Proof generation time:"<<stepTime[2]<<" s"<<std::endl;
    std::cout<<"Verify time:"<<stepTime[3]<<" s"<<std::endl;
#endif
//    for(int i = 0; i < fileBlocks.size(); i++)
//        mpz_clear(fileBlocks[i]), mpz_clear(names[i]), mpz_clear(tags[i]), mpz_clear(coeff[i]);
    mpz_clear(R);
    mpz_clear(commitment);
    delete rsa_test;
    return 0;
}