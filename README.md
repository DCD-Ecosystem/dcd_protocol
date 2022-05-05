
# DCD Protocol. Divide trust. Unite players
Welcome to the DCD Ecosystem source code repository! This software is actively developed by the Graphene Lab team based on previous internal developments, as well as using a number of public technologies, including EOSIO, Ethereum and others.


## Software Installation From Scratch Ubuntu 18.04
### Install the pre-requisite packages
```
apt-get update &&  apt-get upgrade -y && apt-get install -y git make  bzip2 automake libbz2-dev libssl-dev doxygen graphviz libgmp3-dev autotools-dev libicu-dev python2.7 python2.7-dev python3 python3-dev python-configparser python-requests python-pip  autoconf libtool g++ gcc curl zlib1g-dev sudo ruby libusb-1.0-0-dev    libcurl4-gnutls-dev pkg-config patch vim-common jq net-tools
```

### Install cmake
```
curl -LO https://cmake.org/files/v3.16/cmake-3.16.2.tar.gz &&  tar -xzf cmake-3.16.2.tar.gz &&  cd cmake-3.16.2 &&  ./bootstrap --prefix=/usr/local &&  make -j$(nproc) &&  make install 
```

### Build clang and modules
```
git clone --single-branch --branch llvmorg-10.0.0 https://github.com/llvm/llvm-project clang10 &&  mkdir -p clang10/build && cd clang10/build &&  cmake -G 'Unix Makefiles' -DCMAKE_INSTALL_PREFIX='/usr/local' -DLLVM_ENABLE_PROJECTS='lld;polly;clang;clang-tools-extra;libcxx;libcxxabi;libunwind;compiler-rt' -DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_ENABLE_RTTI=ON -DLLVM_INCLUDE_DOCS=OFF -DLLVM_TARGETS_TO_BUILD=host -DCMAKE_BUILD_TYPE=Release ../llvm && make -j $(nproc) &&  make install
```

### Add the required config file
```
cp docker/clang.make /tmp/clang.cmake
```

###  Build and install llvm using required config
```
git clone --depth 1 --single-branch --branch llvmorg-10.0.0 https://github.com/llvm/llvm-project llvm && cd llvm/llvm && mkdir build && cd build &&  cmake -G 'Unix Makefiles' -DLLVM_TARGETS_TO_BUILD=host -DLLVM_BUILD_TOOLS=false -DLLVM_ENABLE_RTTI=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_TOOLCHAIN_FILE=/tmp/clang.cmake -DCMAKE_EXE_LINKER_FLAGS=-pthread -DCMAKE_SHARED_LINKER_FLAGS=-pthread -DLLVM_ENABLE_PIC=NO -DLLVM_ENABLE_TERMINFO=OFF .. &&     make -j$(nproc) &&      make install
```

### Install boost
```
curl -LO https://boostorg.jfrog.io/artifactory/main/release/1.72.0/source/boost_1_72_0.tar.bz2 &&  tar -xjf boost_1_72_0.tar.bz2 &&  cd boost_1_72_0 &&  ./bootstrap.sh --with-toolset=clang --prefix=/usr/local && ./b2 toolset=clang cxxflags='-stdlib=libc++ -D__STRICT_ANSI__ -nostdinc++ -I/usr/local/include/c++/v1 -D_FORTIFY_SOURCE=2 -fstack-protector-strong -fpie' linkflags='-stdlib=libc++ -pie' link=static threading=multi --with-iostreams --with-date_time --with-filesystem --with-system --with-program_options --with-chrono --with-test -q -j$(nproc) install 
```

### Install postgresql
```
export TZ=Europe/Amsterdam
ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone &&  echo "deb http://apt.postgresql.org/pub/repos/apt bionic-pgdg main" > /etc/apt/sources.list.d/pgdg.list &&  curl -sL https://www.postgresql.org/media/keys/ACCC4CF8.asc | apt-key add - && apt-get update && apt-get -y install libpq-dev postgresql-13 &&    apt-get clean

```
### Install libpqxx
curl -L https://github.com/jtv/libpqxx/archive/7.2.1.tar.gz | tar zxvf - && cd  libpqxx-7.2.1  &&  cmake -DCMAKE_TOOLCHAIN_FILE=/tmp/clang.cmake -DSKIP_BUILD_TEST=ON -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql -DCMAKE_BUILD_TYPE=Release -S . -B build &&  cmake --build build && cmake --install build

### Install nvm

```
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.35.0/install.sh | bash
cp ~/.bashrc ~/.bashrc.bak && \ cat ~/.bashrc.bak | tail -3 > ~/.bashrc && \  cat ~/.bashrc.bak | head -n '-3' >> ~/.bashrc && \  rm ~/.bashrc.bak 
bash -c '. ~/.bashrc; nvm install --lts=dubnium' && ln -s "/root/.nvm/versions/node/$(ls -p /root/.nvm/versions/node | sort -Vr | head -1)bin/node" /usr/local/bin/node 
```

### Install nodejs
```
curl -sL https://deb.nodesource.com/setup_13.x | sudo -E bash -
apt-get update && apt-get install -y nodejs 
```

### Build dcd_protocol 
```
git clone https://github.com/DCD-Ecosystem/dcd_protocol.git && cd dcd_protocol && mkdir -p build && cd build && cmake -G 'Unix Makefiles' -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/dcd_protocol -DCMAKE_TOOLCHAIN_FILE='/tmp/clang.cmake' -DCMAKE_EXE_LINKER_FLAGS=-pthread -DCMAKE_SHARED_LINKER_FLAGS=-pthread .. && make -j10 && make install
```


## Software Installation Using Docker
Proceed to the provided docker folder and follow the instructions in the README included

## License

DCD is released under the open source [MIT](./LICENSE) license and is offered “AS IS” without warranty of any kind, express or implied. Any security provided by the DCD software depends in part on how it is used, configured, and deployed. DCD is built upon many third-party libraries such as WABT (Apache License) and WAVM (BSD 3-clause) which are also provided “AS IS” without warranty of any kind. Without limiting the generality of the foregoing, Block.one makes no representation or guarantee that DCD or any third-party libraries will perform as intended or will be free of errors, bugs or faulty code. Both may fail in large or small ways that could completely or partially limit functionality or compromise computer systems. If you use or implement DCD, you do so at your own risk. In no event will Block.one be liable to any party for any damages whatsoever, even if it had been advised of the possibility of damage.  

# DCD NODES SPECIFICATIONS 

### PRODUCTION MAINNET NODES
* Processor: Intel® Core™ i7-8700 Hexa-Core (incl. Hyper-Threading Technology)
* RAM: 64 GB DDR4 RAM
* Storage: 2 x 1 TB NVMe SSD
* Connection: 1 GBit/s port

### BACKUP/TESTNET PRODUCTION NODES
* Processor: Intel® Xeon® E3-1270 v6 Octa-Core (Hyper-Threading Technology)
* RAM: 32 GB DDR4 RAM
* Storage: 500 GB SSD
* Connection: 1 GBit/s port

### API/HTTPS NODES
* Processor: Intel® Core™ i7-8700 Hexa-Core (incl. Hyper-Threading Technology)
* RAM: 64 GB DDR4 RAM
* Storage: 2 x 1 TB NVMe SSD
* Connection: 1 GBit/s port

### BACKUP API/HTTPS NODES
* Processor: Intel® Xeon® E3-1270 v6 Octa-Core (Hyper-Threading Technology)
* RAM: 32 GB DDR4 RAM
* Storage: 500 GB SSD
* Connection: 1 GBit/s port

### SEED NODES
* Processor: Intel® Xeon® E3-1270 v6 Octa-Core (Hyper-Threading Technology)
* RAM: 32 GB DDR4 RAM
* Storage: 500 GB SSD
* Connection: 1 GBit/s port
