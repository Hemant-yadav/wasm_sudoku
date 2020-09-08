# Create dist folder
mkdir -p dist

emcc \
  -O3 \
  -s WASM=1 \
  -s FETCH=1 \
  -s EXPORTED_FUNCTIONS='["_solver", "_malloc", "_free"]' \
  -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' \
  webassembly/sudoku.c \
  -o out/sudoku.js
