//#include <ArduinoPins.h>
//#include <FatReader.h>
//#include <FatStructs.h>
//#include <mcpDac.h>
//#include <SdInfo.h>
//#include <SdReader.h>
//#include <Wavemainpage.h>
//#include <WavePinDefs.h>

#include <WaveUtil.h>
#include <WaveHC.h>
#include <FatReader.h>
#include <SdReader.h>
#include <avr/pgmspace.h>
#include <SoftwareSerial.h>

/*
  jukebox
  Based on WaveShield by Lady Ada, using the Waveshield v1.1 kit, Arduino Uno and ID12 RFID reader
  created @ 8.1.12 by dominik grob (@ruedi)
  Adapted April 2018 by Fleeta Chew Siegel @fleetaj
*/

//  audio settings
boolean PlayComplete = true;
long playnext_prevmillis = 0;
long playanother_time = 3000;

//  sdcard settings
SdReader card;
FatVolume vol;
FatReader root;
FatReader f;
WaveHC wave;

//  rfid settings
SoftwareSerial RFIDSerial(8, 6);
int RFIDResetPin = 7;
char RFIDtag[14];
int RFIDindex = 0;
boolean RFIDreading = false;
boolean debug = true;

//  define tag id and tracks- change the number according to numb. of tags
#define NUMTAGS 15
char audiotags[NUMTAGS][14] = {
  "0110FB6642CEC",
  "0110FB65901F",
  "0110FB6546C9",
  "0110FB6DDF58",
  "0110FB69A320",
  "0110FB6921A2",
  "0110FB5D9720",
  "0110FB65A12E",
  "0110FB6573FC",
  
  // These tags are recorded here, but need the sound files replaced below. 
  //The order corresponds to the sound file playes, list order 1-25 = top to bottom.
  "0110FB690C8F",
  "0110FB699E1D",
  "0110FB6D0384",
  "0110FB6D68EF",
  "0110FB664AC6",
  "0110FB5DBA0D",
//  "0110FB6D40C7",
//  "0110FB698D0E",
//  "0110FB6552DD",
//  "0110FB6D78FF",
//  "0110FB65A22D",
 
};

// make sure soundfile names are not longer then 8 chars (without filetype)
char audiofiles[NUMTAGS][14] = {
  "car00.wav",
  "horse00.wav",
  "lion00.wav",
  "cell00.wav",
  "cell00.wav",
  "fog00.wav",
  "hawk01.wav",
  "sch00.wav",
  "hawk01.wav",
  
  // these tags need new sound files associated with them
  // plays the same sounds as the first tags. Copy files to SD card
  "car00.wav",
  "horse00.wav",
  "lion00.wav",
  "cell00.wav",
  "fog00.wav",
  "hawk01.wav",
 // "sch00.wav",
 // "hawk01.wav",
 // "car00.wav",
 // "horse00.wav",
//  "lion00.wav",
  
};

void setup() {
  Serial.begin(9600);
  putstring_nl("debug");
  putstring_nl("----------------");
  putstring("Free RAM: ");
  Serial.println(freeRam());

  //  set output pins for DAC control
  //  pins are defined in the library
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  // pin13 LED
  pinMode(13, OUTPUT);
  if (!card.init()) {
    putstring_nl("Card init. failed!");
    sdErrorCheck();
    while (1);
  }

  // enable optimize read
  card.partialBlockRead(true);

  // fat partition?
  uint8_t part;
  for (part = 0; part < 5; part++) {
    if (vol.init(card, part))
      break;
  }
  if (part == 5) {
    putstring_nl("No valid FAT partition!");
    sdErrorCheck();
    while (1);
  }

  // show infos
  putstring("Using partition ");
  Serial.print(part, DEC);
  putstring(", type is FAT");
  Serial.println(vol.fatType(), DEC);

  if (!root.openRoot(vol)) {
    putstring_nl("Can't open root dir!");
    while (1);
  }

  putstring_nl("> sdcard ready");

  // rfid setup
  pinMode(RFIDResetPin, OUTPUT);
  digitalWrite(RFIDResetPin, HIGH);
  RFIDSerial.begin(9600);
  putstring_nl("> rfid ready");

  // play startup chime
  //delay(500);  // avoid loudspeaker click noise
  //playcomplete("whis00.wav");
}


void loop() {
  RFIDindex = 0;

  //  rfid data?
  while (RFIDSerial.available()) {
    int readByte = RFIDSerial.read();

    if (readByte == 2) RFIDreading = true;
    if (readByte == 3) RFIDreading = false;
    if (RFIDreading && readByte != 2 && readByte != 10 && readByte != 13) {
      RFIDtag[RFIDindex] = readByte;
      RFIDindex++;
    }
  }

  //  check tag and play track if tag id found
  checkTag(RFIDtag);
  //  prepare for next read
  clearTag(RFIDtag);
  resetReader();
}

void playcomplete(char *name) {
  playfile(name);
  while (wave.isplaying) {
    // playing
  }
}

void playfile(char *name) {
  if (wave.isplaying) {
    wave.stop();
  }
  if (!f.open(root, name)) {
    putstring("Couldn't open file "); Serial.println(name); return;
  }
  if (!wave.create(f)) {
    putstring_nl("Not a valid WAV"); return;
  }
  wave.play();
}

void resetReader() {
  digitalWrite(RFIDResetPin, LOW);
  digitalWrite(RFIDResetPin, HIGH);
  delay(150);
}

void clearTag(char one[]) {
  for (int i = 0; i < strlen(one); i++) {
    one[i] = 0;
  }
}

void checkTag(char tag[]) {
  if (strlen(tag) == 0) return;

  if (debug) {
    putstring("rfid tag id: ");
    for (int d = 0; d < 12; d++) {
      Serial.print(tag[d]);
    }
    Serial.println();
  }

  boolean matching = true;

  //  compare tag id
  for (int a = 0; a < NUMTAGS; a++) {
    matching = true;
    for (int c = 0; c < 12; c++) {
      if (tag[c] != audiotags[a][c]) {
        matching = false;
        break;
      }
    }

    //  in case of a match play the track
    if (matching) {
      putstring("playing: "); Serial.println(audiofiles[a]);
      delay(500);  // avoid loudspeaker click noise
      if (PlayComplete) {
        digitalWrite(RFIDResetPin, LOW);
        playcomplete(audiofiles[a]);
      } else {
        unsigned long playnext_currentmillis = millis();
        // wait with playing another audio track
        if (playnext_currentmillis - playnext_prevmillis > playanother_time) {
          playfile(audiofiles[a]);
          playnext_prevmillis = playnext_currentmillis;
        }
      }
      break;
    }
  }
}


// ---------------------------
// debug functions
// ---------------------------

int freeRam(void) {
  extern int  __bss_end;
  extern int  *__brkval;
  int free_memory;
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__bss_end);
  }
  else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
  }
  return free_memory;
}

void sdErrorCheck(void) {
  if (!card.errorCode()) return;
  putstring("\n\rSD I/O error: ");
  Serial.print(card.errorCode(), HEX);
  putstring(", ");
  Serial.println(card.errorData(), HEX);
  while (1);
}
