#define COS(X) cos_table[X&(NUMBER_OF_DEGREES-1)]
#define SIN(X) sin_table[X&(NUMBER_OF_DEGREES-1)]

const NUMBER_OF_DEGREES = 4096;
const SHIFT = 16;
const SHIFT_MULT = 1<<SHIFT;

extern long cos_table[4096];
extern long sin_table[4096];
