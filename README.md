# Embodiology-Work-Play
Performance using RFID tags to generate sounds from an Arduino.

* Please read through all the information to understand how these components work and how to setup the hardware and interface with the software.

Emobodiology software and hardware information.

- The Hardware consists of the Arduino and a WaveShield component connected to an RFID reader. When the RFID reader picks up a signal from the RFID tag, it plays a certain ‘wav’ file stored on the SD card, which is in turn connected to the WaveShield, which is attached to the  Arduino.
- The songs are played out through the mini-jack stereo connection and there is a volume control dial to adjust the sound output. Both of these controls are on the WaveShield.
- If there are software changes to be made, i.e.: adjust or change the sound files being played, then you will need to connect the Arduino to a computer via the USB socket [a USB cable is provided].
- The sound files need to be converted into 44.1 or 22.050 Mono, 16 but files. You can use iTunes or Audacity to convert the sound files.
- The entire component needs to be wired as illustrated in image 01. It can be powered by a 9v battery or via the USB port- but not both! The 9v battery cable needs to be connected as illustrated.
- The sound files need to finish playing BEFORE another RFID tag is presented to the reader. It cannot play more than one song at a time.
- The specific RFID tags are numbered, 1-25. Only a specific bunch have been associated with sound files. Open the Arduino file labeled ‘embodiology.ino’ to see the program and adjust the sound file names accordingly. Instructions are included in the software as comments- anything.
 

Links to relavent software:
Arduino v.1: https://www.arduino.cc/en/Main/OldSoftwareReleases#1.0.x
[There are more recent releases, but this is the version used for creating this program]

SD Card formatter: https://www.sdcard.org/downloads/formatter_4/
[Use this software to re-format the SD card, as when copying files to the SD card, the data structure can become unreliable.]

Audacity: https://sourceforge.net/projects/audacity/
[Use this software to open and edit the sound files. See above for the correct file formats.]


