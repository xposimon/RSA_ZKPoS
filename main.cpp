#include "RSA_ZKPoS.h"
#define SECURITY_BITS 10

int main()
{
    std::cout<<"RSA_ZKPoS test codes"<<std::endl;
    RSA_ZKPoS* rsa_test = new RSA_ZKPoS();
    rsa_test->keyGen(SECURITY_BITS/2);
    rsa_test->exportKeys();
    std::vector<unsigned char> file_bytes;
    rsa_test->tagGen(file_bytes);
    return 0;
}