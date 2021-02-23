#include "enigma.h"

int main()
{
	wheels *cash;
	plugboard *money;

	cash = malloc(sizeof(wheels));
	money = malloc(sizeof(plugboard));
	int a = config_wheel_order(cash, "IV I III C_THIN");
	a = config_start_pos_rotors(cash, "A B N");
	a = config_plug_connections(money, "EJ OY IV AQ KW FX MT PS LU BD");
	a = config_reflector_wiring(cash, "BETA", "IU AS DV GL FT OX EZ CH MR KN BQ PW");

	config_ring_settings(cash, "AB");

	printf("\na = %d\n", a);

	// encrypt(cash, money, "FRUGAL")

	free(cash);

	// printf("%s\n", money->alpha);

	return 0;
}
