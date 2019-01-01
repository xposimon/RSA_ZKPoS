## Introduction
RSA based PoS pro

### Depends:
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

### Api