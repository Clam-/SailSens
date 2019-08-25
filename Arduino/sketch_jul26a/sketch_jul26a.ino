void setup() {
  // put your setup code here, to run once:
  SerialUSB.begin(115200);
}

unsigned int parity_check(unsigned int v){
  //http://graphics.stanford.edu/~seander/bithacks.html#ParityNaive
  v ^= v >> 16;
  v ^= v >> 8;
  v ^= v >> 4;
  v &= 0xf;
  return (0x6996 >> v) & 1;
}

void loop() {
  // put your main code here, to run repeatedly:
  for (unsigned int x=0;x<10;x++){
    SerialUSB.print(x);
    SerialUSB.print("\t");
    SerialUSB.print(B1000,BIN);
    SerialUSB.print("\t");
    SerialUSB.println(parity_check(x), BIN);
  }
  delay(2000);
}
