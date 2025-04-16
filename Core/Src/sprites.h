

#include <stdint.h>


typedef struct {
	const uint16_t *spriteData;
	uint16_t width;
	uint16_t height;
} sprite;

extern sprite Player_ship_flat;
extern sprite Enemy_ship_flat1;
extern sprite Enemy_ship_flat2;
extern sprite Enemy_ship_dying1;
extern sprite Enemy_ship_dying2;

extern const uint16_t player_ship_flat[132];
extern const uint16_t enemy_ship_flat1[64];
extern const uint16_t enemy_ship_flat2[64];
extern const uint16_t enemy_ship_dying1[64];
extern const uint16_t enemy_ship_dying2[64];
