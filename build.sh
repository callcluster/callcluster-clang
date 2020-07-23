
cd test-projects/simple
bash build.sh
cd ../..

mkdir build
cd build
cmake --debug-find .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON 
cmake --build .
cd ..
chmod a+x ./build/callclusterClang
#./build/callclusterClang ./test-projects/simple/build