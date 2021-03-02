#ifndef ENIGMA
#define ENIGMA

// if sandbox

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define _ENIGMA_FUNC static inline
#define _ENIGMA_MALLOC(_size) malloc(_size)
#define _ENIGMA_FREE(_ptr) free(_ptr)
#define ENGIMA_STRING(s) s

#define _enigma_char_t char
#define _enigma_strlen strlen
#define _enigma_strcpy strcpy
#define _enigma_strcmp strcmp
#define _enigma_strchr strchr

typedef struct rotor
{
	int is_reflector;
	_enigma_char_t *alpha;
	_enigma_char_t *model_name;
	_enigma_char_t *notch;
} _enigma_rotor;

typedef struct wheels
{
	_enigma_rotor *_rotors;
	size_t n_rotors;
	int is_config;
	int *ring;
	int is_step;
} _enigma_wheels;

typedef struct plugboard
{
	_enigma_char_t *alpha;
	int stecker_count;
} _enigma_plugboard;

typedef struct indices
{
	int index;
	int r_index;
	int is_inverted;
	int is_contact;
	int rotate_inner;
	int rotate_outer;
} _engima_indices;

_ENIGMA_FUNC
int _step(_enigma_wheels *_wheels);
_ENIGMA_FUNC
int _increment_ring(int *c);
_ENIGMA_FUNC
int _index(int c);
_ENIGMA_FUNC
int _walk(_engima_indices *_indices, int n_rotors, int index);
_ENIGMA_FUNC
int _scramble(int *c, _engima_indices *_indices, int *ring, _enigma_rotor *rotors);

_ENIGMA_FUNC
int config_wheel_order(_enigma_wheels *_wheels, const _enigma_char_t *w_order);
_ENIGMA_FUNC
int config_ring_settings(_enigma_wheels *_wheels, const _enigma_char_t *r_settings);
_ENIGMA_FUNC
int config_reflector_wiring(_enigma_wheels *_wheels, const _enigma_char_t *reflector, const _enigma_char_t *r_wiring);
_ENIGMA_FUNC
int config_start_pos_rotors(_enigma_wheels *_wheels, const _enigma_char_t *indicator);
_ENIGMA_FUNC
int config_plug_connections(_enigma_plugboard *_plugboard, const _enigma_char_t *p_connections);
_ENIGMA_FUNC
int encrypt(_enigma_wheels *_wheels, _enigma_plugboard *_plugboard, const _enigma_char_t *plaintext);

_ENIGMA_FUNC
int _increment_ring(int *c)
{
	if (*c < 65 || *c > 90)
		return 0;
	if (++*c > 90)
		*c -= 26;

	return 1;
}

_ENIGMA_FUNC
int config_wheel_order(_enigma_wheels *_wheels, const _enigma_char_t *w_order)
{
	int index = 0;
	int model_or_wire = 0;
	int reflector_cnt = 0;

	_enigma_char_t buf[4096];
	_enigma_char_t *w_tokens[256];
	_enigma_char_t *pch;
	_enigma_char_t *w_order_cpy;

	for (index = 0; index < 256; index++)
		w_tokens[index] = NULL;

	w_order_cpy = _ENIGMA_MALLOC(_enigma_strlen(w_order) + 1);
	_enigma_strcpy(w_order_cpy, w_order);
	w_order_cpy[_enigma_strlen(w_order)] = '\0';

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

	_wheels->_rotors = _ENIGMA_MALLOC(--index * sizeof(_enigma_rotor));
	_wheels->is_config = 0;
	_wheels->n_rotors = index;

	FILE *f = fopen("./ROTORS", "r");

	if (f == NULL)
		return 0;

	while (fgets(buf, 4096, f) != NULL)
	{

		if (_enigma_strlen(buf) > 0 && buf[_enigma_strlen(buf) - 1] == '\n')
			buf[_enigma_strlen(buf) - 1] = '\0';

		if (_enigma_strlen(buf) == 0)
		{
			model_or_wire = 0;
			continue;
		}

		if (model_or_wire == 0)
		{
			for (index = 0; index < _wheels->n_rotors; index++)
			{
				if (_enigma_strcmp(w_tokens[index], buf) == 0)
				{
					model_or_wire = 1;
					break;
				}
			}
		}
		else
		{

			_enigma_char_t *ttok = w_tokens[index];

			for (; index < _wheels->n_rotors; index++)
			{
				if (_enigma_strcmp(w_tokens[index], ttok) != 0)
					continue;
				_enigma_rotor r;

				pch = _enigma_strchr(buf + 1, '/');

				if (pch == NULL)
				{

					reflector_cnt++;
					if (reflector_cnt == 3)
						return 0;

					r.is_reflector = 1;

					r.alpha = _ENIGMA_MALLOC(_enigma_strlen(buf + 1) + 1);
					_enigma_strcpy(r.alpha, buf + 1);
					r.alpha[_enigma_strlen(buf + 1)] = '\0';

					r.model_name = _ENIGMA_MALLOC(_enigma_strlen(w_tokens[index]) + 1);
					_enigma_strcpy(r.model_name, w_tokens[index]);
					r.model_name[_enigma_strlen(w_tokens[index])] = '\0';

					r.notch = NULL;
				}
				else
				{

					r.is_reflector = 0;

					r.alpha = _ENIGMA_MALLOC(26);
					strncpy(r.alpha, buf + 1, 26);
					r.alpha[26] = '\0';

					r.model_name = _ENIGMA_MALLOC(_enigma_strlen(w_tokens[index]) + 1);
					_enigma_strcpy(r.model_name, w_tokens[index]);
					r.model_name[_enigma_strlen(w_tokens[index])] = '\0';

					r.notch = _ENIGMA_MALLOC(3);
					_enigma_strcpy(r.notch, buf + 28);
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

	_ENIGMA_FREE(w_order_cpy);

	return 1;
}

_ENIGMA_FUNC
int config_start_pos_rotors(_enigma_wheels *_wheels, const _enigma_char_t *indicator)
{

	_enigma_char_t *pch;
	_enigma_char_t *indicator_cpy;
	int index;
	int max_rings;

	max_rings = _wheels->n_rotors > 5 ? 3 : 4;
	_wheels->ring = _ENIGMA_MALLOC(sizeof(int) * max_rings);

	indicator_cpy = _ENIGMA_MALLOC(_enigma_strlen(indicator) + 1);
	_enigma_strcpy(indicator_cpy, indicator);
	indicator_cpy[_enigma_strlen(indicator)] = '\0';

	pch = strtok(indicator_cpy, " ");

	index = 0;
	while (pch != NULL)
	{
		if (_enigma_strlen(pch) > 1 || index == max_rings)
			return 0;

		_wheels->ring[index++] = pch[0];
		pch = strtok(NULL, " ");
	}

	_ENIGMA_FREE(indicator_cpy);

	return 1;
}

_ENIGMA_FUNC
int config_plug_connections(_enigma_plugboard *_plugboard, const _enigma_char_t *p_connections)
{
	_enigma_char_t *p_connections_cpy;
	_enigma_char_t *pch;
	int index;

	p_connections_cpy = _ENIGMA_MALLOC(_enigma_strlen(p_connections) + 1);
	_enigma_strcpy(p_connections_cpy, p_connections);
	p_connections_cpy[_enigma_strlen(p_connections)] = '\0';

	_plugboard->alpha = _ENIGMA_MALLOC(27);
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
		if (_enigma_strlen(pch) > 2 || _plugboard->stecker_count++ > 13)
			return 0;

		_plugboard->alpha[(pch[0] % 65)] = pch[1];
		_plugboard->alpha[(pch[1] % 65)] = pch[0];

		pch = strtok(NULL, " ");
	}

	_ENIGMA_FREE(p_connections_cpy);

	return 1;
}

_ENIGMA_FUNC
int config_reflector_wiring(_enigma_wheels *_wheels, const _enigma_char_t *reflector, const _enigma_char_t *r_wiring)
{
	_enigma_char_t *pch;
	_enigma_char_t *pch2[2];
	_enigma_char_t *r_wiring_cpy;
	_enigma_char_t *rfl_alpha_cpy;

	int n_rotors;
	int index;
	int r_index;

	n_rotors = _wheels->n_rotors;

	for (index = 0; index < n_rotors; index++)
	{
		if (_wheels->_rotors[index].is_reflector && _enigma_strcmp(_wheels->_rotors[index].model_name, reflector) == 0)
			break;
	}

	if (index == n_rotors)
		return 0;

	r_wiring_cpy = _ENIGMA_MALLOC(_enigma_strlen(r_wiring) + 1);
	_enigma_strcpy(r_wiring_cpy, r_wiring);
	r_wiring_cpy[_enigma_strlen(r_wiring)] = '\0';

	r_index = index;
	pch = strtok(r_wiring_cpy, " ");

	rfl_alpha_cpy = _ENIGMA_MALLOC(_enigma_strlen(_wheels->_rotors[r_index].alpha) + 1);
	_enigma_strcpy(rfl_alpha_cpy, _wheels->_rotors[r_index].alpha);
	rfl_alpha_cpy[_enigma_strlen(r_wiring)] = '\0';

	index = 0;
	while (pch != NULL)
	{
		if (_enigma_strlen(pch) > 2 || index > 13)
			return 0;

		pch2[0] = _enigma_strchr(rfl_alpha_cpy, pch[0]);
		pch2[1] = _enigma_strchr(rfl_alpha_cpy, pch[1]);

		rfl_alpha_cpy[(pch2[0] - rfl_alpha_cpy)] = pch[1];
		rfl_alpha_cpy[(pch2[1] - rfl_alpha_cpy)] = pch[0];

		index++;
		pch = strtok(NULL, " ");
	}

	_enigma_strcpy(_wheels->_rotors[r_index].alpha, rfl_alpha_cpy);

	_ENIGMA_FREE(rfl_alpha_cpy);

	return 1;
}

_ENIGMA_FUNC
int config_ring_settings(_enigma_wheels *_wheels, const _enigma_char_t *r_settings)
{
	return 1;
}

_ENIGMA_FUNC
int _step(_enigma_wheels *_wheels)
{
	_enigma_rotor *rs;
	int n_rotors;
	int *ring;

	if (!_wheels->is_config)
		return 0;

	rs = _wheels->_rotors;
	n_rotors = _wheels->n_rotors;
	ring = _wheels->ring;

	if (_enigma_strchr(rs[2].notch, (_enigma_char_t)ring[1]) == NULL)
	{
		_wheels->is_step = 0;
	}

	if (_enigma_strchr(rs[2].notch, (_enigma_char_t)ring[1]) && !_wheels->is_step)
	{
		_increment_ring(&ring[2]);
		_increment_ring(&ring[1]);
		_increment_ring(&ring[0]);

		_wheels->is_step = 1;
		return 1;
	}

	if (_enigma_strchr(rs[3].notch, (_enigma_char_t)ring[2]))
	{
		_increment_ring(&ring[2]);
		_increment_ring(&ring[1]);
		return 1;
	}

	_increment_ring(&ring[2]);

	return 1;
}

_ENIGMA_FUNC
int _walk(_engima_indices *_indices, int n_rotors, int index)
{

	if (_indices->r_index < 0 || _indices->r_index > n_rotors)
		return 0;

	if (_indices->r_index == n_rotors - 1)
		_indices->is_inverted = 1;

	if (index % 2 == 0)
	{
		++_indices->is_contact;
		_indices->is_contact %= 2;
	}

	if (_indices->is_inverted)
		_indices->r_index--;
	else
		_indices->r_index++;

	_indices->rotate_inner = !_indices->is_inverted ? _indices->r_index % 2 == 0 : _indices->r_index % 2 == 1;
	_indices->rotate_outer = !(_indices->is_inverted && _indices->is_contact) && (_indices->index % 2 == 1);

	return 1;
}

_ENIGMA_FUNC
int _index(int c)
{
	return c - 65;
}

_ENIGMA_FUNC
int _scramble(int *c, _engima_indices *_indices, int *ring, _enigma_rotor *rotors)
{

	int swap_idxx;

	if (_indices->r_index < 0)
	{
		*c = _index(*c) - _index(ring[2]);
		return 1;
	}

	switch (_indices->is_contact)
	{
	case 1:

		if (_indices->rotate_inner)
		{
			swap_idxx = _indices->is_inverted ? 1 : 2;
			*c = _index(*c) + (_index(ring[swap_idxx & 1]) - _index(ring[(swap_idxx - 1) & 1]));
		}
		else
		{
			swap_idxx = _indices->is_inverted ? 2 : 1;
			*c = _index(*c) + (_index(ring[swap_idxx]) - _index(ring[(swap_idxx | 2) - 1]));
		}
		break;
	case 0:
		swap_idxx = (_indices->r_index <= 0) ? 2 : 0;
		if (_indices->rotate_outer)
			*c = _index(*c) - _index(ring[swap_idxx]);
		else
			*c = _index(*c) + _index(ring[swap_idxx]);

		break;

	default:
		break;
	}

	*c = *c % 26;

	if (*c < 0)
		*c += 26;

	if (_indices->is_inverted)
	{

		_enigma_char_t *e;
		e = _enigma_strchr(rotors[3 - _indices->r_index].alpha, *c + 65);

		*c = (int)(e - rotors[3 - _indices->r_index].alpha);
		*c += 65;
	}
	else
	{
		*c = rotors[3 - _indices->r_index].alpha[*c];
	}

	return 1;
}

_ENIGMA_FUNC
int encrypt(_enigma_wheels *_wheels, _enigma_plugboard *_plugboard, const _enigma_char_t *plaintext)
{

	int n_rotors;
	int route;
	int *ring;
	_enigma_char_t *plug_al;
	_enigma_char_t *pch;
	_enigma_char_t *plaintext_cpy;

	_enigma_rotor *rs;
	_enigma_plugboard *p;
	_engima_indices i;

	i.is_contact = 0;
	i.is_inverted = 0;
	i.r_index = 0;
	i.index = 0;
	i.rotate_inner = 0;
	i.rotate_outer = 0;

	rs = _wheels->_rotors;
	p = _plugboard;
	n_rotors = _wheels->n_rotors;

	plaintext_cpy = _ENIGMA_MALLOC(_enigma_strlen(plaintext) + 1);
	_enigma_strcpy(plaintext_cpy, plaintext);
	plaintext_cpy[_enigma_strlen(plaintext_cpy)] = '\0';

	ring = _wheels->ring;
	plug_al = _plugboard->alpha;

	int index = 0;

	int max_str = _enigma_strlen(plaintext_cpy);

	while (index < max_str)
	{
		plaintext_cpy[index] = plug_al[(plaintext_cpy[index] % 65)];
		int c = plaintext_cpy[index];

		_step(_wheels);

		i.index = 0;
		while (i.index <= 6)
		{
			_scramble(&c, &i, ring, rs);
			_walk(&i, n_rotors, i.index);

			i.index++;
		}

		_scramble(&c, &i, ring, rs);
		printf("--> %c <--", c + 65);

		i.is_contact = 0;
		i.is_inverted = 0;
		i.r_index = 0;
		i.index = 0;
		i.rotate_inner = 0;
		i.rotate_outer = 0;

		index++;
	}

	// index = 0;
	// while (index < 3)
	// {
	// 	printf("%c ", ring[index++]);
	// }

	_ENIGMA_FREE(plaintext_cpy);

	return 1;
}

#endif // ENIGMA
