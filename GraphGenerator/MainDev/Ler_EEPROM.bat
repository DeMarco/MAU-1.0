cd C:\Leitor_MAU
del eeprom_contents.txt
del eeprom_contents.bin
del flight_data_raw.txt
del flight_data_true.txt
del timestamps.txt
avrdude -c usbasp -p attiny85 -U eeprom:r:C:\Leitor_MAU\eeprom_contents.txt:h
avrdude -c usbasp -p attiny85 -U eeprom:r:C:\Leitor_MAU\eeprom_contents.bin:r
GraphGenerator
