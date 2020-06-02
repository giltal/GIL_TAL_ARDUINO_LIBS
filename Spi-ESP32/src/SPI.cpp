/* 
 SPI.cpp - SPI library for esp8266

 Copyright (c) 2015 Hristo Gochkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "SPI.h"

SPIClass::SPIClass(uint8_t spi_bus)
    :_spi_num(spi_bus)
    ,_spi(NULL)
    ,_use_hw_ss(false)
    ,_sck(-1)
    ,_miso(-1)
    ,_mosi(-1)
    ,_ss(-1)
    ,_div(0)
    ,_freq(1000000)
    , _inTransaction(false)
{}

void SPIClass::begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
    if(_spi) {
        return;
    }

    if(!_div) {
        _div = spiFrequencyToClockDiv(_freq);
    }

    _spi = spiStartBus(_spi_num, _div, SPI_MODE0, SPI_MSBFIRST);
    if(!_spi) {
        return;
    }

    if(sck == -1 && miso == -1 && mosi == -1 && ss == -1) {
        _sck = (_spi_num == VSPI) ? SCK : 14;
        _miso = (_spi_num == VSPI) ? MISO : 12;
        _mosi = (_spi_num == VSPI) ? MOSI : 13;
        _ss = (_spi_num == VSPI) ? SS : 15;
    } else {
        _sck = sck;
        _miso = miso;
        _mosi = mosi;
        _ss = ss;
    }

    spiAttachSCK(_spi, _sck);
    spiAttachMISO(_spi, _miso);
    spiAttachMOSI(_spi, _mosi);

}

void SPIClass::end()
{
    if(!_spi) {
        return;
    }
    spiDetachSCK(_spi, _sck);
    spiDetachMISO(_spi, _miso);
    spiDetachMOSI(_spi, _mosi);
    setHwCs(false);
    spiStopBus(_spi);
    _spi = NULL;
}

void SPIClass::setHwCs(bool use)
{
    if(use && !_use_hw_ss) {
        spiAttachSS(_spi, 0, _ss);
        spiSSEnable(_spi);
    } else if(_use_hw_ss) {
        spiSSDisable(_spi);
        spiDetachSS(_spi, _ss);
    }
    _use_hw_ss = use;
}

void SPIClass::setFrequency(uint32_t freq)
{
    //check if last freq changed
    uint32_t cdiv = spiGetClockDiv(_spi);
    if(_freq != freq || _div != cdiv) {
        _freq = freq;
        _div = spiFrequencyToClockDiv(_freq);
        spiSetClockDiv(_spi, _div);
    }
}

void SPIClass::setClockDivider(uint32_t clockDiv)
{
    _div = clockDiv;
    spiSetClockDiv(_spi, _div);
}

void SPIClass::setDataMode(uint8_t dataMode)
{
    spiSetDataMode(_spi, dataMode);
}

void SPIClass::setBitOrder(uint8_t bitOrder)
{
    spiSetBitOrder(_spi, bitOrder);
}

void SPIClass::beginTransaction(SPISettings settings)
{
    //check if last freq changed
    uint32_t cdiv = spiGetClockDiv(_spi);
    if(_freq != settings._clock || _div != cdiv) {
        _freq = settings._clock;
        _div = spiFrequencyToClockDiv(_freq);
    }
    spiTransaction(_spi, _div, settings._dataMode, settings._bitOrder);
    _inTransaction = true;
}

void SPIClass::endTransaction()
{
    if(_inTransaction){
        _inTransaction = false;
        spiEndTransaction(_spi);
    }
}

void SPIClass::write(uint8_t data)
{
    if(_inTransaction){
        return spiWriteByteNL(_spi, data);
    }
    spiWriteByte(_spi, data);
}

uint8_t SPIClass::transfer(uint8_t data)
{
    if(_inTransaction){
        return spiTransferByteNL(_spi, data);
    }
    return spiTransferByte(_spi, data);
}

void SPIClass::write16(uint16_t data)
{
    if(_inTransaction){
        return spiWriteShortNL(_spi, data);
    }
    spiWriteWord(_spi, data);
}

uint16_t SPIClass::transfer16(uint16_t data)
{
    if(_inTransaction){
        return spiTransferShortNL(_spi, data);
    }
    return spiTransferWord(_spi, data);
}

void SPIClass::write32(uint32_t data)
{
    if(_inTransaction){
        return spiWriteLongNL(_spi, data);
    }
    spiWriteLong(_spi, data);
}

uint32_t SPIClass::transfer32(uint32_t data)
{
    if(_inTransaction){
        return spiTransferLongNL(_spi, data);
    }
    return spiTransferLong(_spi, data);
}

void SPIClass::transferBits(uint32_t data, uint32_t * out, uint8_t bits)
{
    if(_inTransaction){
        return spiTransferBitsNL(_spi, data, out, bits);
    }
    spiTransferBits(_spi, data, out, bits);
}

/**
 * @param data uint8_t *
 * @param size uint32_t
 */
void SPIClass::writeBytes(uint8_t * data, uint32_t size)
{
    if(_inTransaction){
        return spiWriteNL(_spi, data, size);
    }
    spiSimpleTransaction(_spi);
    spiWriteNL(_spi, data, size);
    spiEndTransaction(_spi);
}

/**
 * @param data void *
 * @param size uint32_t
 */
void SPIClass::writePixels(const void * data, uint32_t size)
{
    if(_inTransaction){
        return spiWritePixelsNL(_spi, data, size);
    }
    spiSimpleTransaction(_spi);
    spiWritePixelsNL(_spi, data, size);
    spiEndTransaction(_spi);
}

/**
 * @param data uint8_t * data buffer. can be NULL for Read Only operation
 * @param out  uint8_t * output buffer. can be NULL for Write Only operation
 * @param size uint32_t
 */
void SPIClass::transferBytes(uint8_t * data, uint8_t * out, uint32_t size)
{
    if(_inTransaction){
        return spiTransferBytesNL(_spi, data, out, size);
    }
    spiTransferBytes(_spi, data, out, size);
}

/**
 * @param data uint8_t *
 * @param size uint8_t  max for size is 64Byte
 * @param repeat uint32_t
 */
void SPIClass::writePattern(uint8_t * data, uint8_t size, uint32_t repeat)
{
	if(size > 64) {
        return;    //max Hardware FIFO
    }

    uint32_t byte = (size * repeat);
    uint8_t r = (64 / size);
    const uint8_t max_bytes_FIFO = r * size;    // Max number of whole patterns (in bytes) that can fit into the hardware FIFO

    while(byte) {
        if(byte > max_bytes_FIFO) {
            writePattern_(data, size, r);
            byte -= max_bytes_FIFO;
        } else {
            writePattern_(data, size, (byte / size));
            byte = 0;
        }
    }
}

void SPIClass::writePattern_(uint8_t * data, uint8_t size, uint8_t repeat)
{
    uint8_t bytes = (size * repeat);
    uint8_t buffer[64];
    uint8_t * bufferPtr = &buffer[0];
    uint8_t * dataPtr;
    uint8_t dataSize = bytes;
    for(uint8_t i = 0; i < repeat; i++) {
        dataSize = size;
        dataPtr = data;
        while(dataSize--) {
            *bufferPtr = *dataPtr;
            dataPtr++;
            bufferPtr++;
        }
    }

    writeBytes(&buffer[0], bytes);
}

///////////////////////////////////////// LCD functions - By Gil Tal /////////////////////////////////////////////////////

void SPIClass::writeGrayScale(uint8_t color, uint32_t repeat)
{
	//writePattern((unsigned char*)&color, 1, repeat*3);
	unsigned int	colorValue, dataSize = repeat * 3, i, j;
	colorValue = color | (color << 8) | (color << 16) | (color << 24);
	
	if (dataSize < SPI_FIFO_SIZE)
	{
		for (i = 0; i < (dataSize / 4 + 1); i++)
		{
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, i*4, colorValue);
		}
		ESP32_SPI_DATA_LEN(ESP32_VSPI, dataSize);
		ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
		while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
	}
	else
	{
		for (i = 0; i < 16; i++)
		{
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, i * 4, colorValue);
		}
		ESP32_SPI_DATA_LEN(ESP32_VSPI, SPI_FIFO_SIZE);
		for (j = 0; j < dataSize / SPI_FIFO_SIZE; j++)
		{
			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}
		// Left overs
		if ((dataSize - j * SPI_FIFO_SIZE) != 0)
		{
			ESP32_SPI_DATA_LEN(ESP32_VSPI, (dataSize - j * SPI_FIFO_SIZE));
			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}
	}
}

void SPIClass::writeBuffer(unsigned int * buffer, unsigned int size) // size is number on ints
{
	unsigned int i, leftOver,index = 0;
	if (size >= 8)
	{
		ESP32_SPI_DATA_LEN(ESP32_VSPI, 32);
		for (i = 0; i < size / 8; i++)
		{
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, buffer[index]);
			index++;
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 4, buffer[index]);
			index++;
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 8, buffer[index]);
			index++;
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 12, buffer[index]);
			index++;
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 16, buffer[index]);
			index++;
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 20, buffer[index]);
			index++;
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 24, buffer[index]);
			index++;
			ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 28, buffer[index]);
			index++;

			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}

		leftOver = size % 8;
		if (leftOver)
		{
			ESP32_SPI_DATA_LEN(ESP32_VSPI, leftOver * 4);
			for (i = 0; i < leftOver; i++)
			{
				ESP32_SPI_FIFO_WRITE(ESP32_VSPI, i*4, buffer[index]);
				index++;
			}

			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}
	}
	else
	{
		leftOver = size % 8;
		if (leftOver)
		{
			ESP32_SPI_DATA_LEN(ESP32_VSPI, leftOver * 4);
			for (i = 0; i < leftOver; i++)
			{
				ESP32_SPI_FIFO_WRITE(ESP32_VSPI, i * 4, buffer[index]);
				index++;
			}

			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}
	}
}

void SPIClass::writeRGB(unsigned int color, uint32_t repeat) // color is 0x00RRGGBB
{
	// Fifo size is 512 bits = 64 bytes
	//writePattern((unsigned char*)&color, 3, repeat);
	unsigned int i, leftOver;
	if (repeat >= 8)
	{
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, (color | (color << 24)));
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 4, ((color >> 8) | (color << 16)));
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 8, ((color << 8) | (color >> 16)));
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 12, (color | (color << 24)));
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 16, ((color >> 8) | (color << 16)));
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 20, ((color << 8) | (color >> 16)));


		ESP32_SPI_DATA_LEN(ESP32_VSPI, 24);
		for (i = 0; i < repeat/8; i++)
		{
			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}
		
		leftOver = repeat % 8;
		if (leftOver)
		{
			ESP32_SPI_DATA_LEN(ESP32_VSPI, leftOver * 3);
			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}
	}
	else
	{
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, color);
		ESP32_SPI_DATA_LEN(ESP32_VSPI, 3);
		for (i = 0; i < repeat; i++)
		{
			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}
	}
}

void SPIClass::writeRGB(unsigned int color) // color is 0x00RRGGBB
{
	//writeBytes((unsigned char*)&color,3);//writeBytes(&buffer[0], bytes);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, color);
	ESP32_SPI_DATA_LEN(ESP32_VSPI, 3);
	ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
	while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
}

void SPIClass::writeBYTE(unsigned char data)
{
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, data);
	ESP32_SPI_DATA_LEN(ESP32_VSPI, 1);
	ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
	while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
}

void SPIClass::writeBYTE(unsigned char data, uint32_t repeat)
{
	// Fifo size is 512 bits = 64 bytes
	unsigned int tempColor = data, color = (tempColor << 24) | (tempColor << 16) | (tempColor << 8) | tempColor;

	unsigned int i, leftOver;
	if (repeat >= 8)
	{
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, color);
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 4, color);
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 8, color);
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 12, color);
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 16, color);
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 20, color);
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 24, color);
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 28, color);

		ESP32_SPI_DATA_LEN(ESP32_VSPI, 32);
		for (i = 0; i < repeat / 8; i++)
		{
			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}

		leftOver = repeat % 8;
		if (leftOver)
		{
			ESP32_SPI_DATA_LEN(ESP32_VSPI, leftOver * 3);
			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}
	}
	else
	{
		ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, color);
		ESP32_SPI_DATA_LEN(ESP32_VSPI, 3);
		for (i = 0; i < repeat; i++)
		{
			ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
			while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
		}
	}
}

void SPIClass::writeUINT(uint32_t data)
{
	data = (data >> 24) | ((data & 0x0000ff00) << 8) | ((data & 0x00ff0000) >> 8) | (data << 24);
	//writeBytes((unsigned char*)&data, 4);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, data);
	ESP32_SPI_DATA_LEN(ESP32_VSPI, 4);
	ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
	while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
}

SPIClass SPI(VSPI);
