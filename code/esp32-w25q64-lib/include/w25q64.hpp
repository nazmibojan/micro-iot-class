#include <Arduino.h>
#include <SPI.h>

#define CS_PIN 5
#define SCK_PIN 18
#define MISO_PIN 19
#define MOSI_PIN 23

#define CHIP_ID 0x9F

#define LEN_ID 4

#define STAT_WIP 1
#define STAT_WEL 2

#define CMD_WRITE_STATUS_REG 0x01
#define CMD_PAGE_PROGRAM 0x02
#define CMD_READ_DATA 0x03
#define CMD_WRITE_DISABLE 0x04 // not tested
#define CMD_READ_STATUS_REG 0x05
#define CMD_WRITE_ENABLE 0x06
#define CMD_READ_HIGH_SPEED 0x0B // not tested
#define CMD_SECTOR_ERASE 0x20 // not tested
#define CMD_BLOCK32K_ERASE 0x52 // not tested
#define CMD_RESET_DEVICE 0xF0 //<<-different from winbond
#define CMD_READ_ID 0x9F
#define CMD_RELEASE_POWER_DOWN 0xAB // not tested
#define CMD_POWER_DOWN 0xB9 // not tested
#define CMD_CHIP_ERASE 0xC7
#define CMD_BLOCK64K_ERASE 0xD8 // not tested

class w25q64
{
private:
  int chipWaitForWrite = 0;

  int pageToAddress(int pn);
  int addressToPagee(int addr);
  unsigned char readStatus();
  void writePause();
  
public:
  w25q64();
  ~w25q64();
  void begin();
  void getId(byte * chipId);
  void readId(unsigned char *idt);
  void hardReset();
  void erase(bool wait);
  void erasePageSector(int pn);
  void pageWrite(unsigned char *wp, int pn);
  void readPages(unsigned char *p, int pn, const int n_pages);
  void fastReadPages(unsigned char *p, int pn, const int n_pages);
};
