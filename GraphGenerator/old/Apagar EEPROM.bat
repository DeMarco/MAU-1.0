cd C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\AVRdude
avrdude -c usbasp -p attiny85 -U eeprom:w:C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\Firmware\All_0xFF:r
timeout /t -1
