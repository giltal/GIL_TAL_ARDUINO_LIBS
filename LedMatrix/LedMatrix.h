/*
 Red LED Matrix driver (Using SPI interface) - derived from UTFT lib
 Support: 64x16 - one board, 128x16 - two board side by side, 64x32 - two boards, one under the other
 Using double buffer for flicker free operation, uses an interrupt service routine based on timer set for 10 frames per second
*/

// Choose only one!!!

enum matrixColor {	BLACK_COLOR, 
					WHITE_COLOR,
					RED_COLOR,
					BLUE_COLOR,
					GREEN_COLOR,
					PURPLE_COLOR,
					BROWN_COLOR,
					CYAN_COLOR
};

#ifndef LEDMATRIX_h
#define LEDMATRIX_h

#include "SPI.h"

class LEDmatrix
{
	protected:
		short			maxX,maxY,scanRow;
		char			latchPin, en_74138, la_74138, lb_74138, lc_74138, ld_74138, le_74138;
		unsigned char * frameBuffer[2];
		unsigned char * currentFrameBuffer,* nowDisplayingFrameBuffer;
		bool			frameBufferFlag;
		matrixColor		color;
		
		void swap(short * a, short * b);
						
	public:
		LEDmatrix(); // 0 - VSPI, 1 HSPI
		
		virtual void shiftOutRow() = 0;
		virtual void drawPixel(short x, short y) = 0;
		virtual void clrScr() = 0;
		virtual void fillScr() = 0;
		
		void	swapFrameBuffer();
		void	setColor(matrixColor color);
		void	drawLine(short x1, short y1, short x2, short y2);
		void	drawRect(short x1, short y1, short x2, short y2);
		void	drawRoundRect(short x1, short y1, short x2, short y2);
		void	fillRect(short x1, short y1, short x2, short y2);
		void	fillRoundRect(short x1, short y1, short x2, short y2);
		void	drawCircle(short x, short y, int radius);
		void	fillCircle(short x, short y, int radius);
		void	print(char *st, short x, short y);
		//void	print(String st, short x, short y, int deg=0);
		void	printNumI(long num, short x, short y, int length=0, char filler=' ');
		void	printNumF(double num, char dec, short x, short y, char divider='.', int length=0, char filler=' ');
		////////// Gil add ons
		void	drawHebStringUTF8(short x, short y, const char * str, bool swapString = true);
		int		getStringWidth(const char * str);
		int		getCharWidth(unsigned short c);
		void	drawBitMapFromArray(short x, short y,short * dataArray);
		///////// Gil - Up until here
		
		int		getMatrixXSize();
		int		getMatrixYSize();

		void	drawHLine(short x, short y, int l);
		void	drawVLine(short x, short y, int l);
		void	printChar(char c, short x, short y);
		void	reverseUTF8string(char * str);
		unsigned char printUTF8char(unsigned short c, short x, short y);
		void	dumpFrameBuffer();
};

class RGBLEDmatrix: public LEDmatrix
{
private:
	unsigned int intensity = 500;
public:
	RGBLEDmatrix(char latchPin, char en_74138, char la_74138, char lb_74138, char lc_74138, char ld_74138, char le_74138, char spiInterface = 0,char ySize = 32);
	~RGBLEDmatrix()
	{
		free(frameBuffer[0]);
		free(frameBuffer[1]);
	}

	void shiftOutRow();
	void drawPixel(short x, short y);
	void clrScr();
	void fillScr();
	void setIntensity(unsigned char val);
};

class RedLEDmatrix : public LEDmatrix
{
public:
	RedLEDmatrix(char latchPin, char en_74138, char la_74138, char lb_74138, char lc_74138, char ld_74138, char le_74138, char spiInterface = 0);
	~RedLEDmatrix()
	{
		free(frameBuffer[0]);
		free(frameBuffer[1]);
	}

	void shiftOutRow();
	void drawPixel(short x, short y);
	void clrScr();
	void fillScr();
};



#endif