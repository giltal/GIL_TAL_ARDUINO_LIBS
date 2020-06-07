
#include "LedMatrix.h"
#include <SPI.h>
#include <arduino.h>

#define nESP8266
#ifndef ESP8266
#define ESP32
#endif // !ESP8266

//extern PCF8574 PCF_38;

LEDmatrix::LEDmatrix()
{

}

void LEDmatrix::dumpFrameBuffer()
{
	for (size_t i = 0; i < maxX*maxY*3/32; i++)
	{
		Serial.println(~((unsigned int *)nowDisplayingFrameBuffer)[i],HEX);
	}
}

void LEDmatrix::swapFrameBuffer()
{
	if (frameBufferFlag)
	{
		nowDisplayingFrameBuffer = frameBuffer[1];
		currentFrameBuffer = frameBuffer[0];
		frameBufferFlag = false;
	}
	else
	{
		nowDisplayingFrameBuffer = frameBuffer[0];
		currentFrameBuffer = frameBuffer[1];
		frameBufferFlag = true;
	}
}

void LEDmatrix::drawRect(short x1, short y1, short x2, short y2)
{
	if (x1>x2)
	{
		swap(&x1, &x2);
	}
	if (y1>y2)
	{
		swap(&y1, &y2);
	}

	drawHLine(x1, y1, x2-x1 + 1);
	drawHLine(x1, y2, x2-x1 + 1);
	drawVLine(x1, y1, y2-y1 + 1);
	drawVLine(x2, y1, y2-y1 + 1);
}

void LEDmatrix::drawRoundRect(short x1, short y1, short x2, short y2)
{
	if (x1>x2)
	{
		swap(&x1, &x2);
	}
	if (y1>y2)
	{
		swap(&y1, &y2);
	}
	if ((x2-x1)>4 && (y2-y1)>4)
	{
		drawPixel(x1+1,y1+1);
		drawPixel(x2-1,y1+1);
		drawPixel(x1+1,y2-1);
		drawPixel(x2-1,y2-1);
		drawHLine(x1+2, y1, x2-x1-4);
		drawHLine(x1+2, y2, x2-x1-4);
		drawVLine(x1, y1+2, y2-y1-4);
		drawVLine(x2, y1+2, y2-y1-4);
	}
}

void LEDmatrix::fillRect(short x1, short y1, short x2, short y2)
{
	if (x1>x2)
	{
		swap(&x1, &x2);
	}
	if (y1>y2)
	{
		swap(&y1, &y2);
	}
	
	for (int i=0; i<((y2-y1)/2)+1; i++)
	{
		drawHLine(x1, y1+i, x2-x1);
		drawHLine(x1, y2-i, x2-x1);
	}
}

void LEDmatrix::fillRoundRect(short x1, short y1, short x2, short y2)
{
	if (x1>x2)
	{
		swap(&x1, &x2);
	}
	if (y1>y2)
	{
		swap(&y1, &y2);
	}

	if ((x2-x1)>4 && (y2-y1)>4)
	{
		for (int i=0; i<((y2-y1)/2)+1; i++)
		{
			switch(i)
			{
			case 0:
				drawHLine(x1+2, y1+i, x2-x1-4);
				drawHLine(x1+2, y2-i, x2-x1-4);
				break;
			case 1:
				drawHLine(x1+1, y1+i, x2-x1-2);
				drawHLine(x1+1, y2-i, x2-x1-2);
				break;
			default:
				drawHLine(x1, y1+i, x2-x1);
				drawHLine(x1, y2-i, x2-x1);
			}
		}
	}
}

void LEDmatrix::drawCircle(short x, short y, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
 
	drawPixel(x, y + radius);
	drawPixel(x, y - radius);
	drawPixel(x + radius, y);
	drawPixel(x - radius, y);
 
	while(x1 < y1)
	{
		if(f >= 0) 
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;    
		drawPixel(x + x1, y + y1);
		drawPixel(x - x1, y + y1);
		drawPixel(x + x1, y - y1);
		drawPixel(x - x1, y - y1);
		drawPixel(x + y1, y + x1);
		drawPixel(x - y1, y + x1);
		drawPixel(x + y1, y - x1);
		drawPixel(x - y1, y - x1);
	}
}

void LEDmatrix::fillCircle(short x, short y, int radius)
{
	for(int y1=-radius; y1<=0; y1++) 
		for(int x1=-radius; x1<=0; x1++)
			if(x1*x1+y1*y1 <= radius*radius) 
			{
				drawHLine(x+x1, y+y1, 2*(-x1));
				drawHLine(x+x1, y-y1, 2*(-x1));
				break;
			}
}

void LEDmatrix::setColor(matrixColor color)
{
	this->color = color;
}

void LEDmatrix::drawLine(short x1, short y1, short x2, short y2)
{
	if (y1==y2)
		drawHLine(x1, y1, x2-x1+1);
	else if (x1==x2)
		drawVLine(x1, y1, y2-y1+1);
	else
	{
		unsigned int	dx = (x2 > x1 ? x2 - x1 : x1 - x2);
		short			xstep =  x2 > x1 ? 1 : -1;
		unsigned int	dy = (y2 > y1 ? y2 - y1 : y1 - y2);
		short			ystep =  y2 > y1 ? 1 : -1;
		int				col = x1, row = y1;

		if (dx < dy)
		{
			int t = - (dy >> 1);
			while (true)
			{
				drawPixel(col, row);
				if (row == y2)
					return;
				row += ystep;
				t += dx;
				if (t >= 0)
				{
					col += xstep;
					t   -= dy;
				}
			} 
		}
		else
		{
			int t = - (dx >> 1);
			while (true)
			{
				drawPixel(col, row);
				if (col == x2)
					return;
				col += xstep;
				t += dy;
				if (t >= 0)
				{
					row += ystep;
					t   -= dx;
				}
			} 
		}
	}
}

void LEDmatrix::drawHLine(short x, short y, int l)
{
	if (l<0)
	{
		l = -l;
		x -= l;
	}
	for (int i = x; i < x + l; i++)
		drawPixel(i, y);
}

void LEDmatrix::drawVLine(short x, short y, int l)
{
	if (l<0)
	{
		l = -l;
		y -= l;
	}
	for (int i = y; i < y + l; i++)
		drawPixel(x, i);
}

void LEDmatrix::printChar(char c, short x, short y)
{
	/*
	char i,ch;
	int j;
	int temp; 

	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
	for(j=0;j<cfont.y_size;j++) 
	{
		for (int zz=0; zz<(cfont.x_size/8); zz++)
		{
			ch=pgm_read_byte(&cfont.font[temp+zz]); 
			for(i=0;i<8;i++)
			{   
				
				if((ch&(1<<(7-i)))!=0)   
				{
					setXY(x+i+(zz*8),y+j,x+i+(zz*8)+1,y+j+1);
					setPixel(fgColor);
				} 
			}
		}
		temp+=(cfont.x_size/8);
	}
	*/
}


void LEDmatrix::print(char *st, short x, short y) // Fix
{
	/*
	int stl, i;

	stl = strlen(st);

	x = (maxX)-(stl*cfont.x_size);

	for (i=0; i<stl; i++)
		printChar(*st++, x + (i*(cfont.x_size)), y);
	*/
}
/*
void LEDmatrix::print(String st, short x, short y, int deg)
{
	char buf[st.length()+1];

	st.toCharArray(buf, st.length()+1);
	print(buf, x, y, deg);
}
*/
void LEDmatrix::printNumI(long num, short x, short y, int length, char filler)
{
	char buf[25];
	char st[27];
	boolean neg=false;
	int c=0, f=0;
  
	if (num==0)
	{
		if (length!=0)
		{
			for (c=0; c<(length-1); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=0;
		}
		else
		{
			st[0]=48;
			st[1]=0;
		}
	}
	else
	{
		if (num<0)
		{
			neg=true;
			num=-num;
		}
	  
		while (num>0)
		{
			buf[c]=48+(num % 10);
			c++;
			num=(num-(num % 10))/10;
		}
		buf[c]=0;
	  
		if (neg)
		{
			st[0]=45;
		}
	  
		if (length>(c+neg))
		{
			for (int i=0; i<(length-c-neg); i++)
			{
				st[i+neg]=filler;
				f++;
			}
		}

		for (int i=0; i<c; i++)
		{
			st[i+neg+f]=buf[c-i-1];
		}
		st[c+neg+f]=0;

	}

	print(st,x,y);
}

void LEDmatrix::printNumF(double num, char dec, short x, short y, char divider, int length, char filler)
{
	char st[27];
	boolean neg=false;

	if (dec<1)
		dec=1;
	else if (dec>5)
		dec=5;

	if (num<0)
		neg = true;

	//_convert_float(st, num, length, dec); // Gil Fix
	sprintf(st, "%f", num);

	if (divider != '.')
	{
		for (int i=0; i<sizeof(st); i++)
			if (st[i]=='.')
				st[i]=divider;
	}

	if (filler != ' ')
	{
		if (neg)
		{
			st[0]='-';
			for (int i=1; i<sizeof(st); i++)
				if ((st[i]==' ') || (st[i]=='-'))
					st[i]=filler;
		}
		else
		{
			for (int i=0; i<sizeof(st); i++)
				if (st[i]==' ')
					st[i]=filler;
		}
	}

	print(st,x,y);
}

///////////////////////////////////////////////////////////////// Added by Gil Tal Feb 12 2018

void	LEDmatrix::swap(short * a, short * b)
{
	short c;
	c = *b;
	*b = *a;
	*a = c;
}

#define HEBREW_CHAR_SET	0xd7 // 215
#define SPACE_SIZE 2
#define	FIRST_SUPPORTED_CHAR	39
#define LAST_SUPPORTED_CHAR		58

extern unsigned char matrixFontsNumbersThinType[][10];
extern unsigned char matrixFontsHebrewThinType[][10];

unsigned char LEDmatrix::printUTF8char(unsigned short c, short x, short y)
{
	unsigned char	utfCode = c >> 8;
	unsigned char	charToPrint = c,* pCharData = NULL,charW,charH;
	unsigned short	charLine;
	if (utfCode == 0) // No UTF
	{
		if (charToPrint >= FIRST_SUPPORTED_CHAR && charToPrint <= LAST_SUPPORTED_CHAR)
		{
			pCharData = matrixFontsNumbersThinType[charToPrint - FIRST_SUPPORTED_CHAR];
		}
	}
	if (utfCode == HEBREW_CHAR_SET)
	{
		if (charToPrint >= 0x90 && charToPrint <= 0xaa)
		{
			pCharData = matrixFontsHebrewThinType[charToPrint - 0x90];
		}
	}
	if (pCharData == NULL)
	{
		return 0;
	}
	else
	{
		charW = pCharData[0];
		charH = pCharData[1];
		for (size_t i = 0; i < charH; i++)
		{
			charLine = pCharData[2 + i];
			for (size_t j = 0; j < charW; j++)
			{
				if ((charLine >> j) & 0x1)
				{
					drawPixel(x + charW - (j + 1), y + i);
				}
			}
		}
	}
	if (c == 39) // '
		return charW - 1;
	else
		return charW;
}

void LEDmatrix::drawHebStringUTF8(short x, short y, const char * str, bool swapString)
{
	// Heb Strings are swapped //
	char *	tempStr;
	int *	tempStrNum;
	int i, j,counter = 0,index;
	if (swapString)
	{
		int len = strlen(str);
		tempStr = new char[len+1];
		tempStrNum = new int[len+1];
		for (i = 0; i < len; i++)
		{
			tempStrNum[i] = 0;
		}
		for(i = 0 ; i < len ; i++)
		{
			if (str[i] >= '0' && str[i] <= '9')
			{
				counter++;
				tempStrNum[i] = counter;
			}
			else
				counter = 0;
		}
		index = - 1;
		for(i = len - 1, j = 0; i >= 0; i--, j++)
		{
			if (str[i] >= '0' && str[i] <= '9')
			{
				tempStr[index + tempStrNum[i]] = str[i];
			}
			else
			{
				index = j;
			}
		}
		j = len - 1;
		for (i = 0; i < len; )
		{
			if (str[i] == HEBREW_CHAR_SET)
			{
				tempStr[j - 1] = str[i];
				tempStr[j] = str[i + 1];
				i += 2;
				j -= 2;
			}
			else
			{
				if (!(str[i] >= '0' && str[i] <= '9'))
					tempStr[j] = str[i];
				i += 1;
				j -= 1;
			}
		}
		tempStr[len] = '\0';
	}
	else
	{
		tempStr = (char *)str;
	}

	/////////////////////////////
	int xIndex = x,strLen = strlen(tempStr),tempX,tempY;
	unsigned short * currentChar,tempChar;
	for (i = 0 ; i < strLen;) // UTF8 -> each char is 2 bytes 
	{
		if (tempStr[i] == HEBREW_CHAR_SET) // UTF8 char
		{
			i++;
			if (tempStr[i] >= 0x90 && tempStr[i] <= 0xaa)
			{
				tempChar = (HEBREW_CHAR_SET << 8) | tempStr[i];
				xIndex += printUTF8char(tempChar,xIndex, y);
			}
			i++; // Un-supported UTF8 char
		}
		else // No UTF
		{
			if (tempStr[i] >= FIRST_SUPPORTED_CHAR && tempStr[i] <= LAST_SUPPORTED_CHAR)
			{
				tempChar = tempStr[i];
				xIndex += printUTF8char(tempChar, xIndex, y);
			}
			else if (tempStr[i] == 32) // Space
			{
				xIndex += SPACE_SIZE;
			}
			i++;
		}
	}
	if (swapString)
	{
		delete[] tempStr;
		delete[] tempStrNum;
	}
}

int	LEDmatrix::getCharWidth(unsigned short c)
{
	unsigned char	utfCode = c >> 8;
	unsigned char	charToCalc = c, *pCharData = NULL, charW;
	unsigned short	charLine;

	if (utfCode == 0) // No UTF
	{
		if (charToCalc == 32)
			return SPACE_SIZE;
		if (charToCalc >= FIRST_SUPPORTED_CHAR && charToCalc <= LAST_SUPPORTED_CHAR)
		{
			pCharData = matrixFontsNumbersThinType[charToCalc - FIRST_SUPPORTED_CHAR];
		}
	}
	if (utfCode == HEBREW_CHAR_SET)
	{
		if (charToCalc >= 0x90 && charToCalc <= 0xaa)
		{
			pCharData = matrixFontsHebrewThinType[charToCalc - 0x90];
		}
	}
	if (pCharData == NULL)
	{
		return 0;
	}
	else
	{
		charW = pCharData[0];
	}
	return charW;
}

int	LEDmatrix::getStringWidth(const char * str)
{
	int strSize = 0, strLen = strlen(str), i;
	unsigned short * currentChar, tempChar;
	for (i = 0; i < strLen;) // UTF8 -> each char is 2 bytes 
	{
		if (str[i] == HEBREW_CHAR_SET) // UTF8 char
		{
			i++;
			if (str[i] >= 0x90 && str[i] <= 0xaa)
			{
				tempChar = (HEBREW_CHAR_SET << 8) | str[i];
				strSize += getCharWidth(tempChar);
			}
			i++; // Un-supported UTF8 char
		}
		else // No UTF
		{
			if (str[i] >= FIRST_SUPPORTED_CHAR && str[i] <= LAST_SUPPORTED_CHAR)
			{
				tempChar = str[i];
				strSize += getCharWidth(tempChar);
			}
			else if (str[i] == 32) // Space
			{
				strSize += SPACE_SIZE;
			}
			i++;
		}
	}
	return strSize;
}

void LEDmatrix::reverseUTF8string(char * str)
{
	int len = strlen(str);
	
	char * tempStr = new char[len+1];
	int i, j = len - 1;
	for (i = 0; i < len; )
	{
		if (str[i] == HEBREW_CHAR_SET)
		{
			tempStr[j - 1] = str[i];
			tempStr[j] = str[i + 1];
			i += 2;
			j -= 2;
		}
		else
		{
			tempStr[j] = str[i];
			i += 1;
			j -= 1;
		}
	}

	for (i = 0; i < len; i++)
	{	
		str[i] = tempStr[i];
	}

	delete[] tempStr;
}

////////////////////////////////////////////////////////////////// Gil up to here

int LEDmatrix::getMatrixXSize()
{
	return maxX;
}

int LEDmatrix::getMatrixYSize()
{
	return maxY;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// RGB LED Matrix ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

RGBLEDmatrix::RGBLEDmatrix(char latchPin, char en_74138, char la_74138, char lb_74138, char lc_74138, char ld_74138, char le_74138, char spiInterface, char ySize)
{
	if (spiInterface == 0) // VSPI
	{
#ifdef ESP32
		//void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
		SPI.begin(18, 19, 23, 5);
		SPI.setBitOrder(MSBFIRST);
		SPI.setFrequency(20000000L);
		ESP32_SPI_DIS_MOSI_MISO_FULL_DUPLEX(ESP32_VSPI);
#else
		SPI.pins(14, 12, 13, 15);
		SPI.begin();
		SPI.setBitOrder(MSBFIRST);
		SPI.setFrequency(10000000L); // Board limitation
#endif // ESP32

	}
	else
	{
		printf("HSPI not supported\n");
		// Not supported yet
	}
	this->latchPin = latchPin;
	this->en_74138 = en_74138;
	pinMode(latchPin, OUTPUT);
	pinMode(en_74138, OUTPUT);

	if (la_74138 == 100) // Use shift register to feed the mux
	{
		this->la_74138 = 100;
		this->lb_74138 = 100;
		this->lc_74138 = 100;
		this->ld_74138 = 100;
		this->le_74138 = 100;
	}
	else
	{
		this->la_74138 = la_74138;
		this->lb_74138 = lb_74138;
		this->lc_74138 = lc_74138;
		this->ld_74138 = ld_74138;
		this->le_74138 = le_74138;
		pinMode(la_74138, OUTPUT);
		pinMode(lb_74138, OUTPUT);
		pinMode(lc_74138, OUTPUT);
		pinMode(ld_74138, OUTPUT);
		pinMode(le_74138, OUTPUT);
	}

	// Data Pin and clock are drived by SPI, SPI clk, and MOSI
#ifdef ESP32
	ledcSetup(0, intensity, 8);
	ledcAttachPin(33, 0);
	ledcWriteTone(0, intensity);
	//ledcWrite(0, 255);
	//digitalWrite(en_74138, LOW);
#else
	digitalWrite(en_74138, LOW);
#endif

	maxX = 64;
	if (ySize == 32 || ySize == 64)
	{
		maxY = ySize;
	}
	else
	{
		printf("Iligal ySize value, defaulting to 32.\n");
		maxY = 32;
	}
	
	frameBuffer[0] = (unsigned char*)malloc(8 * maxY * 3);
	frameBuffer[1] = (unsigned char*)malloc(8 * maxY * 3);
	if (frameBuffer[0] == NULL || frameBuffer[1] == NULL)
		Serial.println("Cannot allocate memory buffers!");

	nowDisplayingFrameBuffer = frameBuffer[0];
	currentFrameBuffer = frameBuffer[1];
	for (int i; i < (maxX / 8)*maxY * 3; i++)
	{
		frameBuffer[0][i] = 0x00;
		frameBuffer[1][i] = 0x00;
	}
	frameBufferFlag = true;
	color = RED_COLOR;
	scanRow = 0;
}
#define TIMING_SCHEME 2
#if 1
void RGBLEDmatrix::shiftOutRow()
{
	static unsigned int redData0, redData1, redData2, redData3;
	static unsigned int greenData0, greenData1, greenData2, greenData3;
	static unsigned int blueData0, blueData1, blueData2, blueData3;

#ifdef ESP32	
	ledcDetachPin(33);
	digitalWrite(en_74138, HIGH);

	bool d0, d1, d2, d3, d4;
	d0 = ((scanRow & 0x1) == 0x1);
	d1 = ((scanRow & 0x2) == 0x2);
	d2 = ((scanRow & 0x4) == 0x4);
	d3 = ((scanRow & 0x8) == 0x8);
	if (maxY == 64)
	{
		d4 = ((scanRow & 0x10) == 0x10);
	}
	else
		d4 = 0;

	if (la_74138 != 100)
	{
		digitalWrite(la_74138, d0);
		digitalWrite(lb_74138, d1);
		digitalWrite(lc_74138, d2);
		digitalWrite(ld_74138, d3);
		digitalWrite(le_74138, d4);
	}

#else	
	digitalWrite(en_74138, HIGH); // Turn off display
#endif
#if (TIMING_SCHEME == 1)
								  /////// Scan Row
	blueData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0];
	blueData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1];
	blueData2 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY];
	blueData3 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY];

	redData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 2];
	redData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 2];
	redData2 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 2 + maxY];
	redData3 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 2 + maxY];

	greenData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 4];
	greenData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 4];
	greenData2 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 4 + maxY];
	greenData3 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 4 + maxY];
	digitalWrite(latchPin, LOW);
#else
	redData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0];
	redData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1];
	redData2 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY];
	redData3 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY];

	greenData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 2];
	greenData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 2];
	greenData2 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 2 + maxY];
	greenData3 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 2 + maxY];

	blueData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 4];
	blueData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 4];
	blueData2 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 4 + maxY];
	blueData3 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 4 + maxY];
	digitalWrite(latchPin, HIGH);
#endif
	/////// 
#ifdef ESP32
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, blueData2);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 4, blueData3);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 8, greenData2);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 12, greenData3);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 16, redData2);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 20, redData3);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 24, blueData0);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 28, blueData1);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 32, greenData0);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 36, greenData1);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 40, redData0);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 44, redData1);

	ESP32_SPI_DATA_LEN(ESP32_VSPI, 12 * 4);
	ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
	while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
#else
	static volatile	unsigned int * fifoPtr = &SPI1W0;
	static const unsigned int mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
	/*
		static unsigned int bits = 64 * 6 - 1 + 8;
		fifoPtr[0] = blueData2;
		fifoPtr[1] = blueData3;
		fifoPtr[2] = greenData2;
		fifoPtr[3] = greenData3;

		fifoPtr[4] = redData2;
		fifoPtr[5] = redData3;
		fifoPtr[6] = blueData0;
		fifoPtr[7] = blueData1;

		fifoPtr[8] = greenData0;
		fifoPtr[9] = greenData1;
		fifoPtr[10] = redData0;
		fifoPtr[11] = redData1;

		fifoPtr[12] = (scanRow);
	*/
	static unsigned int bits = 64 * 6 - 1 + 32;
	fifoPtr[0] = (scanRow) << 24;

	fifoPtr[1] = blueData2;
	fifoPtr[2] = blueData3;
	fifoPtr[3] = greenData2;
	fifoPtr[4] = greenData3;

	fifoPtr[5] = redData2;
	fifoPtr[6] = redData3;
	fifoPtr[7] = blueData0;
	fifoPtr[8] = blueData1;

	fifoPtr[9] = greenData0;
	fifoPtr[10] = greenData1;
	fifoPtr[11] = redData0;
	fifoPtr[12] = redData1;

	SPI1U = SPIUMOSI | SPIUSSE;
	SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
	SPI1CMD |= SPIBUSY;
	while (SPI1CMD & SPIBUSY) {}
#endif
	// Force 74HC595 lo latch data	
#if (TIMING_SCHEME == 1)
	digitalWrite(latchPin, HIGH);
#else
	digitalWrite(latchPin, LOW);
#endif

#ifdef ESP32
	ledcAttachPin(33, 0);
	ledcWriteTone(0, intensity);
	//digitalWrite(en_74138, LOW);
#else
	digitalWrite(en_74138, LOW);     // Turn on display
#endif
	scanRow++;                       // Do the next pair of rows next time this routine is called
	if (scanRow == maxY / 2)
		scanRow = 0;
}
#else
void RGBLEDmatrix::shiftOutRow()
{
	static unsigned int redData0, redData1, redData2, redData3;
	static unsigned int greenData0, greenData1, greenData2, greenData3;
	static unsigned int blueData0, blueData1, blueData2, blueData3;

	//ledcDetachPin(33);
	digitalWrite(en_74138, HIGH); // Turn off display
								  /////// Scan Row
	redData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0];
	redData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1];
	redData2 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY];
	redData3 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY];

	greenData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 2];
	greenData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 2];
	greenData2 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 2 + maxY];
	greenData3 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 2 + maxY];

	blueData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 4];
	blueData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 4];
	blueData2 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0 + maxY * 4 + maxY];
	blueData3 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1 + maxY * 4 + maxY];

	/////// 
#ifdef ESP32
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, blueData2);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 4, blueData3);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 8, greenData2);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 12, greenData3);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 16, redData2);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 20, redData3);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 24, blueData0);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 28, blueData1);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 32, greenData0);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 36, greenData1);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 40, redData0);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 44, redData1);

	ESP32_SPI_DATA_LEN(ESP32_VSPI, 12*4);
	ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
	while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
#else
	static volatile	unsigned int * fifoPtr = &SPI1W0;
	static const unsigned int mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
/*
	static unsigned int bits = 64 * 6 - 1 + 8;
	fifoPtr[0] = blueData2;
	fifoPtr[1] = blueData3;
	fifoPtr[2] = greenData2;
	fifoPtr[3] = greenData3;

	fifoPtr[4] = redData2;
	fifoPtr[5] = redData3;
	fifoPtr[6] = blueData0;
	fifoPtr[7] = blueData1;

	fifoPtr[8] = greenData0;
	fifoPtr[9] = greenData1;
	fifoPtr[10] = redData0;
	fifoPtr[11] = redData1;

	fifoPtr[12] = (scanRow);
*/
	static unsigned int bits = 64 * 6 - 1 + 32;
	fifoPtr[0] = (scanRow) << 24;

	fifoPtr[1] = blueData2;
	fifoPtr[2] = blueData3;
	fifoPtr[3] = greenData2;
	fifoPtr[4] = greenData3;

	fifoPtr[5] = redData2;
	fifoPtr[6] = redData3;
	fifoPtr[7] = blueData0;
	fifoPtr[8] = blueData1;

	fifoPtr[9] = greenData0;
	fifoPtr[10] = greenData1;
	fifoPtr[11] = redData0;
	fifoPtr[12] = redData1;	

	SPI1U = SPIUMOSI | SPIUSSE;
	SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
	SPI1CMD |= SPIBUSY;
	while (SPI1CMD & SPIBUSY) {}
#endif

	// Force 74HC595 lo latch data
	digitalWrite(latchPin, LOW);
	digitalWrite(latchPin, HIGH);
	bool d0, d1, d2, d3, d4;
	d0 = ((scanRow & 0x1) == 0x1);
	d1 = ((scanRow & 0x2) == 0x2);
	d2 = ((scanRow & 0x4) == 0x4);
	d3 = ((scanRow & 0x8) == 0x8);
	if (maxY == 64)
	{
		d4 = ((scanRow & 0x10) == 0x10);
	}
	else
		d4 = 0;
	
	if (la_74138 != 100)
	{
		digitalWrite(la_74138, d0);
		digitalWrite(lb_74138, d1);
		digitalWrite(lc_74138, d2);
		digitalWrite(ld_74138, d3);
		digitalWrite(le_74138, d4);
		/*switch (scanRow)
		{
		case 0:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		case 1:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		case 2:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		case 3:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		case 4:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 0);
			break;
		case 5:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 0);
			break;
		case 6:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 0);
			break;
		case 7:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 0);
			break;
		case 8:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 1);
			break;
		case 9:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 1);
			break;
		case 10:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 1);
			break;
		case 11:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 1);
			break;
		case 12:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 1);
			break;
		case 13:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 1);
			break;
		case 14:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 1);
			break;
		case 15:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 1);
			break;
		default:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		}*/
	}

#ifdef nESP32
	ledcAttachPin(33, 0);
	ledcWriteTone(0, 0);// intensity);
#else
	digitalWrite(en_74138, LOW);     // Turn on display
#endif
	digitalWrite(latchPin, LOW);	
	scanRow++;                       // Do the next pair of rows next time this routine is called
	if (scanRow == maxY/2)
		scanRow = 0;
}
#endif

#define DRAW_RED_OFF	currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 0] = currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 0] & ~(0x80 >> ((unsigned char)x % 8))
#define DRAW_GREEN_OFF	currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 1] = currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 1] & ~(0x80 >> ((unsigned char)x % 8))
#define DRAW_BLUE_OFF	currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 2] = currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 2] & ~(0x80 >> ((unsigned char)x % 8))

#define DRAW_RED_ON		currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 0] = currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 0] | (0x80 >> ((unsigned char)x % 8))
#define DRAW_GREEN_ON	currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 1] = currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 1] | (0x80 >> ((unsigned char)x % 8))
#define DRAW_BLUE_ON	currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 2] = currentFrameBuffer[y*maxX / 8 + x / 8 + 8 * maxY * 2] | (0x80 >> ((unsigned char)x % 8))

void RGBLEDmatrix::drawPixel(short x, short y)
{
	if ((x >= maxX || x < 0) || (y >= maxY || y < 0))
		return;
	switch (color)
	{
	case BLACK_COLOR:
		DRAW_RED_OFF;
		DRAW_BLUE_OFF;
		DRAW_GREEN_OFF;
		break;
	case WHITE_COLOR:
		DRAW_RED_ON;
		DRAW_BLUE_ON;
		DRAW_GREEN_ON;
		break;
	case RED_COLOR:
		DRAW_RED_ON;
		DRAW_BLUE_OFF;
		DRAW_GREEN_OFF;
		break;
	case BLUE_COLOR:
		DRAW_RED_OFF;
		DRAW_BLUE_ON;
		DRAW_GREEN_OFF;
		break;
	case GREEN_COLOR:
		DRAW_RED_OFF;
		DRAW_BLUE_OFF;
		DRAW_GREEN_ON;
		break;
	case PURPLE_COLOR:
		DRAW_RED_ON;
		DRAW_BLUE_ON;
		DRAW_GREEN_OFF;
		break;
	case BROWN_COLOR:
		DRAW_RED_ON;
		DRAW_BLUE_OFF;
		DRAW_GREEN_ON;
		break;
	case CYAN_COLOR:
		DRAW_RED_OFF;
		DRAW_BLUE_ON;
		DRAW_GREEN_ON;
		break;
	default:
		break;
	}
}

void RGBLEDmatrix::clrScr()
{
	for (int i = 0; i < (maxX / 8)*maxY * 3; i++)
	{
		currentFrameBuffer[i] = 0x00;
	}
}

void RGBLEDmatrix::fillScr()
{
	// Need to implement
}

void RGBLEDmatrix::setIntensity(unsigned char val)
{
	// map(value, fromLow, fromHigh, toLow, toHigh)
	intensity = map(val, 0, 255, 500, 6000);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// RED LED Matrix ///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

RedLEDmatrix::RedLEDmatrix(char latchPin, char en_74138, char la_74138, char lb_74138, char lc_74138, char ld_74138, char le_74138, char spiInterface)
{
	if (spiInterface == 0) // VSPI
	{
#ifdef ESP32
		SPI.begin(18, 19, 23, 5);
		SPI.setBitOrder(MSBFIRST);
		SPI.setFrequency(25000000L);
		ESP32_SPI_DIS_MOSI_MISO_FULL_DUPLEX(ESP32_VSPI);
#else
		SPI.pins(14, 12, 13, 15);
		SPI.begin();
		SPI.setBitOrder(MSBFIRST);
		SPI.setFrequency(20000000L); // 8266 cannot run at 25MHz		
#endif // ESP32

	}
	else
	{
		// Not supported yet
	}
	this->latchPin = latchPin;
	this->en_74138 = en_74138;
	pinMode(latchPin, OUTPUT);
	pinMode(en_74138, OUTPUT);

	if (la_74138 == 100)
	{
		this->la_74138 = 100;
		this->lb_74138 = 100;
		this->lc_74138 = 100;
		this->ld_74138 = 100;
	}
	else
	{
		this->la_74138 = la_74138;
		this->lb_74138 = lb_74138;
		this->lc_74138 = lc_74138;
		this->ld_74138 = ld_74138;
		pinMode(la_74138, OUTPUT);
		pinMode(lb_74138, OUTPUT);
		pinMode(lc_74138, OUTPUT);
		pinMode(ld_74138, OUTPUT);
	}

	// Data Pin and clock are drived by SPI, SPI clk, and MOSI
	digitalWrite(en_74138, LOW);

	maxX = 64;
	maxY = 16;
	frameBuffer[0] = (unsigned char*)malloc(8 * 16);
	frameBuffer[1] = (unsigned char*)malloc(8 * 16);

	if (frameBuffer[0] == NULL || frameBuffer[1] == NULL)
		Serial.println("Cannot allocate memory buffers!");

	nowDisplayingFrameBuffer = frameBuffer[0];
	currentFrameBuffer = frameBuffer[1];
	for (int i; i < (maxX / 8)*maxY; i++)
	{
		frameBuffer[0][i] = 0xff;
		frameBuffer[1][i] = 0xff;
	}
	frameBufferFlag = true;
	color = RED_COLOR;
	scanRow = 0;
}

void RedLEDmatrix::shiftOutRow()
{
	static unsigned int tempData0, tempData1;

	digitalWrite(en_74138, HIGH); // Turn off display
								  /////// Scan Row
	tempData0 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 0];
	tempData1 = ((unsigned int *)nowDisplayingFrameBuffer)[scanRow * 2 + 1];

	/////// 
#ifdef ESP32
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 0, tempData0);
	ESP32_SPI_FIFO_WRITE(ESP32_VSPI, 4, tempData1);
	ESP32_SPI_DATA_LEN(ESP32_VSPI, 8);
	ESP32_SPI_EXECUTE_USER_CMD(ESP32_VSPI);
	while (ESP32_SPI_USER_CMD_STATUS(ESP32_VSPI));
#else
	static volatile	unsigned int * fifoPtr = &SPI1W0;
	static const unsigned int mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
	static unsigned int bits = 63 + 8;

	fifoPtr[0] = tempData0;
	fifoPtr[1] = tempData1;
	fifoPtr[2] = (scanRow);

	SPI1U = SPIUMOSI | SPIUSSE;
	SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
	SPI1CMD |= SPIBUSY;
	while (SPI1CMD & SPIBUSY) {}
#endif

	// Force 74HC595 lo latch data
	digitalWrite(latchPin, LOW);
	digitalWrite(latchPin, HIGH);

	if (la_74138 != 100)
	{
		switch (scanRow)
		{
		case 0:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		case 1:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		case 2:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		case 3:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		case 4:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 0);
			break;
		case 5:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 0);
			break;
		case 6:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 0);
			break;
		case 7:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 0);
			break;
		case 8:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 1);
			break;
		case 9:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 1);
			break;
		case 10:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 1);
			break;
		case 11:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 1);
			break;
		case 12:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 1);
			break;
		case 13:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 1);
			break;
		case 14:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 1);
			break;
		case 15:
			digitalWrite(la_74138, 1);
			digitalWrite(lb_74138, 1);
			digitalWrite(lc_74138, 1);
			digitalWrite(ld_74138, 1);
			break;
		default:
			digitalWrite(la_74138, 0);
			digitalWrite(lb_74138, 0);
			digitalWrite(lc_74138, 0);
			digitalWrite(ld_74138, 0);
			break;
		}
	}

	digitalWrite(en_74138, LOW);     // Turn on display
	scanRow++;                       // Do the next pair of rows next time this routine is called
	if (scanRow == 16)
		scanRow = 0;
}

void RedLEDmatrix::drawPixel(short x, short y)
{
	if ((x >= maxX || x < 0) || (y >= maxY || y < 0))
		return;
	if (color != BLACK_COLOR)
		currentFrameBuffer[y*maxX / 8 + x / 8] = currentFrameBuffer[y*maxX / 8 + x / 8] & ~(0x80 >> ((unsigned char)x % 8));
	else
		currentFrameBuffer[y*maxX / 8 + x / 8] = currentFrameBuffer[y*maxX / 8 + x / 8] | (0x80 >> ((unsigned char)x % 8));
}

void RedLEDmatrix::clrScr()
{
	for (int i = 0; i < (maxX / 8)*maxY; i++)
	{
		currentFrameBuffer[i] = 0xff;
	}
}

void RedLEDmatrix::fillScr()
{
	for (int i = 0; i < (maxX / 8)*maxY; i++)
	{
		currentFrameBuffer[i] = 0x0;
	}
}


