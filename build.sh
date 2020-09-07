# Create dist folder
mkdir -p dist

emcc \
  -s WASM=1 \
  -s FETCH=1 \
  -s EXPORTED_FUNCTIONS='["_solver"]' \
  -s EXTRA_EXPORTED_RUNTIME_METHODS='["cwrap"]' \
  webassembly/sudoku.c \
  -o sudoku.js
