git clone https://github.com/oktal/pistache.git
git submodule update --init
cd pistache
mkdir build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
sudo apt-get install cmake
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
sudo make
sudo make install

