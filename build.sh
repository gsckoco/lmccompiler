if g++-10 -std=c++14 -g -pipe `sourcefiles ./src c cpp` -o./out/lmccompiler; then
    ./out/lmccompiler $@
else
    echo "Unable to run, build failed"
fi