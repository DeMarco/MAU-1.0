cd C:\Leitor_MAU
avrdude -c usbasp -p attiny85 -U eeprom:w:Zeroed_Memory.hex:r
