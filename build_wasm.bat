@echo off
echo Building wasm DEBUG:
pushd build
rem set Exported="['_BactorialUpdateWorld', '_BactorialInitWorld', '_BactorialSelect', '_BactorialDivide', '_BactorialUnselect', '_BactorialSpawnEnemy']"
set Exported="['_main']"
call emcc ^
    ../main.cpp -o main.js ^
    -O0 -std=c++11 -Wno-null-dereference ^
    -DWASM=1 ^
    -s USE_WEBGL2=1 -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s TOTAL_MEMORY=268435456 -s ASSERTIONS=1 -s WASM=1 -s NO_EXIT_RUNTIME=1 ^
    -s EXPORTED_FUNCTIONS=%Exported% -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap', 'getValue']"
popd