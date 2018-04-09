#More installation options can be found here : https://github.com/log4cplus/log4cplus/blob/master/INSTALL
git clone https://github.com/log4cplus/log4cplus.git
cd log4cplus/
git submodule update --init --recursive
./configure
sudo make
sudo make install
