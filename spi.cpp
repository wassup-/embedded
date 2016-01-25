#include "spi.h"

#include <stdint.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_RX_SIZE 4
#define SPI_TX_SIZE 4

struct spi_context
{
  unsigned int fd;
  spi_ioc_transfer transport;
};

int spi_read(spi_context* spi)
{
  uint8_t rx[SPI_RX_SIZE];

  struct spi_ioc_transfer tr;
  tr.tx_buf = 0;
  tr.rx_buf = (unsigned long) rx;
  tr.len = SPI_RX_SIZE;
  tr.delay_usecs = spi->transport.delay_usecs;
  tr.speed_hz = spi->transport.speed_hz;
  tr.bits_per_word = spi->transport.bits_per_word;

  if(ioctl(spi->fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
    perror("can't receive spi message");
    return -1;
  }

  return rx[0];
}

int spi_write(spi_context* spi, int val)
{
  uint8_t tx[SPI_TX_SIZE];

  tx[0] = ~val;

  struct spi_ioc_transfer tr;
  tr.tx_buf = (unsigned long) tx;
  tr.rx_buf = 0;
  tr.len = SPI_TX_SIZE;
  tr.delay_usecs = spi->transport.delay_usecs;
  tr.speed_hz = spi->transport.speed_hz;
  tr.bits_per_word = spi->transport.bits_per_word;

  if(ioctl(spi->fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
    perror("can't send spi message");
    return -1;
  }

  return 1;
}

spi_context* spi_open(const char* device,
                      uint8_t mode,
                      uint8_t bits,
                      uint32_t speed,
                      uint16_t delay)
{
  const int fd = open(device, O_RDWR);
  if (fd < 0) {
    pabort("can't open SPI device");
  }

  // spi mode
  if (-1 == ioctl(fd, SPI_IOC_WR_MODE, &mode)) {
    pabort("can't set spi mode");
  }

  // bits per word
  if (-1 == ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits)) {
    pabort("can't set bits per word");
  }

  // max speed hz
  if (-1 == ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed)) {
    pabort("can't set max speed hz");
  }

  spi_context* ctx = new spi_context();
  ctx->fd = fd;
  ctx->transport.tx_buf = 0;
  ctx->transport.rx_buf = 0;
  ctx->transport.len = 0;
  ctx->transport.delay_usecs = delay;
  ctx->transport.speed_hz = speed;
  ctx->transport.bits_per_word = bits;
  return ctx;
}

void spi_close(spi_context* spi)
{
  if(close(spi->fd) < 0) {
    pabort("can't close SPI device");
  }
  delete spi;
}
