## Introduction
It's a demo of RSA-HLA off-chain prototype. The codes will finish the tasks with keyGen, tagGen, commit, challenge, prove, and verify.

### Required library
gmp

### Ubuntu Install gmp
##### 1. Install depend: m4
```
$sudo apt-get install m4
```

##### 2.Download gmp package
```
$lzip d gmp-6.1.2.tar.lz
$tar -xvf gmp-6.1.2.tar
```
##### 3. Compile and install
```
$cd gmp-6.1.2
$./configure --enable-cxx
$make
$make check
$sudo make install
```

### Usage
```
$git clone [URL]
$cd RSA_ZKPoS
$mkdir build
$cmake ..
$make
./rsa_test
```

### Remaining tasks
1. Random oracle H isn't implemented with the current implementation as H(z){ return g1^z mod N }
2. If the size of a file isn't the multiple of BLOCKSIZE, there is a need of padding, which is not implemented.
Therefore, the size of input files should be of multiple of BLOCKSIZE.