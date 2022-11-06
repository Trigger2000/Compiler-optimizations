# Compiler-optimizations v3

Major changes:  
    - Added slow algorithm for building dominator's tree with tests
    - Added fast algorithm for building dominator's tree with tests
    - Introduced PassManager class and all passes are implemented as classes 

# Build and launch test
```
cmake <SRC_DIR_PATH>
make
<BUILD_DIR_PATH>/compiler_opts

<BUILD_DIR_PATH>/tests/tests - for tests
```