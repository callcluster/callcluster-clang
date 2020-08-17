cd test-projects/simple
bash build.sh
cd ../..

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON  -DCMAKE_C_FLAGS=-pg -DCMAKE_EXE_LINKER_FLAGS=-pg -DCMAKE_SHARED_LINKER_FLAGS=-pg ..
cmake --build .
#cmake --build .
#make CFLAGS="-pg" LDFLAGS="-pg" LDLIBS="-pg"
cd ..
chmod a+x ./build/callclusterClang
./build/callclusterClang ./test-projects/simple/build --functions
#./build/callclusterClang ../php-src/ --progress
#./build/callclusterClang ../redis/ --progress
python3 test.py