
fontoj : https://znanev.github.io/Cross-compile-tools-for-Ingenic-T20/
	https://github.com/Dafang-Hacks/Main


# installation des outils de compilation :
## option 1 : avec docker (plus rapide à installer, plus lent à exécuter)
sudo apt install docker.io
sudo usermod -aG docker mia_uzanto
 se déconnecter puis se reconnecter
mkdir ~/dafang
cd ~/dafang
docker run --rm -ti -v $(pwd):/root/ daviey/dafang-cross-compile:latest

# option 2 : en local (plus long à installer, plus rapide à exécuter)
sudo apt install build-essential git gcc-mips-linux-gnu autoconf libtool cmake ftp-upload u-boot-tools
mkdir -p ~/dafang/Main/mips-gcc472-glibc216-64bit
ln -s /usr/bin ~/dafang/Main/mips-gcc472-glibc216-64bit



git clone --recurse-submodules https://github.com/Dafang-Hacks/Main.git
cd Main
./compile_libraries.sh
cd v4l2rtpsserver-master
./compile.sh

