cd C:\Leitor_MAU
avrdude -c usbasp -p attiny85 -U eeprom:r:C:\Leitor_MAU\eeprom_contents.txt:h
avrdude -c usbasp -p attiny85 -U eeprom:r:C:\Leitor_MAU\eeprom_contents.bin:r
GraphGenerator_User
