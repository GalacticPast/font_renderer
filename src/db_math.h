#include "db.h"

typedef union {
    struct
    {
        f32 x;
        f32 y;
    };
    // i can add more things here
    f32 data[2];
} db_vector2;

typedef union {
    struct
    {
        f32 x;
        f32 y;
        f32 z;
    };
    // i can add more things here
    f32 data[3];
} db_vector3;

typedef union {
    struct
    {
        f32 r;
        f32 g;
        f32 b;
        f32 a;
    };
    // i can add more things here
    f32 data[4];
} db_vector4;

db_vector2 db_vector2_add(db_vector2 a, db_vector2 b);
db_vector2 db_vector2_subtract(db_vector2 a, db_vector2 b);
db_vector2 db_vector2_dot_multiplication(db_vector2 a, db_vector2 b);
db_vector2 db_vector2_rotate(db_vector2 a, f32 degrees);

db_vector3 db_vector3_add(db_vector3 a, db_vector3 b);
db_vector3 db_vector3_subtract(db_vector3 a, db_vector3 b);
db_vector3 db_vector3_dot_multiplication(db_vector3 a, db_vector3 b);

#ifdef DB_MATH_IMPLEMENTATION

static inline db_vector2               db_vector2_add(db_vector2 a, db_vector2 b);
static inline db_vector2               db_vector2_subtract(db_vector2 a, db_vector2 b);
static inline db_vector2               db_vector2_dot_multiplication(db_vector2 a, db_vector2 b);
static inline db_vector2               db_vector2_rotate(db_vector2 a, f32 degrees);
static inline static inline db_vector3 db_vector3_add(db_vector3 a, db_vector3 b);
static inline db_vector3               db_vector3_subtract(db_vector3 a, db_vector3 b);
static inline db_vector3               db_vector3_dot_multiplication(db_vector3 a, db_vector3 b);

#endif
