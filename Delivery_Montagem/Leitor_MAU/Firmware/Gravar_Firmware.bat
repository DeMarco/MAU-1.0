cd C:\Leitor_MAU
avrdude -c usbasp -p attiny85 -U flash:w:C:\Leitor_MAU\Firmware\firmware.hex:i
avrdude -c usbasp -p attiny85 -U eeprom:w:Zeroed_Memory.hex:r
timeout /t -1
