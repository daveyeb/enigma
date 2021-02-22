#ifndef ENIGMA
#define ENIGMA

// if sandbox

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define rotor_no(s) #s

typedef struct wiring
{
	int no;
	char *value;
} wiring;

typedef struct rotor
{
	int is_reflector;
	char *alpha;
	char *model_name;
	char *notch;
} rotor;

typedef struct wheels
{
	rotor *_rotors;
	size_t n_rotors;
	int is_config;
	int ring[4];
} wheels;

typedef struct plugboard
{
	char *alpha;
	int stecker_count;
} plugboard;

static inline int config_wheel_order(wheels *_wheels, const char *w_order);
static inline int config_ring_settings(wheels *_wheels, const char *r_settings);
// static inline int config_reflector_wiring( wheels * _wheels, char * r_wiring);
static inline int config_start_pos_rotors(wheels *_wheels, char *w_groups);
static inline int config_plug_connections(plugboard *_plugboard, const char *p_connections);
static inline char *encrypt(wheels *_wheels, plugboard *_plugboard, char *plaintext);

static inline int config_wheel_order(wheels *_wheels, const char *w_order)
{
	int index = 0;
	int model_or_wire = 0;
	int reflector_cnt = 0;

	char buf[4096];
	char *w_tokens[256];
	char *pch;
	char *w_order_cpy;

	for (index = 0; index < 256; index++)
		w_tokens[index] = NULL;

	w_order_cpy = malloc(strlen(w_order) + 1);
	strcpy(w_order_cpy, w_order);

	index = 0;
	pch = strtok(w_order_cpy, " ");
	w_tokens[index++] = pch;
	while (pch != NULL)
	{
		if (index > 6)
			return 0;
		pch = strtok(NULL, " ");
		w_tokens[index++] = pch;
	}

	_wheels->_rotors = malloc(6 * sizeof(rotor));
	_wheels->is_config = 0;

	FILE *f = fopen("./ROTORS", "r");

	if (f == NULL)
		return 0;

	while (fgets(buf, 4096, f) != NULL)
	{
		if (strlen(buf) > 0 && buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';

		if (strlen(buf) == 0)
		{
			model_or_wire = 0;
			continue;
		}
		if (model_or_wire == 0)
		{
			for (index = 0; index < 6; index++)
			{
				if (strcmp(w_tokens[index], buf) == 0)
				{
					model_or_wire = 1;
					break;
				}
			}
		}
		else
		{
			char *ttok = w_tokens[index];

			for (; index < 6; index++)
			{
				if (strcmp(w_tokens[index], ttok) != 0)
					continue;
				rotor r;

				pch = strchr(buf + 1, '/');

				if (pch == NULL)
				{

					reflector_cnt++;
					if (reflector_cnt == 2)
						return 0;

					r.is_reflector = 1;

					r.alpha = malloc(strlen(buf + 1) + 1);
					strcpy(r.alpha, buf + 1);

					r.model_name = malloc(strlen(w_tokens[index]) + 1);
					strcpy(r.model_name, w_tokens[index]);

					r.notch = NULL;
				}
				else
				{

					r.is_reflector = 0;

					r.alpha = malloc(strlen(buf + 1) + 1);
					strncpy(r.alpha, buf + 1, 26);

					r.model_name = malloc(strlen(w_tokens[index]) + 1);
					strcpy(r.model_name, w_tokens[index]);

					r.notch = malloc(3);
					strcpy(r.notch, buf + 28);
				}

				_wheels->_rotors[index] = r;
			}

			model_or_wire = 0;
		}

		_wheels->is_config = 1;
	}

	free(w_order_cpy);
	return 1;
}

static inline int config_ring_settings(wheels *_wheels, const char *r_settings){
	
	char * pch;
	int index;
	char *r_settings_cpy;

	r_settings_cpy = malloc(strlen(r_settings) + 1);
	strcpy(r_settings_cpy, r_settings);

	pch = strtok(r_settings_cpy, " ");
	
	index = 0;
	_wheels->ring[index++] = atoi(pch);
	
	while(pch != NULL){
		if(strlen(pch) > 2 || index > 4) return 0;
		_wheels->ring[index++] = atoi(pch);
		pch = strtok(NULL, " ");
	}


	free(r_settings_cpy);
	return 1;
}

static inline int config_plug_connections(plugboard *_plugboard, const char *p_connections) {
	char * p_connections_cpy;
	char * pch;
	int index;

	p_connections_cpy = malloc(strlen(p_connections) + 1);
	strcpy(p_connections_cpy, p_connections);

	_plugboard->alpha = malloc(27);
	_plugboard->stecker_count = 0;

	index = 64;
	while(++index < 91){
		_plugboard->alpha[(index % 65)] = index;
	}

	pch = strtok(p_connections_cpy, " ");

	index = 0;
	while(pch != NULL){
		if(strlen(pch) > 2 || _plugboard->stecker_count++ > 13) return 0;

		_plugboard->alpha[(pch[0] % 65)] = pch[1];
		_plugboard->alpha[(pch[1] % 65)] = pch[0];

		pch = strtok(NULL, " ");
	}


	return 1;
}


#endif // ENIGMA
