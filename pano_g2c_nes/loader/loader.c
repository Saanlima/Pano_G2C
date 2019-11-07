#include <stdio.h>
#include <windows.h>

int main ( int argc, char *argv[] ) {
  unsigned char prgRomBanks, chrRomBanks; 
  int prgRomDataSize, chrRomDataSize;
  int i;
  unsigned char fbuff[65536];
  FILE *fp, *fp2;

  if ((fp = fopen(argv[1], "rb"))) {

    if (fread(fbuff,1,16,fp) != 16) {
      printf("read error!\n");
      return 2;
    }
    if ((fbuff[0] != 'N') || (fbuff[1] != 'E') || (fbuff[2] != 'S') || (fbuff[3] != 0x1A)) {
      printf("Invalid ROM header!\n");
      return 1;
    }
    prgRomBanks = fbuff[4];
    chrRomBanks = fbuff[5];

    if ((prgRomBanks > 2) || (chrRomBanks > 1)) {
      printf("Too many ROM banks!\n");
      return 1;
    }
    
    if (fbuff[6] & 0x08) {
      printf("Only horizontal and vertical mirroring are supported!\n");
      return 1;
    }
    
    if ((((fbuff[6] & 0xF0) >> 4)| (fbuff[7] & 0xF0)) != 0) {
      printf("Only mapper 0 is supported!\n");
      return 1;
    }

    prgRomDataSize = prgRomBanks * 0x4000;
    if (fread(fbuff,1,prgRomDataSize,fp) != prgRomDataSize) {
      printf("prgrom read error!\n");
      return 2;
    }

    HANDLE serialHandle;
    serialHandle = CreateFile("\\\\.\\COM5", GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);


    fp2 = fopen("prgrom.mem", "w");
    for (i = 0; i < prgRomDataSize; i++) {
      if (i % 16 == 0)
        fprintf(fp2, "@%x ", i);
      fprintf(fp2, "%02x ", fbuff[i]);
      if (i % 16 == 15)
        fprintf(fp2, "\n");
    }
    fclose(fp2);

    chrRomDataSize = chrRomBanks * 0x2000;
    if (fread(fbuff,1,chrRomDataSize,fp) != chrRomDataSize) {
      printf("chrrom read error!\n");
      return 2;
    }
    
    fp2 = fopen("chrrom.mem", "w");
    for (i = 0; i < chrRomDataSize; i++) {
      if (i % 16 == 0)
        fprintf(fp2, "@%x ", i);
      fprintf(fp2, "%02x ", fbuff[i]);
      if (i % 16 == 15)
        fprintf(fp2, "\n");
    }
    fclose(fp2);

    fclose(fp);
    printf("Done\n");
  }
}
