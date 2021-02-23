#include "enigma.h"


void  increase_alpha(int & c){
	if(*c < 65 || *c > 90) return;

	if(++*c > 90) *c -= 26;

}



int main()
{
	wheels *cash;
	plugboard *money;

	cash = malloc(sizeof(wheels));
	money = malloc(sizeof(plugboard));
	int a = config_wheel_order(cash, "IV I VII C_THIN");
	a = config_start_pos_rotors(cash, "A B N");
	a = config_plug_connections(money, "");
	a = config_reflector_wiring(cash, "BETA", "IU AS DV GL FT OX EZ CH MR KN BQ PW");
	a = config_ring_settings(cash, "1 1 1");
	a = encrypt(cash, money, "FRUGAL");

	printf("\na = %d\n", a);

	int al = 'Z';

	increase_alpha(al);

	printf("increased al : %c\n", al);

	free(cash);
	free(money);

	// printf("%s\n", money->alpha);

	return 0;
}
