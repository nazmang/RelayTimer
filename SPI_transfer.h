#include <Arduino.h>

template <typename T> unsigned int SPI_write(const T& value)
{
	const byte * p = (const byte*)&value;
	unsigned int i;
	for (i = 0; i < sizeof value; i++)
		SPI.transfer(*p++);
	return i;
}  

template <typename T> unsigned int SPI_read(T& value)
{
	byte * p = (byte*)&value;
	unsigned int i;
	for (i = 0; i < sizeof value; i++)
		*p++ = SPI.transfer(0);
	return i;
}  

/*template <typename T> unsigned int SPI_read_ISR(T& value)
{
	byte * p = (byte*)&value;
	unsigned int i;
	*p++ = SPDR;  // get first byte
	for (i = 1; i < sizeof value; i++)
		*p++ = SPI.transfer(0);
	return i;
}  */