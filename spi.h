#ifndef EMBEDDED_SPI_HPP_
#define EMBEDDED_SPI_HPP_

struct spi_context;

int spi_read(spi_context*);
int spi_write(spi_context*, int);
spi_context* spi_open(const char*, uint8_t, uint8_t, uint32_t, uint16_t);
void spi_close(spi_context*);

#endif
