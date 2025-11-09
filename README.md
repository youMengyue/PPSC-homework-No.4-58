# SIMD Performance Demonstration

## Student Number: 58

---

### Target Instruction: `_mm_div_pd`

**Calculation:**
- Prefix: 58 % 2 = 0  → `_mm`
- Suffix: 58 % 4 = 2  → `_pd`
- Core: 58 / 8 = 7    → `_div`
- **Result: `_mm_div_pd`**

---

### What the Program Does

This program demonstrates the performance advantage of the SIMD instruction `_mm_div_pd` over regular scalar computations.

**How it works:**
- `_mm_div_pd` processes **2 double values simultaneously** (128 bits)
- To demonstrate real speedup, an intensive computation method is used:
  - 10 iterations of array processing
  - 3 division operations in each iteration
  - Averaging across 3 measurements

**Expected result:** Speedup of **1.5-2.0x** compared to the scalar version.

---

### How to Run

#### Option 1: Using Makefile (recommended)

```bash
# Run with disabled auto-vectorization (shows real difference)
make run_no_vec

# Clean up
make clean
```

#### Option 2: Manual compilation

```bash
# Compile
g++ -std=c++17 -O3 -msse2 -fno-tree-vectorize main.cpp -o homework_58

# Run
./homework_58
```

---

### Project Structure

```
.
├── main.cpp        # Main program code
├── Makefile        # Automated build
└── README.md       # Documentation
```

---

### Requirements

- Compiler: GCC 7.0+ or Clang 6.0+
- Processor with SSE2 support (all modern x86-64 processors)
