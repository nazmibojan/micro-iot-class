#include <w25q64.hpp>

w25q64::w25q64(/* args */) {}

w25q64::~w25q64() {}

void w25q64::begin() {
  pinMode(CS_PIN, OUTPUT);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  digitalWrite(CS_PIN, HIGH);
}

// convert a page number to a 24-bit address
int w25q64::pageToAddress(int pn) { return (pn << 8); }

// convert a 24-bit address to a page number
int w25q64::addressToPagee(int addr) { return (addr >> 8); }

unsigned char w25q64::readStatus() {
  unsigned char c;

  // This can't do a writePause
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_READ_STATUS_REG);
  c = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);
  return (c);
}

void w25q64::writePause() {
  if (chipWaitForWrite) {
    while (readStatus() & STAT_WIP)
      ;
    chipWaitForWrite = 0;
  }
}

void w25q64::getId(byte *chipId) {
  // Get chip ID
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CHIP_ID);
  for (int i = 0; i < LEN_ID; i++) {
    chipId[i] = SPI.transfer(0);
  }
  digitalWrite(CS_PIN, HIGH);
}

void w25q64::readId(unsigned char *idt) {
  writePause();
  // set control register
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_READ_ID);
  for (uint16_t i = 0; i < 20; i++) {
    *idt++ = SPI.transfer(0x00);
  }
  digitalWrite(CS_PIN, HIGH);
}

void w25q64::hardReset() {
  // Make sure that the device is not busy before
  // doing the hard reset sequence
  // At the moment this does NOT check the
  // SUSpend status bit in Status Register 2
  // but the library does not support suspend
  // mode yet anyway
  writePause();

  // Send Write Enable command
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_RESET_DEVICE);
  digitalWrite(CS_PIN, HIGH);
  delayMicroseconds(50);
  // Wait for the hard reset to finish
  // Don't use chipWaitForWrite here
  while (readStatus() & STAT_WIP)
    ;
  // The spec says "the device will take
  // approximately tRST=30 microseconds
  // to reset"
}

void w25q64::erase(bool wait) {
  writePause();
  // Send Write Enable command
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_WRITE_ENABLE);
  digitalWrite(CS_PIN, HIGH);
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_CHIP_ERASE);
  digitalWrite(CS_PIN, HIGH);
  chipWaitForWrite = 1;
  if (wait)
    writePause();
}

void w25q64::erasePageSector(int pn) {
  int address;

  writePause();
  // Send Write Enable command
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_WRITE_ENABLE);
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_SECTOR_ERASE);
  // Send the 3 byte address
  address = pageToAddress(pn);
  SPI.transfer((address >> 16) & 0xff);
  SPI.transfer((address >> 8) & 0xff);
  SPI.transfer(address & 0xff);
  digitalWrite(CS_PIN, HIGH);
  // Indicate that next I/O must wait for this write to finish
  chipWaitForWrite = 1;
}

void w25q64::pageWrite(unsigned char *wp, int pn) {
  int address;

  writePause();
  // Send Write Enable command
  digitalWrite(CS_PIN, LOW);
  delay(10);
  SPI.transfer(CMD_WRITE_ENABLE);
  delay(10);
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  delay(10);
  SPI.transfer(CMD_PAGE_PROGRAM);
  // Send the 3 byte address
  address = pageToAddress(pn);
  SPI.transfer((uint8_t)(address >> 16) & 0xFF);
  SPI.transfer((uint8_t)(address >> 8) & 0xFF);
  SPI.transfer((uint8_t)address & 0xFF);
  // Now write 256 bytes to the page
  for (uint16_t i = 0; i < 256; i++) {
    SPI.transfer(*wp++);
  }
  delay(10);
  digitalWrite(CS_PIN, HIGH);
  // Indicate that next I/O must wait for this write to finish
  chipWaitForWrite = 1;
}

void w25q64::readPages(unsigned char *p, int pn, const int n_pages) {
  int address;
  unsigned char *rp = p;

  writePause();
  digitalWrite(CS_PIN, LOW);
  delay(10);
  SPI.transfer(CMD_READ_DATA);
  // Send the 3 byte address
  address = pageToAddress(pn);
  SPI.transfer((uint8_t)(address >> 16) & 0xFF);
  SPI.transfer((uint8_t)(address >> 8) & 0xFF);
  SPI.transfer((uint8_t)address & 0xFF);
  // Now read the page's data bytes
  for (uint16_t i = 0; i < n_pages * 256; i++) {
    *rp++ = SPI.transfer(0);
  }
  digitalWrite(CS_PIN, HIGH);
}

void w25q64::fastReadPages(unsigned char *p, int pn, const int n_pages) {
  int address;
  unsigned char *rp = p;

  writePause();
  // The chip doesn't run at the higher clock speed until
  // after the command and address have been sent
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(CMD_READ_HIGH_SPEED);
  // Send the 3 byte address
  address = pageToAddress(pn);
  SPI.transfer((address >> 16) & 0xFF);
  SPI.transfer((address >> 8) & 0xFF);
  SPI.transfer(address & 0xFF);
  // send dummy byte
  SPI.transfer(0);
  // Now read the number of pages required
  for (uint16_t i = 0; i < n_pages * 256; i++) {
    *rp++ = SPI.transfer(0);
  }
  digitalWrite(CS_PIN, HIGH);
}