cd C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\AVRdude
avrdude -c usbasp -p attiny85 -U eeprom:r:C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\Firmware\GraphGenerator\eeprom_contents.txt:h
avrdude -c usbasp -p attiny85 -U eeprom:r:C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\Firmware\GraphGenerator\eeprom_contents.bin:r
cd C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\Firmware\GraphGenerator
GraphGenerator
timeout /t -1
