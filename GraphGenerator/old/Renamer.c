#include <stdio.h>
#include <stdlib.h>

int main()
{
	char newname[30];
	int test_num;
	
	printf("\n\nType in the test number: ");
	scanf("%d", &test_num);
	printf("\n\n");
	
	sprintf(newname, "T%d_P1_eeprom_contents.bin", test_num);
	if(rename("P1_eeprom_contents.bin", newname))
	{
		printf("\n\n\tErro ao renomear arquivos P1-1!\n\n\n");
	}	
	sprintf(newname, "T%d_P1_eeprom_contents.txt", test_num);
	if(rename("P1_eeprom_contents.txt", newname))
	{
		printf("\n\n\tErro ao renomear arquivos P1-2!\n\n\n");
	}	
	sprintf(newname, "T%d_P1_flight_data_raw.txt", test_num);
	if(rename("P1_flight_data_raw.txt", newname))
	{
		printf("\n\n\tErro ao renomear arquivos P1-3!\n\n\n");
	}	
	sprintf(newname, "T%d_P1_flight_data_true.txt", test_num);
	if(rename("P1_flight_data_true.txt", newname))
	{
		printf("\n\n\tErro ao renomear arquivos P1-4!\n\n\n");
	}
	
	sprintf(newname, "T%d_P2_eeprom_contents.bin", test_num);
	if(rename("P2_eeprom_contents.bin", newname))
	{
		printf("\n\n\tErro ao renomear arquivos P2-1!\n\n\n");
	}	
	sprintf(newname, "T%d_P2_eeprom_contents.txt", test_num);
	if(rename("P2_eeprom_contents.txt", newname))
	{
		printf("\n\n\tErro ao renomear arquivos P2-2!\n\n\n");
	}	
	sprintf(newname, "T%d_P2_flight_data_raw.txt", test_num);
	if(rename("P2_flight_data_raw.txt", newname))
	{
		printf("\n\n\tErro ao renomear arquivos P2-3!\n\n\n");
	}	
	sprintf(newname, "T%d_P2_flight_data_true.txt", test_num);
	if(rename("P2_flight_data_true.txt", newname))
	{
		printf("\n\n\tErro ao renomear arquivos P2-4!\n\n\n");
	}
	
	/*
	sprintf(newname, "T%d_Dados_de_Voo.xlsx", test_num);
	if(rename("Leitura Dados De Vôo.xlsx", newname))
	{
		printf("\n\n\tErro ao renomear arquivos Excel!\n\n\n");
	}
	*/
	
	return 0;
}
