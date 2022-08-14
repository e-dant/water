//bittest
#include <bitset>
#include <iostream>
#include <vector>
#include <numeric>

inline unsigned int get_a(unsigned int start, unsigned int nbits, unsigned int bits) noexcept {
  return (bits >> (start + 1 - nbits)) & ~(~0 << nbits);
}
inline unsigned int get_b(int start, int nbits, unsigned int bits) noexcept {
  return (bits >> (start + 1 - nbits)) & ~(~0 << nbits);
}

int main(){
  std::cout << "space8:\n";

  std::bitset<8> baseline_8ones("11111111");
  std::cout << " bitset baseline of 11111111b:\n " << baseline_8ones << "\n";
  std::bitset<8> baseline_64(64);
  std::cout << " bitset baseline of 64d:\n " << baseline_64 << "\n";
  std::bitset<8> bitsof_get_a8864(get_a(8, 8, 64));
  std::cout << " bitset of get_a(8, 8, 64)\n " << bitsof_get_a8864 << "\n";
  std::bitset<8> bitsof_get_a0864(get_a(0, 8, 64));
  std::cout << " bitset of get_a(0, 8, 64)\n " << bitsof_get_a0864 << "\n";
  std::bitset<8> bitsof_get_a08128(get_a(0, 8, 128));
  std::cout << " bitset of get_a(0, 8, 128)\n " << bitsof_get_a08128 << "\n";

  std::cout << "space6:\n";

  std::bitset<8> baseline_6ones("111111");
  std::cout << " bitset baseline of 111111b:\n " << baseline_6ones << "\n";
  std::bitset<8> baseline_63(63);
  std::cout << " bitset baseline of 32d:\n " << baseline_63 << "\n";
  std::bitset<8> bitsof_get_a0863(get_a(0, 8, 63));
  std::cout << " bitset of get_a(0, 4, 63)\n " << bitsof_get_a0863 << "\n";
  std::bitset<8> bitsof_get_a0463(get_a(0, 4, 63));
  std::cout << " bitset of get_a(8, 8, 63)\n " << bitsof_get_a0463 << "\n";
  std::bitset<8> bitsof_get_a4863(get_a(4, 8, 63));
  std::cout << " bitset of get_a(4, 8, 63)\n " << bitsof_get_a4863 << "\n";

  std::vector<unsigned> i (16);
  std::iota(i.begin(), i.end(), 0);

  std::vector<unsigned> j (16);
  std::iota(j.begin(), j.end(), 0);

  std::vector<unsigned> k (15);
  std::iota(k.begin(), k.end(), 1);

  for (const unsigned& it : i) {
    for (const unsigned& jt : j) {
      for (const unsigned& kt : k) {
        std::bitset<8> bitsofitjtkt(get_a(it, jt, kt));
        std::bitset<8> bitsofkt(kt);
        std::cout << "========\n"
          << "|> it/jt/kt:\n=>  " << it << "/" << jt << "/" << kt << "\n";
        std::cout << "|> std::bitset bitsof(get_a(it, jt, kt)):\n=>  " << bitsofitjtkt << "\n";
        std::cout << "|> std::bitset bitsof(kt):\n=>  " << bitsofkt << "\n";
        std::cout << "|> get_a(it, jt, kt):\n=>  " <<  get_a(it, jt, kt) << "\n";
      }
    }
  }
  return 0;
}
