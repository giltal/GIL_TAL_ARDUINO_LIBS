// General Lib - Gil Tal
// Ver 0.1
#ifndef __INCGeneralLibh
#define __INCGeneralLibh

#include <Adafruit_FT6206.h>

struct TouchAreaElement
{
	unsigned int x1, y1;
	unsigned int x2, y2;
	int key;
};

#define nESP8266
#define ESP32

#ifdef ESP8266
	#define TOUCH_PANNEL_TOUCHED()	(analogRead(A0) < 100)
#elif defined(ESP32)
	//#define TOUCH_PANNEL_TOUCHED()	(analogRead(34) < 100)
	#define TOUCH_PANNEL_TOUCHED()	(digitalRead(34) == LOW)
#else // Arduino Due
	#define TOUCH_PANNEL_STATUS()	digitalRead(34)
#endif 

bool readTouchPannel(int * xPos, int * yPos, Adafruit_FT6206 * TS);
bool readTouchPannel(TouchAreaElement * element, Adafruit_FT6206 * TS);

int	 getInputFromTouchPannel(TouchAreaElement elementsArray[], unsigned int numOfElements, Adafruit_FT6206 * TS,unsigned int timeOutMs,bool * timeOut); // TimeOutMs = 0 => no time out

class analogJoyStick
{
private:
	unsigned int XaxisPin, YaxisPin;
	int			 xInitVal, yInitVal;
	int deadZone;
public:
	analogJoyStick(unsigned int analogPinForXaxis, unsigned int analogPinForYaxis, unsigned int deadZoneVal);
	~analogJoyStick();

	int		readXaxis();
	int		readYaxis();
	void	init(); // When called joystick must not touched 
	bool	updateDeadZoneVal(unsigned int deadZoneVal);
};

// Sound driver
// Basic C functions

void sound(unsigned short freq, unsigned short timeInMilliSec);
void sound(unsigned short freq);
void soundOff();

struct note
{
	unsigned short note;
	unsigned short duration; // in 10 miliseconds units
	unsigned short volume; // Used when supported, otherwise ignored 
};

class soundStream
{
private:
	unsigned int noteIndex,arrayIndex;
	note * _notesArray;
	unsigned int _numOfElements;
	bool streamValid,_autoRestart,_active;
public:
	soundStream()
	{
		noteIndex = 0;
		arrayIndex = 0;
		_notesArray = NULL;
		_numOfElements = 0;
		streamValid = false;
		_autoRestart = false;
		_active = false;
	};
	~soundStream() {};
	void initStream(note * arrayPointer, unsigned int numOfEelements,bool autoRestart = false)
	{
		if (arrayPointer == NULL || numOfEelements == 0)
		{
			streamValid = false;
			return;
		}
		_notesArray = arrayPointer;
		_numOfElements = numOfEelements;
		_autoRestart = autoRestart;
	}
	unsigned short tick(); // Move the index one tick forward (10ms) and returns the current tone to play, 0 when nothing to play or empty stream
	bool rewind();
	bool isActive();
	unsigned int numOfElements()
	{
		return _numOfElements;
	}
};

#define MAX_SUPPORTED_SOUND_STREAM 2

/*void IRAM_ATTR soundPlayerTimer()
{
	
};*/

typedef enum soundPlayerCounter {CNTR0 = 0, CNTR1 = 1, CNTR2 = 2, CNTR3 = 3};

// Usage: var SOUND_PLAYER = soundPlayer.getInstance(); SOUND_PLAYER.func();

class soundPlayer
{
private:
	soundStream * soundStreams[MAX_SUPPORTED_SOUND_STREAM];
	hw_timer_t * timer;
	bool		 playerInitialized, active;
	unsigned char _speakerPin;
	unsigned char _PWMchannel;
	soundPlayerCounter _counterNum;

	soundPlayer() 
	{
		playerInitialized = false; 
		active = false;
	};
public:
	~soundPlayer();
	static soundPlayer& getInstance()
	{
		static soundPlayer    instance; // Guaranteed to be destroyed.
							  // Instantiated only on first use.
		return instance;
	}
	soundPlayer(soundPlayer const&) = delete;
	void operator=(soundPlayer const&) = delete;

	bool initPlayer(soundPlayerCounter counterNum, unsigned char PWMchannel, unsigned char speakerPin);
	bool insertStream(unsigned int streamIndex, soundStream * soundStreamPointer);
	static bool start();
	static bool stop();
};

// Music Notes
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

#endif // GeneralLib