c:\avrdude\avrdude -c usbasp -p attiny85 -U flash:w:Release\MainDev.hex:i
c:\avrdude\avrdude -c usbasp -p attiny85 -U eeprom:w:Blank_Memory.hex:r
timeout /t -1
