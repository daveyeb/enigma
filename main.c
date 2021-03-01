#include "enigma.h"



int main()
{
	wheels *cash;
	plugboard *money;

	int a = 0;

	cash = malloc(sizeof(wheels));
	money = malloc(sizeof(plugboard));
	a = config_wheel_order(cash, "B I II III");
	a = config_start_pos_rotors(cash, "A A A");
	a = config_plug_connections(money, "");
	a = config_reflector_wiring(cash, "BETA", "IU AS DV GL FT OX EZ CH MR KN BQ PW");
	a = config_ring_settings(cash, "1 1 1");
	a = encrypt(cash, money, "B");

	printf("\na = %d\n", a);

	free(cash);
	free(money);

	// printf("%s\n", money->alpha);

	return 0;
}
