#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>
//#resource "Neslib_CRT0_Template (2).zip"

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

#include <peekpoke.h>

#define MMC_MODE 0x00

#define MMC3_SET_REG(r,n)\
	POKE(0x8000, MMC_MODE|(r));\
	POKE(0x8001, (n));

#define MMC3_CHR_0000(n) MMC3_SET_REG(0,n)
#define MMC3_CHR_0800(n) MMC3_SET_REG(1,n)
#define MMC3_CHR_1000(n) MMC3_SET_REG(2,n)
#define MMC3_CHR_1400(n) MMC3_SET_REG(3,n)
#define MMC3_CHR_1800(n) MMC3_SET_REG(4,n)
#define MMC3_CHR_1C00(n) MMC3_SET_REG(5,n)
#define MMC3_PRG_8000(n) MMC3_SET_REG(6,n)
#define MMC3_PRG_A000(n) MMC3_SET_REG(7,n)

#define MMC3_MIRROR(n) POKE(0xa000, (n))

//; WRAM_OFF $40
//; WRAM_ON $80
//; WRAM_READ_ONLY $C0
#define MMC3_WRAM_DISABLE() POKE(0xA001, 0x40)
#define MMC3_WRAM_ENABLE() POKE(0xA001, 0x80)
#define MMC3_WRAM_READ_ONLY() POKE(0xA001, 0xC0)

// bank-switching configuration
#define NES_MAPPER 4		// Mapper 4 (MMC3)
#define NES_PRG_BANKS 32	// # of 16KB PRG banks
#define NES_CHR_BANKS 1	// # of 8KB CHR banks

//#resource "nesbanked.cfg"
#define CFGFILE nesbanked.cfg
//#resource "tileset.bin"
//#resource "neschars.bin"
//#resource "crt0.o"

void drawChars()
{
  byte x, y, z = 0;
  for (y = 0; y < 16; ++y)
  {
    for (x = 0; x < 16; ++x)
    {
      vram_adr(NTADR_A(x + 2,y + 2));
      vram_put(z);
      ++z;
    }
  }
}

void UploadCharset()
{
  int x = 0;
  unsigned char *chrdata = (unsigned char*)0xA000;  
  MMC3_PRG_A000(31); //CPU $A000-$BFFF: 8 KB switchable PRG ROM bank
  vram_adr(0);
  for (x = 0; x < 0x2000; ++x)
  {
    vram_put(chrdata[x]);
  }
}

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x03,			// screen color

  0x11,0x30,0x27,0x0,	// background palette 0
  0x1c,0x20,0x2c,0x0,	// background palette 1
  0x00,0x10,0x20,0x0,	// background palette 2
  0x06,0x16,0x26,0x0,   // background palette 3

  0x16,0x35,0x24,0x0,	// sprite palette 0
  0x00,0x37,0x25,0x0,	// sprite palette 1
  0x0d,0x2d,0x3a,0x0,	// sprite palette 2
  0x0d,0x27,0x2a	// sprite palette 3
};

// setup PPU and tables
void setup_graphics() {
  // clear sprites
  //oam_clear();
  // set palette colors
  pal_all(PALETTE);
  UploadCharset();
  drawChars();
}

byte x;
void main(void)
{
  #include <_heap.h>
  int *heaporg = (int*)&_heaporg;
  int *heapptr = (int*)&_heapptr;
  int *heapend = (int*)&_heapend;
  heaporg[0] = 0x7000; //heaporg
  heapptr[0] = heaporg[0]; //heapptr
  heapend[0] = 0x8000; //heapend
  memset((int*)heaporg[0], 0, heapend[0] - heaporg[0]); 
  
  MMC3_WRAM_ENABLE();
  
  //Program Banks
  MMC3_PRG_8000(0); //CPU $8000-$9FFF (or $C000-$DFFF): 8 KB switchable PRG ROM bank
  MMC3_PRG_A000(0); //CPU $A000-$BFFF: 8 KB switchable PRG ROM bank
  
  //Backgrounds
  MMC3_CHR_0000(0); 	//PPU $0000-$07FF (or $1000-$17FF): 2 KB switchable CHR bank
  MMC3_CHR_0800(2); 	//PPU $0800-$0FFF (or $1800-$1FFF): 2 KB switchable CHR bank
  
  //Sprites
  MMC3_CHR_1000(4); 	//PPU $1000-$13FF (or $0000-$03FF): 1 KB switchable CHR bank
  MMC3_CHR_1400(5); 	//PPU $1400-$17FF (or $0400-$07FF): 1 KB switchable CHR bank
  MMC3_CHR_1800(6); 	//PPU $1800-$1BFF (or $0800-$0BFF): 1 KB switchable CHR bank
  MMC3_CHR_1C00(7);	//PPU $1C00-$1FFF (or $0C00-$0FFF): 1 KB switchable CHR bank
  
  pal_clear();
  pal_bg(PALETTE);
  setup_graphics();
  ppu_on_all();
  while(1) {
    MMC3_CHR_0000(x); 	//PPU $0000-$07FF (or $1000-$17FF): 2 KB switchable CHR bank

    ++x;
  }
}
