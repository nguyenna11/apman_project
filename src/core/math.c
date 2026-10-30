#include <core/math.h>

#include <assert.h>

size_t factorial(size_t n) {
  assert(n<21 && "Factorial overflow!");

  return !n? 1 : n*factorial(n-1);
}

size_t Pkn(size_t K, size_t N) { return factorial(N) / factorial(N-K); }
size_t Ckn(size_t K, size_t N) { return Pkn(K, N) / factorial(K); }

/* Safe modular multiplication: (a * b) % m without overflow */
static inline uint64_t mul_mod(uint64_t a, uint64_t b, uint64_t m) {
  return (uint64_t)(((unsigned __int128)a * b) % m);
}

/* Modular exponentiation: (base^exp) % mod */
static uint64_t power_mod(uint64_t base, uint64_t exp, uint64_t mod) {
  uint64_t res = 1;
  base %= mod;
  while (exp > 0) {
    if (exp & 1) res = mul_mod(res, base, mod);
    base = mul_mod(base, base, mod);
    exp >>= 1;
  }
  return res;
}

int   is_prime(uint64_t n) {
  // Deterministic bases sufficient for all 64-bit integers
  static const uint64_t bases[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
  size_t            num_bases = sizeof(bases) / sizeof(bases[0]);
  uint64_t                  d = n - 1;
  int                       s = 0;

  if (n < 2) return 0;
  if (n == 2 || n == 3 || n == 5 || n == 7) return 1;
  if (n % 2 == 0 || n % 3 == 0 || n % 5 == 0 || n % 7 == 0) return 0;

  /* Write n - 1 as d * 2^s */
  d = n - 1;
  s = 0;
  while ((d & 1) == 0) {
    d >>= 1;
    s++;
  }

  for (size_t i = 0 ; i < num_bases ; i++) {
    if (n <= bases[i]) break;

    uint64_t x = power_mod(bases[i], d, n);
    if (x == 1 || x == n - 1) continue;

    int composite = 1;
    for (int r = 1 ; r < s ; r++) {
      x = mul_mod(x, x, n);
      if (x == n - 1) {
        composite = 0;
        break;
      }
    }

    if (composite) return 0;
  }

  return 1;
}
