echo "Configuring and building Thirdparty/DBoW2 ..."

cd Thirdparty/DBoW2
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j

cd ../../g2o

echo "Configuring and building Thirdparty/g2o ..."

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j

cd ../../../

echo "Uncompress vocabulary ..."

cd Vocabulary
tar -xf ORBvoc.txt.tar.gz
cd ..

apt-get install build-essential autoconf libtool pkg-config
apt-get install cmake
git clone -b v1.28.1 https://github.com/grpc/grpc
cd grpc
git submodule update --init

cd third_party/protobuf/
git submodule update --init --recursive #确保克隆子模块，更新第三方源码
./autogen.sh   #生成配置脚本
./configure    #生成Makefile文件，为下一步的编译做准备，可以加上安装路径：--prefix=path ，默认路径为/usr/local/
make j4        #从Makefile读取指令，然后编译
make install
sudo ldconfig       #更新共享库缓存
which protoc        #查看软件的安装位置
protoc --version

cd ../..
mkdir build
cd build
cmake -DgRPC_INSTALL=ON -DgRPC_ZLIB_PROVIDER=package \
-DgRPC_ABSL_PROVIDER=package -DgRPC_CARES_PROVIDER=package \
-DgRPC_PROTOBUF_PROVIDER=package -DgRPC_SSL_PROVIDER=package ..
make -j4
make install

echo "Configuring and building ORB_SLAM2 ..."

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
