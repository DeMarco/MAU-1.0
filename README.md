View this project on [CADLAB.io](https://cadlab.io/project/1285). 

# MAU-1.0
First version of MAU - MicroAltímetro Universal - or "Universal MicroAltimeter" (my Final Year Project)

This work, including the circuit and PCB layout files, is protected by GNU GPL v3.0.

Folder structure explained: 

Delivery_Clientes --> basically the files that any customer will need to extract data from the altimeter

Delivery_Montagem --> the files needed by whoever is programming the altimeter before delivery o customers

Firmware/MainDev --> source code for the master branch. As of pull request #7, a new feature called "Flex Sampling Rate" is introduced in the master branch, but not yet tested in any real life rocket. Also several minor improvements to the main source code were added, such as better variable names and better comments in English.

Firmware/PrototypeOne --> This folder contains a preserved copy of the "classic" source code as of end of 2018, before the introduction of "Flex Sampling Rate". "P1V16.c" and "definesV16.h" represent the currently commercialized version of the firmware!

Firmware/Tests --> "scratch pad" projects just to test coding ideas

GraphGenerator/MainDev --> source code for the C app that bridges the communication between the altimeter via "USBasp" programmer/reader and a windows PC. It also generates the necessary txt files needed by Excel file "Leitura Dados De Vôo - V1.1.xlsm" to display the altitude graph.

GraphGenerator/old --> preserved copy of the the old C app and Excel files that are compatible with the "classic" firmware version ("PrototypeOne").

Hardware --> the EAGLE files (.brd and .sch) for the hardware design of both the altimeter and the adapter needed to connect it with the USBasp programmer/reader. These files can be visualized in the linked CADLAB.io repo.

Notes --> random scratch pad files with calculations and annotations

TestesPC --> "scratch pad" projects just to test coding ideas on the PC side
