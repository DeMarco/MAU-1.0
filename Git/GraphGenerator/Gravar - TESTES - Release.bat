cd C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\AVRdude
avrdude -c usbasp -p attiny85 -F -U flash:w:C:\Users\DRAYCON\Dropbox\UP\2016\TCC\Implementacao\Firmware\Testes\Release\Testes.hex:i
timeout /t -1
