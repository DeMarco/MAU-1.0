cd C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\AVRdude
avrdude -c usbasp -p attiny85 -U flash:w:C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\Firmware\PrototypeOne\Release\PrototypeOne.hex:i
timeout /t -1
