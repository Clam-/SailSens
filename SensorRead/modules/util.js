export function parity_check(v){
  //http://graphics.stanford.edu/~seander/bithacks.html#ParityNaive
  v ^= v >>> 16;
  v ^= v >>> 8;
  v ^= v >>> 4;
  v &= 0xf;
  return (0x6996 >>> v) & 1;
}
