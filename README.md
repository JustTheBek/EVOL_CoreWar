# EVOL_CoreWar
My solution of the Core War assignment of the Evolutionary and Logic-based AI class at FHTW in the first semester.



X11 is needed:

```bash
sudo apt update
sudo apt install libx11-dev
```

This project evolves Core War warriors using a Genetic Algorithm (GALib)
and evaluates them using pMARS 0.9.4.

Fitness is computed using:
- Tournament score difference
- Win/loss statistics
- Optional variance penalty across opponents

The system does not rely on unavailable pMARS runtime metrics.


## Dependencies :

### pMars Setup on macOS

This chapter summarizes the steps taken to compile and run **pMars** on macOS, including modifications and build instructions.  
  
**Recomendation:** take the source files and the build script checked-in into this project, since it already has been adapted to the needs of the project. Don't try to setup it from scratch, it's too much effort.

### GALib Setup on macOS

This chapter summarizes the steps taken to compile and run **GALib (Genetic Algorithm Library)** on macOS, including modifications and build instructions.  
  
**Recomendation:** take the source files and the build script checked-in into this project, since it already has been adapted to the needs of the project. Don't try to setup it from scratch, it's too much effort.


#### I. Source Download

The GALib source code was downloaded from the official GALib website:


- **Website:** [http://lancet.mit.edu/ga/dist/](http://lancet.mit.edu/ga/dist/)
- **Version:** 2.4.7(.tgz)
- **Source files included:**  
  - Core library files in `ga/` 
  - Examples in `examples/` 
  - Documentation in `doc/` 



#### II. Source Code Modifications

Several modifications were necessary for successful compilation on modern macOS systems:

1. **Version file issue:**  
   - `ga/VERSION` contained `2.4.7` as the first line, which caused a compile error, becuase the compiler wanted to build this file as well.
   - I removed it via renaming it to VERSION.txt

2. **String literal warnings (C++11+):**  
   - `gaerror.c` had multiple string literals assigned to `char*`.  
   - Warnings such as:
     ```
     ISO C++11 does not allow conversion from string literal to 'char *'
     ```
   - These are harmless but can be fixed later by using `const char*` instead of `char*`.

3. **Deprecated functions:**  
   - Warnings for `sprintf()` and `register` keyword occurred in multiple files:  
     - `gabincvt.c`, `GA1DBinStrGenome.c`, `GA2DBinStrGenome.c`, `GA3DBinStrGenome.c`  
   - These warnings are non-fatal and can be ignored or fixed by switching to `snprintf()` and removing `register`.

4. **C++17 compatibility:**  
   - To fix undeclared identifier errors for `initializer` and `mutator` in:  
     - `GA1DArrayGenome.c`  
     - `GA2DArrayGenome.c`  
     - `GA3DArrayGenome.c`  
    - The following fixes have been applied:  

        4.1 GA1DArrayGenome.c:  
        Replaced calls to `initializer(...)` and `mutator(...)` with:
        ```cpp
        this->initializer(GA1DArrayAlleleGenome<T>::DEFAULT_1DARRAY_ALLELE_INITIALIZER);
        this->mutator(GA1DArrayAlleleGenome<T>::DEFAULT_1DARRAY_ALLELE_MUTATOR);
        ```
        This change was applied at lines 225, 226, 241, and 242.

        4.2 GA2DArrayGenome.c:  
        Replaced calls to `initializer(...)` and `mutator(...)` with:
        ```cpp
        this->initializer(GA2DArrayAlleleGenome<T>::DEFAULT_2DARRAY_ALLELE_INITIALIZER);
        this->mutator(GA2DArrayAlleleGenome<T>::DEFAULT_2DARRAY_ALLELE_MUTATOR);
        ```
        This change was applied at lines 272, 273, 289, and 290.

        4.3 GA3DArrayGenome.c:  
        Replaced calls to `initializer(...)` and `mutator(...)` with:
        ```cpp
        this->initializer(GA3DArrayAlleleGenome<T>::DEFAULT_3DARRAY_ALLELE_INITIALIZER);
        this->mutator(GA3DArrayAlleleGenome<T>::DEFAULT_3DARRAY_ALLELE_MUTATOR);
        ```
        This change was applied at lines 325, 326, 342, and 343.

    - The library was built using **C++98** standard:  
     ```bash
     g++ -std=c++98 ...
     ```

5. **Unused variables warnings:**  
   - `GA3DArrayGenome.c` contained unused variables `maxx`, `maxy`, `maxz`.  
   - Harmless, can be silenced with `(void)maxx;` etc.


#### III. Build Process

1. **Compile the library files**  
   - From the `ga/` directory, compile all `.c` files with:
     ```bash
     make CXX="g++ -std=c++98"
     ```
   - This produces `.o` object files and builds `libga.a`.

2. **Verify compilation**  
   - Compilation may produce warnings about deprecated `register` and `sprintf()`.  
   - All `.o` files are archived into `libga.a` using `ar rv libga.a ...`.

3. **Prepare the tester shell script**  

   - A shell script `build_galib_tester.sh` was created to compile the test program:
     ```bash
     #!/bin/bash
     echo "Compiling galib_tester.cpp..."
     g++ -I./ga -L. galib_tester.cpp -lga -o galib_tester
     ```
   - `-I./ga` tells the compiler where to find GALib headers.  
   - `-L.` tells the linker to look in the current directory for `libga.a`.  
   - `-lga` links against the compiled GALib library.

4. **Run the tester**  
   - Make the script executable and run it:
     ```bash
     chmod +x build_galib_tester.sh
     ./build_galib_tester.sh
     ./galib_tester
     ```
   - Output:
     ```
     GALib is working!
     ```


#### IV. Notes

- Warnings during compilation are mostly due to legacy code using deprecated C++ constructs and string literals.  
- Using `C++98` ensures `initializer` and `mutator` are correctly recognized.  
- Once compiled, `libga.a` can be linked into any C++ project using GALib.  


#### V. Directory Structure

After building, the key directories and files are:

galib247/
├── ga/ # Core library files
│ ├── libga.a # Compiled static library
│ ├── *.c, *.h # Source and headers
│ └── ...
├── galib_tester.cpp # Test program
├── build_galib_tester.sh # Shell script to build tester
└── doc/ # Documentation

#### VI. Summary

1. Downloaded GALib 2.4.7 from [MIT GALib site](http://lancet.mit.edu/ga/).  
2. Built library using `g++ -std=c++98` to maintain compatibility.  
3. Resolved header and `initializer`/`mutator` issues.  
4. Created `build_galib_tester.sh` to compile a test program.  
5. Verified installation using `./galib_tester` → output confirms library is working.  

GALib is now fully functional and ready for use in C++ genetic algorithm projects on macOS.