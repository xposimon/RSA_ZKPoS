cmake_minimum_required (VERSION 2.6)
project (RSA_ZKPoS)

add_executable(rsa_test main.cpp RSA_ZKPoS.cpp RSA_ZKPoS.h common.cpp)
target_link_libraries(rsa_test gmp)