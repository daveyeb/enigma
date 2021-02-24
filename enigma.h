#ifndef ENIGMA
#define ENIGMA

// if sandbox

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define rotor_no(s) #s
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

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
	int *ring;
	int is_step;
} wheels;

typedef struct plugboard
{
	char *alpha;
	int stecker_count;
} plugboard;

typedef struct indices 
{
	int r_index; 
	int is_inverted;
	int is_contact;
} indices;



static inline int config_wheel_order(wheels *_wheels, const char *w_order);
static inline int config_ring_settings(wheels *_wheels, const char *r_settings);
static inline int config_reflector_wiring(wheels *_wheels, const char *reflector, const char *r_wiring);
static inline int config_start_pos_rotors(wheels *_wheels, const char *indicator);
static inline int config_plug_connections(plugboard *_plugboard, const char *p_connections);
static inline int _step(wheels * _wheels);
static inline int _increment_ring(int * c);
static inline int _walk(indices * _indices, int n_rotors, int index);
static inline int _scramble(int * c, int * contact, int int * ring);
static inline int encrypt(wheels *_wheels, plugboard *_plugboard, const char *plaintext);

static inline int _increment_ring(int * c){
	if(*c < 65 || *c > 90) return 0;
	if(++*c > 90) *c -= 26;

	return 1;
}

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
	w_order_cpy[strlen(w_order)] = '\0';

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

	if (index < 4)
		return 0;

	_wheels->_rotors = malloc(--index * sizeof(rotor));
	_wheels->is_config = 0;
	_wheels->n_rotors = index;

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

			for (index = 0; index < _wheels->n_rotors; index++)
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

			for (; index < _wheels->n_rotors; index++)
			{
				if (strcmp(w_tokens[index], ttok) != 0)
					continue;
				rotor r;

				pch = strchr(buf + 1, '/');

				if (pch == NULL)
				{

					reflector_cnt++;
					if (reflector_cnt == 3)
						return 0;

					r.is_reflector = 1;

					r.alpha = malloc(strlen(buf + 1) + 1);
					strcpy(r.alpha, buf + 1);
					r.alpha[strlen(buf + 1)] = '\0';

					r.model_name = malloc(strlen(w_tokens[index]) + 1);
					strcpy(r.model_name, w_tokens[index]);
					r.model_name[strlen(w_tokens[index])] = '\0';

					r.notch = NULL;
				}
				else
				{

					r.is_reflector = 0;

					r.alpha = malloc(26);
					strncpy(r.alpha, buf + 1, 26);
					r.alpha[26] = '\0';

					r.model_name = malloc(strlen(w_tokens[index]) + 1);
					strcpy(r.model_name, w_tokens[index]);
					r.model_name[strlen(w_tokens[index])] = '\0';

					r.notch = malloc(3);
					strcpy(r.notch, buf + 28);
					r.notch[30] = '\0';
				}

				_wheels->_rotors[index] = r;
			}

			model_or_wire = 0;
		}
	}

	if (reflector_cnt == 0)
		return 0;

	_wheels->is_config = 1;
	_wheels->is_step = 0;

	free(w_order_cpy);

	return 1;
}

static inline int config_start_pos_rotors(wheels *_wheels, const char *indicator)
{

	char *pch;
	char *indicator_cpy;
	int index;
	int max_rings;

	max_rings = _wheels->n_rotors > 5 ? 3 : 4;
	_wheels->ring = malloc(sizeof(int) * max_rings);

	indicator_cpy = malloc(strlen(indicator) + 1);
	strcpy(indicator_cpy, indicator);
	indicator_cpy[strlen(indicator)] = '\0';

	pch = strtok(indicator_cpy, " ");

	index = 0;
	while (pch != NULL)
	{
		if (strlen(pch) > 1 || index == max_rings)
			return 0;

		_wheels->ring[index++] = pch[0];
		pch = strtok(NULL, " ");
	}

	free(indicator_cpy);

	return 1;
}

static inline int config_plug_connections(plugboard *_plugboard, const char *p_connections)
{
	char *p_connections_cpy;
	char *pch;
	int index;

	p_connections_cpy = malloc(strlen(p_connections) + 1);
	strcpy(p_connections_cpy, p_connections);
	p_connections_cpy[strlen(p_connections)] = '\0';

	_plugboard->alpha = malloc(27);
	_plugboard->stecker_count = 0;

	index = 64;
	while (++index < 91)
	{
		_plugboard->alpha[(index % 65)] = index;
	}
	_plugboard->alpha[26] = '\0';

	pch = strtok(p_connections_cpy, " ");

	index = 0;
	while (pch != NULL)
	{
		if (strlen(pch) > 2 || _plugboard->stecker_count++ > 13)
			return 0;

		_plugboard->alpha[(pch[0] % 65)] = pch[1];
		_plugboard->alpha[(pch[1] % 65)] = pch[0];

		pch = strtok(NULL, " ");
	}

	free(p_connections_cpy);

	return 1;
}

static inline int config_reflector_wiring(wheels *_wheels, const char *reflector, const char *r_wiring)
{
	char *pch;
	char *pch2[2];
	char *r_wiring_cpy;
	char *rfl_alpha_cpy;

	int n_rotors;
	int index;
	int r_index;
	int f_index;

	n_rotors = _wheels->n_rotors;

	for (index = 0; index < n_rotors; index++)
	{
		if (_wheels->_rotors[index].is_reflector && strcmp(_wheels->_rotors[index].model_name, reflector) == 0)
			break;
	}

	if (index == n_rotors)
		return 0;

	r_wiring_cpy = malloc(strlen(r_wiring) + 1);
	strcpy(r_wiring_cpy, r_wiring);
	r_wiring_cpy[strlen(r_wiring)] = '\0';

	r_index = index;
	pch = strtok(r_wiring_cpy, " ");

	rfl_alpha_cpy = malloc(strlen(_wheels->_rotors[r_index].alpha) + 1);
	strcpy(rfl_alpha_cpy, _wheels->_rotors[r_index].alpha);
	rfl_alpha_cpy[strlen(r_wiring)] = '\0';

	index = 0;
	while (pch != NULL)
	{
		if (strlen(pch) > 2 || index > 13)
			return 0;

		pch2[0] = strchr(rfl_alpha_cpy, pch[0]);
		pch2[1] = strchr(rfl_alpha_cpy, pch[1]);

		rfl_alpha_cpy[(pch2[0] - rfl_alpha_cpy)] = pch[1];
		rfl_alpha_cpy[(pch2[1] - rfl_alpha_cpy)] = pch[0];

		index++;
		pch = strtok(NULL, " ");
	}

	strcpy(_wheels->_rotors[r_index].alpha, rfl_alpha_cpy);
	/// check here

	free(rfl_alpha_cpy);

	return 1;
}

static inline int config_ring_settings(wheels *_wheels, const char *r_settings)
{
	return 1;
}

//arrange rotors

static inline int _step(wheels * _wheels) {
	rotor * rs; 
	int n_rotors;
	int index;
	int * ring;

	if(!_wheels->is_config) return 0;

	rs = _wheels->_rotors;
	n_rotors = _wheels->n_rotors;
	ring = _wheels->ring;

	if(strchr(rs[2].notch, (char)ring[1]) ==  NULL){
		_wheels->is_step = 0;
	}

	if(strchr(rs[2].notch, (char)ring[1]) && !_wheels->is_step){
		_increment_ring(&ring[2]);
		_increment_ring(&ring[1]);
		_increment_ring(&ring[0]);

		_wheels->is_step = 1;
		return 1;
	}

	if(strchr(rs[3].notch, (char)ring[2])){
		_increment_ring(&ring[2]);
		_increment_ring(&ring[1]);
		return 1;
	}

	_increment_ring(&ring[2]);

	return 1; 
}


static inline int _walk(indices * _indices, int n_rotors, int index){

	if( _indices->r_index < 0 || _indices->r_index > n_rotors) 
		return 0;

	if(_indices->r_index == n_rotors - 1) 
		_indices->is_inverted = 1;

	if(index % 2 == 0) 
		_indices->is_contact = ++_indices->is_contact % 2;

	if(_indices->is_inverted) 
		_indices->r_index--;
	else _indices->r_index++;

	return 1;
}

static inline int _scramble(int * c, indices * _indices, int * ring){

	int invert_flg = !(_indices->is_inverted && _indices->is_contact);


	switch (_indices->is_contact)
	{
	case 1:
		if(_indices->is_inverted)
			*c = *c  + (ring[1] - ring[2]);
		else 
			*c = *c  + (ring[2] - ring[3]);
		break;
	case 0:
		if(invert_flg)
			*c = *c - ring[3];
		else 
			*c = *c + ring[];
			
		break;
	
	default:
		break;
	}

	if(*c < 65) 
		*c += 91;
	else if(*c > 90)
		*c %= 91;

	return 1;
}

static inline int encrypt(wheels *_wheels, plugboard *_plugboard, const char *plaintext) {
	
	int n_rotors;
	// int path;
	// int mid_path;
	int route;
	int * ring;
	char * plug_al;
	char * pch;
	char * plaintext_cpy;
	
	rotor * rs;
	plugboard * p;
	indices i;

	i.is_contact = 0;
	i.is_inverted = 0;
	i.r_index = 0;

	rs = _wheels->_rotors;
	p = _plugboard;
	n_rotors = _wheels->n_rotors;

	plaintext_cpy = malloc(strlen(plaintext) + 1);
	strcpy(plaintext_cpy, plaintext);
	plaintext_cpy[strlen(plaintext_cpy)] = '\0';

	ring = _wheels->ring;
	plug_al = _plugboard->alpha;

	int index = 0; 

	// while( index < n_rotors){
	// 	printf("%d r_name %s\n", 3 - index , _wheels->_rotors[index].model_name);
	// 	index++;
	// }

	int max_str = strlen(plaintext_cpy);

	while( index < max_str){
		plaintext_cpy[index] = plug_al[(plaintext_cpy[index] % 65)];
		
		printf("%c\n", plaintext_cpy[index++]);


		route = 0;
		while(route < 7){
			_step(_wheels);	
			// printf("r_index : %d  invert : %d contact : %d\n", i.r_index , i.is_inverted, i.is_contact);	
			_walk(&i, n_rotors, route);

			route++;
		}

	}

	while( index < n_rotors){
		printf("%d r_name %s\n", 3 - index , _wheels->_rotors[index].model_name);
		index++;
	}

	

	return 1;
}

#endif // ENIGMA
