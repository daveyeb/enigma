#include "enigma.h"


int main(){
    wheels * cash;
	plugboard *money;

	cash = malloc(sizeof(wheels));
    config_wheel_order(cash, "IV II V BETA I II");
    
	config_ring_settings(cash, "15 23 26 88 99");

	money = malloc(sizeof(plugboard));
	config_plug_connections(money, "EJ OY IV AQ KW FX MT PS LU BD");

	printf("%s\n", money->alpha);


    return 0;
}