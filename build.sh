cd test-projects/simple
bash build.sh
cd ../..

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 
#cmake --build .
make
cd ..
chmod a+x ./build/callclusterClang
./build/callclusterClang ./test-projects/simple/build --functions