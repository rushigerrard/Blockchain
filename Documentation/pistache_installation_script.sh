git clone https://github.com/oktal/pistache.git
cd pistache
git submodule update --init
mkdir build
cd build
sudo apt-get install cmake
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
sudo make
sudo make install

