#include "enigma.h"



int main()
{
	wheels *cash;
	plugboard *money;

	cash = malloc(sizeof(wheels));
	money = malloc(sizeof(plugboard));
	int a = config_wheel_order(cash, "III II I C_THIN");
	a = config_start_pos_rotors(cash, "V E Q");
	a = config_plug_connections(money, "");
	a = config_reflector_wiring(cash, "BETA", "IU AS DV GL FT OX EZ CH MR KN BQ PW");
	a = config_ring_settings(cash, "1 1 1");
	a = encrypt(cash, money, "FRUGAL");

	printf("\na = %d\n", a);

	free(cash);
	free(money);

	// printf("%s\n", money->alpha);

	return 0;
}
