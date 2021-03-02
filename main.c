#include "enigma.h"



int main()
{
	_enigma_wheels *cash;
	_enigma_plugboard *money;

	cash = malloc(sizeof(_enigma_wheels));
	money = malloc(sizeof(_enigma_plugboard));
  
	int a = config_wheel_order(cash, "B I II III");
	a = config_start_pos_rotors(cash, "A A A");
	a = config_plug_connections(money, "");
	a = config_reflector_wiring(cash, "BETA", "IU AS DV GL FT OX EZ CH MR KN BQ PW");
	a = config_ring_settings(cash, "1 1 1");

	a = encrypt(cash, money, "MDQQI");

	printf("\na = %d\n", a);

	free(cash);
	free(money);


	return 0;
}
