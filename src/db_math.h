#include "db.h"

// warining I copied this from ginger bill. So im sorry if you see this mista bill

#ifndef DB_MATH_INCLUDE_DB_MATH_H
#define DB_MATH_INCLUDE_DB_MATH_H

#include "db.h"
#include <stddef.h>

#if !defined(DB_MATH_NO_MATH_H)
#include <math.h>
#else
#include <intrin.h>
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4201)
#endif

typedef union db_vector2 {
    struct
    {
        f32 x, y;
    };
    f32 data[2];
} db_vector2;

typedef union db_vector3 {
    struct
    {
        f32 x, y, z;
    };
    struct
    {
        f32 r, g, b;
    };

    db_vector2 xy;
    f32        data[3];
} db_vector3;

typedef union db_vector4 {
    struct
    {
        f32 x, y, z, w;
    };
    struct
    {
        f32 r, g, b, a;
    };
    struct
    {
        db_vector2 xy, zw;
    };
    db_vector3 xyz;
    db_vector3 rgb;
    f32        data[4];
} db_vector4;

typedef union db_matrix2 {
    struct
    {
        db_vector2 x, y;
    };
    db_vector2 col[2];
    f32        data[4];
} db_matrix2;

typedef union db_matrix3 {
    struct
    {
        db_vector3 x, y, z;
    };
    db_vector3 col[3];
    f32        data[9];
} db_matrix3;

typedef union db_matrix4 {
    struct
    {
        db_vector4 x, y, z, w;
    };
    db_vector4 col[4];
    f32        data[16];
} db_matrix4;

typedef union db_quaternion {
    struct
    {
        f32 x, y, z, w;
    };
    db_vector4 xyzw;
    db_vector3 xyz;
    f32        data[4];
} db_quaternion;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

typedef f32 db_float2[2];
typedef f32 db_float3[3];
typedef f32 db_float4[4];

typedef struct db_rect2
{
    db_vector2 pos, dim;
} db_rect2;
typedef struct db_rect3
{
    db_vector3 pos, dim;
} db_rect3;

typedef struct db_aabb2
{
    db_vector2 centre, half_size;
} db_aabb2;
typedef struct db_aabb3
{
    db_vector3 centre, half_size;
} db_aabb3;

#if defined(_MSC_VER)
typedef unsigned __int32 db_math_u32;
typedef unsigned __int64 db_math_u64;
#else
#if defined(DB_USE_STDINT)
#include <stdint.h>
typedef uint32_t db_math_u32;
typedef uint64_t db_math_u64;
#else
typedef unsigned int       db_math_u32;
typedef unsigned long long db_math_u64;
#endif
#endif

typedef short db_half;

#ifndef DB_MATH_CONSTANTS
#define DB_MATH_CONSTANTS
#define DB_MATH_EPSILON 1.19209290e-7f
#define DB_MATH_ZERO 0.0f
#define DB_MATH_ONE 1.0f
#define DB_MATH_TWO_THIRDS 0.666666666666666666666666666666666666667f

#define DB_MATH_TAU 6.28318530717958647692528676655900576f
#define DB_MATH_PI 3.14159265358979323846264338327950288f
#define DB_MATH_ONE_OVER_TAU 0.159154943091895335768883763372514362f
#define DB_MATH_ONE_OVER_PI 0.318309886183790671537767526745028724f

#define DB_MATH_TAU_OVER_2 3.14159265358979323846264338327950288f
#define DB_MATH_TAU_OVER_4 1.570796326794896619231321691639751442f
#define DB_MATH_TAU_OVER_8 0.785398163397448309615660845819875721f

#define DB_MATH_E 2.7182818284590452353602874713526625f
#define DB_MATH_SQRT_TWO 1.41421356237309504880168872420969808f
#define DB_MATH_SQRT_THREE 1.73205080756887729352744634150587236f
#define DB_MATH_SQRT_FIVE 2.23606797749978969640917366873127623f

#define DB_MATH_LOG_TWO 0.693147180559945309417232121458176568f
#define DB_MATH_LOG_TEN 2.30258509299404568401799145468436421f
#endif

#if defined(__cplusplus)
extern "C"
{
#endif

#ifndef db_clamp
#define db_clamp(x, lower, upper) (db_min(db_max(x, (lower)), (upper)))
#endif
#ifndef db_clamp01
#define db_clamp01(x) db_clamp(x, 0, 1)
#endif

#ifndef db_square
#define db_square(x) ((x) * (x))
#endif

#ifndef db_cube
#define db_cube(x) ((x) * (x) * (x))
#endif

#ifndef db_abs
#define db_abs(x) ((x) > 0 ? (x) : -(x))
#endif

#ifndef db_sign
#define db_sign(x) ((x) >= 0 ? 1 : -1)
#endif

    static inline f32 db_to_radians(f32 degrees);
    static inline f32 db_to_degrees(f32 radians);

    static inline f32 db_angle_diff(f32 radians_a, f32 radians_b);

#ifndef db_min
#define db_min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef db_max
#define db_max(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef db_min3
#define db_min3(a, b, c) db_min(db_min(a, b), c)
#endif

#ifndef db_max3
#define db_max3(a, b, c) db_max(db_max(a, b), c)
#endif

    static inline f32 db_copy_sign(f32 x, f32 y);
    static inline f32 db_remainder(f32 x, f32 y);
    static inline f32 db_mod(f32 x, f32 y);
    static inline f32 db_sqrt(f32 a);
    static inline f32 db_rsqrt(f32 a);
    static inline f32 db_quake_rsqrt(f32 a);

    static inline f32 db_sin(f32 radians);
    static inline f32 db_cos(f32 radians);
    static inline f32 db_tan(f32 radians);
    static inline f32 db_arcsin(f32 a);
    static inline f32 db_arccos(f32 a);
    static inline f32 db_arctan(f32 a);
    static inline f32 db_arctan2(f32 y, f32 x);

    static inline f32 db_exp(f32 x);
    static inline f32 db_exp2(f32 x);
    static inline f32 db_log(f32 x);
    static inline f32 db_log2(f32 x);
    static inline f32 db_fast_exp(f32 x);
    static inline f32 db_fast_exp2(f32 x);
    static inline f32 db_pow(f32 x, f32 y);

    static inline f32 db_round(f32 x);
    static inline f32 db_floor(f32 x);
    static inline f32 db_ceil(f32 x);

    static inline f32     db_half_to_float(db_half value);
    static inline db_half db_float_to_half(f32 value);

    static inline db_vector2 db_vector2_zero(void);
    static inline db_vector2 db_vector2_make(f32 x, f32 y);
    static inline db_vector2 db_vector2_from_array(f32 x[2]);

    static inline db_vector3 db_vector3_zero(void);
    static inline db_vector3 db_vector3_make(f32 x, f32 y, f32 z);
    static inline db_vector3 db_vector3_from_array(f32 x[3]);

    static inline db_vector4 db_vector4_zero(void);
    static inline db_vector4 db_vector4_make(f32 x, f32 y, f32 z, f32 w);
    static inline db_vector4 db_vector4_from_array(f32 x[4]);

    static inline void db__vector2_add(db_vector2 *d, db_vector2 v0, db_vector2 v1);
    static inline void db__vector2_subtract(db_vector2 *d, db_vector2 v0, db_vector2 v1);
    static inline void db__vector2_multiply(db_vector2 *d, db_vector2 v, f32 s);
    static inline void db__vector2_divide(db_vector2 *d, db_vector2 v, f32 s);

    static inline void db__vector3_add(db_vector3 *d, db_vector3 v0, db_vector3 v1);
    static inline void db__vector3_subtract(db_vector3 *d, db_vector3 v0, db_vector3 v1);
    static inline void db__vector3_multiply(db_vector3 *d, db_vector3 v, f32 s);
    static inline void db__vector3_divide(db_vector3 *d, db_vector3 v, f32 s);

    static inline void db__vector4_add(db_vector4 *d, db_vector4 v0, db_vector4 v1);
    static inline void db__vector4_subtract(db_vector4 *d, db_vector4 v0, db_vector4 v1);
    static inline void db__vector4_multiply(db_vector4 *d, db_vector4 v, f32 s);
    static inline void db__vector4_divide(db_vector4 *d, db_vector4 v, f32 s);

    static inline void db_vector2_addeq(db_vector2 *d, db_vector2 v);
    static inline void db_vector2_subeq(db_vector2 *d, db_vector2 v);
    static inline void db_vector2_muleq(db_vector2 *d, f32 s);
    static inline void db_vector2_diveq(db_vector2 *d, f32 s);

    static inline void db_vector3_addeq(db_vector3 *d, db_vector3 v);
    static inline void db_vector3_subeq(db_vector3 *d, db_vector3 v);
    static inline void db_vector3_muleq(db_vector3 *d, f32 s);
    static inline void db_vector3_diveq(db_vector3 *d, f32 s);

    static inline void db_vector4_addeq(db_vector4 *d, db_vector4 v);
    static inline void db_vector4_subeq(db_vector4 *d, db_vector4 v);
    static inline void db_vector4_muleq(db_vector4 *d, f32 s);
    static inline void db_vector4_diveq(db_vector4 *d, f32 s);

    static inline f32 db_vector2_dot_multiplication(db_vector2 v0, db_vector2 v1);
    static inline f32 db_vector3_dot_multiplication(db_vector3 v0, db_vector3 v1);
    static inline f32 db_vector4_dot_multiplication(db_vector4 v0, db_vector4 v1);

    static inline void db__vector2_cross(f32 *d, db_vector2 v0, db_vector2 v1);
    static inline void db__vector3_cross(db_vector3 *d, db_vector3 v0, db_vector3 v1);

    static inline f32 db_vector2_magnitude_squared(db_vector2 v);
    static inline f32 db_vector3_magnitude_squared(db_vector3 v);
    static inline f32 db_vector4_magnitude_squared(db_vector4 v);

    static inline f32 db_vector2_magnitude(db_vector2 v);
    static inline f32 db_vector3_magnitude(db_vector3 v);
    static inline f32 db_vector4_magnitude(db_vector4 v);

    static inline void db__vector2_normalize(db_vector2 *d, db_vector2 v);
    static inline void db__vector3_normalize(db_vector3 *d, db_vector3 v);
    static inline void db__vector4_normalize(db_vector4 *d, db_vector4 v);

    static inline void db__vector2_normalize_or_zero(db_vector2 *d, db_vector2 v);
    static inline void db__vector3_normalize_or_zero(db_vector3 *d, db_vector3 v);
    static inline void db__vector4_normalize_or_zero(db_vector4 *d, db_vector4 v);

    static inline void db__vector2_reflect(db_vector2 *d, db_vector2 i, db_vector2 n);
    static inline void db__vector3_reflect(db_vector3 *d, db_vector3 i, db_vector3 n);
    static inline void db__vector2_refract(db_vector2 *d, db_vector2 i, db_vector2 n, f32 eta);
    static inline void db__vector3_refract(db_vector3 *d, db_vector3 i, db_vector3 n, f32 eta);

    static inline f32 db_vector2_aspect_ratio(db_vector2 v);

    static inline void db_matrix2_identity(db_matrix2 *m);
    static inline void db_float22_identity(f32 m[2][2]);

    static inline void db_matrix2_transpose(db_matrix2 *m);
    static inline void db_matrix2_mul(db_matrix2 *out, db_matrix2 *m1, db_matrix2 *m2);
    static inline void db_matrix2_mul_vector2(db_vector2 *out, db_matrix2 *m, db_vector2 in);
    static inline void db_matrix2_inverse(db_matrix2 *out, db_matrix2 *in);
    static inline f32  db_matrix2_determinate(db_matrix2 *m);

    static inline db_matrix2 *db_matrix2_v(db_vector2 m[2]);
    static inline db_matrix2 *db_matrix2_f(f32 m[2][2]);
    static inline db_float2  *db_float22_m(db_matrix2 *m);
    static inline db_float2  *db_float22_v(db_vector2 m[2]);
    static inline db_float2  *db_float22_4(f32 m[4]);

    static inline void db_float22_transpose(f32 (*vec)[2]);
    static inline void db_float22_mul(f32 (*out)[2], f32 (*mat1)[2], f32 (*mat2)[2]);
    static inline void db_float22_mul_vector2(db_vector2 *out, f32 m[2][2], db_vector2 in);

    static inline void db_matrix3_identity(db_matrix3 *m);
    static inline void db_float33_identity(f32 m[3][3]);

    static inline void db_matrix3_transpose(db_matrix3 *m);
    static inline void db_matrix3_mul(db_matrix3 *out, db_matrix3 *m1, db_matrix3 *m2);
    static inline void db_matrix3_mul_vector3(db_vector3 *out, db_matrix3 *m, db_vector3 in);
    static inline void db_matrix3_inverse(db_matrix3 *out, db_matrix3 *in);
    static inline f32  db_matrix3_determinate(db_matrix3 *m);

    static inline db_matrix3 *db_matrix3_v(db_vector3 m[3]);
    static inline db_matrix3 *db_matrix3_f(f32 m[3][3]);

    static inline db_float3 *db_float33_m(db_matrix3 *m);
    static inline db_float3 *db_float33_v(db_vector3 m[3]);
    static inline db_float3 *db_float33_9(f32 m[9]);

    static inline void db_float33_transpose(f32 (*vec)[3]);
    static inline void db_float33_mul(f32 (*out)[3], f32 (*mat1)[3], f32 (*mat2)[3]);
    static inline void db_float33_mul_vector3(db_vector3 *out, f32 m[3][3], db_vector3 in);

    static inline void db_matrix4_identity(db_matrix4 *m);
    static inline void db_float44_identity(f32 m[4][4]);

    static inline void db_matrix4_transpose(db_matrix4 *m);
    static inline void db_matrix4_mul(db_matrix4 *out, db_matrix4 *m1, db_matrix4 *m2);
    static inline void db_matrix4_mul_vector4(db_vector4 *out, db_matrix4 *m, db_vector4 in);
    static inline void db_matrix4_inverse(db_matrix4 *out, db_matrix4 *in);

    static inline db_matrix4 *db_matrix4_v(db_vector4 m[4]);
    static inline db_matrix4 *db_matrix4_f(f32 m[4][4]);

    static inline db_float4 *db_float44_m(db_matrix4 *m);
    static inline db_float4 *db_float44_v(db_vector4 m[4]);
    static inline db_float4 *db_float44_16(f32 m[16]);

    static inline void db_float44_transpose(f32 (*vec)[4]);
    static inline void db_float44_mul(f32 (*out)[4], f32 (*mat1)[4], f32 (*mat2)[4]);
    static inline void db_float44_mul_vector4(db_vector4 *out, f32 m[4][4], db_vector4 in);

    static inline void db_matrix4_translate(db_matrix4 *out, db_vector3 v);
    static inline void db_matrix4_rotate(db_matrix4 *out, db_vector3 v, f32 angle_radians);
    static inline void db_matrix4_scale(db_matrix4 *out, db_vector3 v);
    static inline void db_matrix4_scalef(db_matrix4 *out, f32 s);
    static inline void db_matrix4_ortho2d(db_matrix4 *out, f32 left, f32 right, f32 bottom, f32 top);
    static inline void db_matrix4_ortho3d(db_matrix4 *out, f32 left, f32 right, f32 bottom, f32 top, f32 z_near,
                                          f32 z_far);
    static inline void db_matrix4_perspective(db_matrix4 *out, f32 fovy, f32 aspect, f32 z_near, f32 z_far);
    static inline void db_matrix4_infinite_perspective(db_matrix4 *out, f32 fovy, f32 aspect, f32 z_near);

    static inline void db_matrix4_look_at(db_matrix4 *out, db_vector3 eye, db_vector3 centre, db_vector3 up);

    static inline db_quaternion db_quaternion_make(f32 x, f32 y, f32 z, f32 w);
    static inline db_quaternion db_quaternion_from_array(f32 e[4]);
    static inline db_quaternion db_quaternion_axis_angle(db_vector3 axis, f32 angle_radians);
    static inline db_quaternion db_quaternion_euler_angles(f32 pitch, f32 yaw, f32 roll);
    static inline db_quaternion db_quaternion_identity(void);

    static inline void db_quaternion_add(db_quaternion *d, db_quaternion q0, db_quaternion q1);
    static inline void db_quaternion_sub(db_quaternion *d, db_quaternion q0, db_quaternion q1);
    static inline void db_quaternion_mul(db_quaternion *d, db_quaternion q0, db_quaternion q1);
    static inline void db_quaternion_div(db_quaternion *d, db_quaternion q0, db_quaternion q1);

    static inline void db_quaternion_mulf(db_quaternion *d, db_quaternion q, f32 s);
    static inline void db_quaternion_divf(db_quaternion *d, db_quaternion q, f32 s);

    static inline void db_quaternion_addeq(db_quaternion *d, db_quaternion q);
    static inline void db_quaternion_subeq(db_quaternion *d, db_quaternion q);
    static inline void db_quaternion_muleq(db_quaternion *d, db_quaternion q);
    static inline void db_quaternion_diveq(db_quaternion *d, db_quaternion q);

    static inline void db_quaternion_muleqf(db_quaternion *d, f32 s);
    static inline void db_quaternion_diveqf(db_quaternion *d, f32 s);

    static inline f32 db_quaternion_dot(db_quaternion q0, db_quaternion q1);
    static inline f32 db_quaternion_mag(db_quaternion q);

    static inline void db_quaternion_norm(db_quaternion *d, db_quaternion q);
    static inline void db_quaternion_conj(db_quaternion *d, db_quaternion q);
    static inline void db_quaternion_inverse(db_quaternion *d, db_quaternion q);

    static inline void db_quaternion_axis(db_vector3 *axis, db_quaternion q);
    static inline f32  db_quaternion_angle(db_quaternion q);

    static inline f32 db_quaternion_pitch(db_quaternion q);
    static inline f32 db_quaternion_yaw(db_quaternion q);
    static inline f32 db_quaternion_roll(db_quaternion q);

    static inline void db_quaternion_rotate_vector3(db_vector3 *d, db_quaternion q, db_vector3 v);
    static inline void db_matrix4_from_quat(db_matrix4 *out, db_quaternion q);
    static inline void db_quaternion_from_mat4(db_quaternion *out, db_matrix4 *m);

    /* Interpolations */
    static inline f32 db_lerp(f32 a, f32 b, f32 t);
    static inline f32 db_unlerp(f32 t, f32 a, f32 b);
    static inline f32 db_smooth_step(f32 a, f32 b, f32 t);
    static inline f32 db_smoother_step(f32 a, f32 b, f32 t);

    static inline void db__vector2_lerp(db_vector2 *d, db_vector2 a, db_vector2 b, f32 t);
    static inline void db__vector3_lerp(db_vector3 *d, db_vector3 a, db_vector3 b, f32 t);
    static inline void db__vector4_lerp(db_vector4 *d, db_vector4 a, db_vector4 b, f32 t);

    static inline void db_quaternion_lerp(db_quaternion *d, db_quaternion a, db_quaternion b, f32 t);
    static inline void db_quaternion_nlerp(db_quaternion *d, db_quaternion a, db_quaternion b, f32 t);
    static inline void db_quaternion_slerp(db_quaternion *d, db_quaternion a, db_quaternion b, f32 t);
    static inline void db_quaternion_nquad(db_quaternion *d, db_quaternion p, db_quaternion a, db_quaternion b,
                                           db_quaternion q, f32 t);
    static inline void db_quaternion_squad(db_quaternion *d, db_quaternion p, db_quaternion a, db_quaternion b,
                                           db_quaternion q, f32 t);
    static inline void db_quaternion_slerp_approx(db_quaternion *d, db_quaternion a, db_quaternion b, f32 t);
    static inline void db_quaternion_squad_approx(db_quaternion *d, db_quaternion p, db_quaternion a, db_quaternion b,
                                                  db_quaternion q, f32 t);

    /* Rects */
    static inline db_rect2 db_rect2_make(db_vector2 pos, db_vector2 dim);
    static inline db_rect2 db_rect2v(f32 v[4]);

    static inline db_rect3 db_rect3_make(db_vector3 pos, db_vector3 dim);
    static inline db_rect3 db_rect3v(f32 v[6]);

    static inline int db_rect2_contains(db_rect2 a, f32 x, f32 y);
    static inline int db_rect2_contains_vector2(db_rect2 a, db_vector2 p);
    static inline int db_rect2_intersects(db_rect2 a, db_rect2 b);
    static inline int db_rect2_intersection_result(db_rect2 a, db_rect2 b, db_rect2 *intersection);

#ifndef DB_MURMUR64_DEFAULT_SEED
#define DB_MURMUR64_DEFAULT_SEED 0x9747b28c
#endif
    /* Hashing */
    static inline db_math_u64 db_hash_murmur64(void const *key, size_t num_bytes, db_math_u64 seed);

    /* Random */
    static inline f32 db_random_range_float(f32 min_inc, f32 max_inc);
    static inline int db_random_range_int(int min_inc, int max_inc);
    static inline f32 db_random01(void);

    /* Value-returning vector operations. db__ helpers are private. */
    static inline db_vector2 db_vector2_add(db_vector2 v0, db_vector2 v1);
    static inline db_vector2 db_vector2_subtract(db_vector2 v0, db_vector2 v1);
    static inline db_vector2 db_vector2_multiply(db_vector2 v, f32 s);
    static inline db_vector2 db_vector2_divide(db_vector2 v, f32 s);
    static inline db_vector3 db_vector3_add(db_vector3 v0, db_vector3 v1);
    static inline db_vector3 db_vector3_subtract(db_vector3 v0, db_vector3 v1);
    static inline db_vector3 db_vector3_multiply(db_vector3 v, f32 s);
    static inline db_vector3 db_vector3_divide(db_vector3 v, f32 s);
    static inline db_vector4 db_vector4_add(db_vector4 v0, db_vector4 v1);
    static inline db_vector4 db_vector4_subtract(db_vector4 v0, db_vector4 v1);
    static inline db_vector4 db_vector4_multiply(db_vector4 v, f32 s);
    static inline db_vector4 db_vector4_divide(db_vector4 v, f32 s);
    static inline f32        db_vector2_cross(db_vector2 v0, db_vector2 v1);
    static inline db_vector3 db_vector3_cross(db_vector3 v0, db_vector3 v1);
    static inline db_vector2 db_vector2_normalize(db_vector2 v);
    static inline db_vector3 db_vector3_normalize(db_vector3 v);
    static inline db_vector4 db_vector4_normalize(db_vector4 v);
    static inline db_vector2 db_vector2_normalize_or_zero(db_vector2 v);
    static inline db_vector3 db_vector3_normalize_or_zero(db_vector3 v);
    static inline db_vector4 db_vector4_normalize_or_zero(db_vector4 v);
    static inline db_vector2 db_vector2_reflect(db_vector2 i, db_vector2 n);
    static inline db_vector3 db_vector3_reflect(db_vector3 i, db_vector3 n);
    static inline db_vector2 db_vector2_refract(db_vector2 i, db_vector2 n, f32 eta);
    static inline db_vector3 db_vector3_refract(db_vector3 i, db_vector3 n, f32 eta);
    static inline db_vector2 db_vector2_lerp(db_vector2 a, db_vector2 b, f32 t);
    static inline db_vector3 db_vector3_lerp(db_vector3 a, db_vector3 b, f32 t);
    static inline db_vector4 db_vector4_lerp(db_vector4 a, db_vector4 b, f32 t);
    static inline db_vector2 db_vector2_hadamard(db_vector2 a, db_vector2 b);
    static inline db_vector3 db_vector3_hadamard(db_vector3 a, db_vector3 b);
    static inline db_vector4 db_vector4_hadamard(db_vector4 a, db_vector4 b);
    static inline db_vector2 db_vector2_rotate(db_vector2 a, f32 degrees);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

inline bool operator==(db_vector2 a, db_vector2 b)
{
    return (a.x == b.x) && (a.y == b.y);
}
inline bool operator!=(db_vector2 a, db_vector2 b)
{
    return !operator==(a, b);
}

inline db_vector2 operator+(db_vector2 a)
{
    return a;
}
inline db_vector2 operator-(db_vector2 a)
{
    db_vector2 r = {-a.x, -a.y};
    return r;
}

inline db_vector2 operator+(db_vector2 a, db_vector2 b)
{
    db_vector2 r;
    db__vector2_add(&r, a, b);
    return r;
}
inline db_vector2 operator-(db_vector2 a, db_vector2 b)
{
    db_vector2 r;
    db__vector2_subtract(&r, a, b);
    return r;
}

inline db_vector2 operator*(db_vector2 a, f32 scalar)
{
    db_vector2 r;
    db__vector2_multiply(&r, a, scalar);
    return r;
}
inline db_vector2 operator*(f32 scalar, db_vector2 a)
{
    return operator*(a, scalar);
}

inline db_vector2 operator/(db_vector2 a, f32 scalar)
{
    return operator*(a, 1.0f / scalar);
}

/* Hadamard Product */
inline db_vector2 operator*(db_vector2 a, db_vector2 b)
{
    db_vector2 r = {a.x * b.x, a.y * b.y};
    return r;
}
inline db_vector2 operator/(db_vector2 a, db_vector2 b)
{
    db_vector2 r = {a.x / b.x, a.y / b.y};
    return r;
}

inline db_vector2 &operator+=(db_vector2 &a, db_vector2 b)
{
    return (a = a + b);
}
inline db_vector2 &operator-=(db_vector2 &a, db_vector2 b)
{
    return (a = a - b);
}
inline db_vector2 &operator*=(db_vector2 &a, f32 scalar)
{
    return (a = a * scalar);
}
inline db_vector2 &operator/=(db_vector2 &a, f32 scalar)
{
    return (a = a / scalar);
}

inline bool operator==(db_vector3 a, db_vector3 b)
{
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}
inline bool operator!=(db_vector3 a, db_vector3 b)
{
    return !operator==(a, b);
}

inline db_vector3 operator+(db_vector3 a)
{
    return a;
}
inline db_vector3 operator-(db_vector3 a)
{
    db_vector3 r = {-a.x, -a.y, -a.z};
    return r;
}

inline db_vector3 operator+(db_vector3 a, db_vector3 b)
{
    db_vector3 r;
    db__vector3_add(&r, a, b);
    return r;
}
inline db_vector3 operator-(db_vector3 a, db_vector3 b)
{
    db_vector3 r;
    db__vector3_subtract(&r, a, b);
    return r;
}

inline db_vector3 operator*(db_vector3 a, f32 scalar)
{
    db_vector3 r;
    db__vector3_multiply(&r, a, scalar);
    return r;
}
inline db_vector3 operator*(f32 scalar, db_vector3 a)
{
    return operator*(a, scalar);
}

inline db_vector3 operator/(db_vector3 a, f32 scalar)
{
    return operator*(a, 1.0f / scalar);
}

/* Hadamard Product */
inline db_vector3 operator*(db_vector3 a, db_vector3 b)
{
    db_vector3 r = {a.x * b.x, a.y * b.y, a.z * b.z};
    return r;
}
inline db_vector3 operator/(db_vector3 a, db_vector3 b)
{
    db_vector3 r = {a.x / b.x, a.y / b.y, a.z / b.z};
    return r;
}

inline db_vector3 &operator+=(db_vector3 &a, db_vector3 b)
{
    return (a = a + b);
}
inline db_vector3 &operator-=(db_vector3 &a, db_vector3 b)
{
    return (a = a - b);
}
inline db_vector3 &operator*=(db_vector3 &a, f32 scalar)
{
    return (a = a * scalar);
}
inline db_vector3 &operator/=(db_vector3 &a, f32 scalar)
{
    return (a = a / scalar);
}

inline bool operator==(db_vector4 a, db_vector4 b)
{
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}
inline bool operator!=(db_vector4 a, db_vector4 b)
{
    return !operator==(a, b);
}

inline db_vector4 operator+(db_vector4 a)
{
    return a;
}
inline db_vector4 operator-(db_vector4 a)
{
    db_vector4 r = {-a.x, -a.y, -a.z, -a.w};
    return r;
}

inline db_vector4 operator+(db_vector4 a, db_vector4 b)
{
    db_vector4 r;
    db__vector4_add(&r, a, b);
    return r;
}
inline db_vector4 operator-(db_vector4 a, db_vector4 b)
{
    db_vector4 r;
    db__vector4_subtract(&r, a, b);
    return r;
}

inline db_vector4 operator*(db_vector4 a, f32 scalar)
{
    db_vector4 r;
    db__vector4_multiply(&r, a, scalar);
    return r;
}
inline db_vector4 operator*(f32 scalar, db_vector4 a)
{
    return operator*(a, scalar);
}

inline db_vector4 operator/(db_vector4 a, f32 scalar)
{
    return operator*(a, 1.0f / scalar);
}

/* Hadamard Product */
inline db_vector4 operator*(db_vector4 a, db_vector4 b)
{
    db_vector4 r = {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
    return r;
}
inline db_vector4 operator/(db_vector4 a, db_vector4 b)
{
    db_vector4 r = {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
    return r;
}

inline db_vector4 &operator+=(db_vector4 &a, db_vector4 b)
{
    return (a = a + b);
}
inline db_vector4 &operator-=(db_vector4 &a, db_vector4 b)
{
    return (a = a - b);
}
inline db_vector4 &operator*=(db_vector4 &a, f32 scalar)
{
    return (a = a * scalar);
}
inline db_vector4 &operator/=(db_vector4 &a, f32 scalar)
{
    return (a = a / scalar);
}

inline db_matrix2 operator+(db_matrix2 const &a, db_matrix2 const &b)
{
    int        i, j;
    db_matrix2 r = {0};
    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 2; i++)
            r.data[2 * j + i] = a.data[2 * j + i] + b.data[2 * j + i];
    }
    return r;
}

inline db_matrix2 operator-(db_matrix2 const &a, db_matrix2 const &b)
{
    int        i, j;
    db_matrix2 r = {0};
    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 2; i++)
            r.data[2 * j + i] = a.data[2 * j + i] - b.data[2 * j + i];
    }
    return r;
}

inline db_matrix2 operator*(db_matrix2 const &a, db_matrix2 const &b)
{
    db_matrix2 r;
    db_matrix2_mul(&r, (db_matrix2 *)&a, (db_matrix2 *)&b);
    return r;
}
inline db_vector2 operator*(db_matrix2 const &a, db_vector2 v)
{
    db_vector2 r;
    db_matrix2_mul_vector2(&r, (db_matrix2 *)&a, v);
    return r;
}
inline db_matrix2 operator*(db_matrix2 const &a, f32 scalar)
{
    db_matrix2 r = {0};
    int        i;
    for (i = 0; i < 2 * 2; i++)
        r.data[i] = a.data[i] * scalar;
    return r;
}
inline db_matrix2 operator*(f32 scalar, db_matrix2 const &a)
{
    return operator*(a, scalar);
}
inline db_matrix2 operator/(db_matrix2 const &a, f32 scalar)
{
    return operator*(a, 1.0f / scalar);
}

inline db_matrix2 &operator+=(db_matrix2 &a, db_matrix2 const &b)
{
    return (a = a + b);
}
inline db_matrix2 &operator-=(db_matrix2 &a, db_matrix2 const &b)
{
    return (a = a - b);
}
inline db_matrix2 &operator*=(db_matrix2 &a, db_matrix2 const &b)
{
    return (a = a * b);
}

inline db_matrix3 operator+(db_matrix3 const &a, db_matrix3 const &b)
{
    int        i, j;
    db_matrix3 r = {0};
    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < 3; i++)
            r.data[3 * j + i] = a.data[3 * j + i] + b.data[3 * j + i];
    }
    return r;
}

inline db_matrix3 operator-(db_matrix3 const &a, db_matrix3 const &b)
{
    int        i, j;
    db_matrix3 r = {0};
    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < 3; i++)
            r.data[3 * j + i] = a.data[3 * j + i] - b.data[3 * j + i];
    }
    return r;
}

inline db_matrix3 operator*(db_matrix3 const &a, db_matrix3 const &b)
{
    db_matrix3 r;
    db_matrix3_mul(&r, (db_matrix3 *)&a, (db_matrix3 *)&b);
    return r;
}
inline db_vector3 operator*(db_matrix3 const &a, db_vector3 v)
{
    db_vector3 r;
    db_matrix3_mul_vector3(&r, (db_matrix3 *)&a, v);
    return r;
}
inline db_matrix3 operator*(db_matrix3 const &a, f32 scalar)
{
    db_matrix3 r = {0};
    int        i;
    for (i = 0; i < 3 * 3; i++)
        r.data[i] = a.data[i] * scalar;
    return r;
}
inline db_matrix3 operator*(f32 scalar, db_matrix3 const &a)
{
    return operator*(a, scalar);
}
inline db_matrix3 operator/(db_matrix3 const &a, f32 scalar)
{
    return operator*(a, 1.0f / scalar);
}

inline db_matrix3 &operator+=(db_matrix3 &a, db_matrix3 const &b)
{
    return (a = a + b);
}
inline db_matrix3 &operator-=(db_matrix3 &a, db_matrix3 const &b)
{
    return (a = a - b);
}
inline db_matrix3 &operator*=(db_matrix3 &a, db_matrix3 const &b)
{
    return (a = a * b);
}

inline db_matrix4 operator+(db_matrix4 const &a, db_matrix4 const &b)
{
    int        i, j;
    db_matrix4 r = {0};
    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 4; i++)
            r.data[4 * j + i] = a.data[4 * j + i] + b.data[4 * j + i];
    }
    return r;
}

inline db_matrix4 operator-(db_matrix4 const &a, db_matrix4 const &b)
{
    int        i, j;
    db_matrix4 r = {0};
    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 4; i++)
            r.data[4 * j + i] = a.data[4 * j + i] - b.data[4 * j + i];
    }
    return r;
}

inline db_matrix4 operator*(db_matrix4 const &a, db_matrix4 const &b)
{
    db_matrix4 r;
    db_matrix4_mul(&r, (db_matrix4 *)&a, (db_matrix4 *)&b);
    return r;
}
inline db_vector4 operator*(db_matrix4 const &a, db_vector4 v)
{
    db_vector4 r;
    db_matrix4_mul_vector4(&r, (db_matrix4 *)&a, v);
    return r;
}
inline db_matrix4 operator*(db_matrix4 const &a, f32 scalar)
{
    db_matrix4 r = {0};
    int        i;
    for (i = 0; i < 4 * 4; i++)
        r.data[i] = a.data[i] * scalar;
    return r;
}
inline db_matrix4 operator*(f32 scalar, db_matrix4 const &a)
{
    return operator*(a, scalar);
}
inline db_matrix4 operator/(db_matrix4 const &a, f32 scalar)
{
    return operator*(a, 1.0f / scalar);
}

inline db_matrix4 &operator+=(db_matrix4 &a, db_matrix4 const &b)
{
    return (a = a + b);
}
inline db_matrix4 &operator-=(db_matrix4 &a, db_matrix4 const &b)
{
    return (a = a - b);
}
inline db_matrix4 &operator*=(db_matrix4 &a, db_matrix4 const &b)
{
    return (a = a * b);
}

inline bool operator==(db_quaternion a, db_quaternion b)
{
    return a.xyzw == b.xyzw;
}
inline bool operator!=(db_quaternion a, db_quaternion b)
{
    return !operator==(a, b);
}

inline db_quaternion operator+(db_quaternion q)
{
    return q;
}
inline db_quaternion operator-(db_quaternion q)
{
    return db_quaternion_make(-q.x, -q.y, -q.z, -q.w);
}

inline db_quaternion operator+(db_quaternion a, db_quaternion b)
{
    db_quaternion r;
    db_quaternion_add(&r, a, b);
    return r;
}
inline db_quaternion operator-(db_quaternion a, db_quaternion b)
{
    db_quaternion r;
    db_quaternion_sub(&r, a, b);
    return r;
}

inline db_quaternion operator*(db_quaternion a, db_quaternion b)
{
    db_quaternion r;
    db_quaternion_mul(&r, a, b);
    return r;
}
inline db_quaternion operator*(db_quaternion q, f32 s)
{
    db_quaternion r;
    db_quaternion_mulf(&r, q, s);
    return r;
}
inline db_quaternion operator*(f32 s, db_quaternion q)
{
    return operator*(q, s);
}
inline db_quaternion operator/(db_quaternion q, f32 s)
{
    db_quaternion r;
    db_quaternion_divf(&r, q, s);
    return r;
}

inline db_quaternion &operator+=(db_quaternion &a, db_quaternion b)
{
    db_quaternion_addeq(&a, b);
    return a;
}
inline db_quaternion &operator-=(db_quaternion &a, db_quaternion b)
{
    db_quaternion_subeq(&a, b);
    return a;
}
inline db_quaternion &operator*=(db_quaternion &a, db_quaternion b)
{
    db_quaternion_muleq(&a, b);
    return a;
}
inline db_quaternion &operator/=(db_quaternion &a, db_quaternion b)
{
    db_quaternion_diveq(&a, b);
    return a;
}

inline db_quaternion &operator*=(db_quaternion &a, f32 b)
{
    db_quaternion_muleqf(&a, b);
    return a;
}
inline db_quaternion &operator/=(db_quaternion &a, f32 b)
{
    db_quaternion_diveqf(&a, b);
    return a;
}

/* Rotate v by a */
inline db_vector3 operator*(db_quaternion q, db_vector3 v)
{
    db_vector3 r;
    db_quaternion_rotate_vector3(&r, q, v);
    return r;
}

#endif

#endif /* DB_MATH_INCLUDE_DB_MATH_H */

/****************************************************************
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * Implementation
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 ****************************************************************/

#if defined(DB_MATH_IMPLEMENTATION) && !defined(DB_MATH_IMPLEMENTATION_DONE)
#define DB_MATH_IMPLEMENTATION_DONE

#if (defined(__GCC__) || defined(__GNUC__)) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#pragma GCC diagnostic ignored "-Wmissing-braces"
#elif __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wattributes"
#pragma clang diagnostic ignored "-Wmissing-braces"
#endif

static void db__memcpy_4byte(void *dest, void const *src, size_t size)
{
    size_t        i;
    unsigned int *d, *s;
    d = (unsigned int *)dest;
    s = (unsigned int *)src;
    for (i = 0; i < size / 4; i++)
    {
        *d++ = *s++;
    }
}

static inline f32 db_to_radians(f32 degrees)
{
    return degrees * DB_MATH_TAU / 360.0f;
}
static inline f32 db_to_degrees(f32 radians)
{
    return radians * 360.0f / DB_MATH_TAU;
}

static inline f32 db_angle_diff(f32 radians_a, f32 radians_b)
{
    f32 delta  = db_mod(radians_b - radians_a, DB_MATH_TAU);
    delta      = db_mod(delta + 1.5f * DB_MATH_TAU, DB_MATH_TAU);
    delta     -= 0.5f * DB_MATH_TAU;
    return delta;
}

static inline f32 db_copy_sign(f32 x, f32 y)
{
    int ix, iy;
    ix = *(int *)&x;
    iy = *(int *)&y;

    ix &= 0x7fffffff;
    ix |= iy & 0x80000000;
    return *(f32 *)&ix;
}

static inline f32 db_remainder(f32 x, f32 y)
{
    return x - (db_round(x / y) * y);
}

static inline f32 db_mod(f32 x, f32 y)
{
    f32 result;
    y      = db_abs(y);
    result = db_remainder(db_abs(x), y);
    if (db_sign(result))
        result += y;
    return db_copy_sign(result, x);
}

static inline f32 db_quake_rsqrt(f32 a)
{
    union {
        int i;
        f32 f;
    } t;
    f32       x2;
    f32 const three_halfs = 1.5f;

    x2  = a * 0.5f;
    t.f = a;
    t.i = 0x5f375a86 - (t.i >> 1);                /* What the fuck? */
    t.f = t.f * (three_halfs - (x2 * t.f * t.f)); /* 1st iteration */
    t.f = t.f * (three_halfs - (x2 * t.f * t.f)); /* 2nd iteration, this can be removed */

    return t.f;
}

#if defined(DB_MATH_NO_MATH_H)
#if defined(_MSC_VER)

static inline f32 db_rsqrt(f32 a)
{
    return _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(a)));
}
static inline f32 db_sqrt(f32 a)
{
    return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(a)));
};
static inline f32 db_sin(f32 a)
{
    static f32 const a0 = +1.91059300966915117e-31f;
    static f32 const a1 = +1.00086760103908896f;
    static f32 const a2 = -1.21276126894734565e-2f;
    static f32 const a3 = -1.38078780785773762e-1f;
    static f32 const a4 = -2.67353392911981221e-2f;
    static f32 const a5 = +2.08026600266304389e-2f;
    static f32 const a6 = -3.03996055049204407e-3f;
    static f32 const a7 = +1.38235642404333740e-4f;
    return a0 + a * (a1 + a * (a2 + a * (a3 + a * (a4 + a * (a5 + a * (a6 + a * a7))))));
}
static inline f32 db_cos(f32 a)
{
    static f32 const a0 = +1.00238601909309722f;
    static f32 const a1 = -3.81919947353040024e-2f;
    static f32 const a2 = -3.94382342128062756e-1f;
    static f32 const a3 = -1.18134036025221444e-1f;
    static f32 const a4 = +1.07123798512170878e-1f;
    static f32 const a5 = -1.86637164165180873e-2f;
    static f32 const a6 = +9.90140908664079833e-4f;
    static f32 const a7 = -5.23022132118824778e-14f;
    return a0 + a * (a1 + a * (a2 + a * (a3 + a * (a4 + a * (a5 + a * (a6 + a * a7))))));
}

static inline f32 db_tan(f32 radians)
{
    f32 rr  = radians * radians;
    f32 a   = 9.5168091e-03f;
    a      *= rr;
    a      += 2.900525e-03f;
    a      *= rr;
    a      += 2.45650893e-02f;
    a      *= rr;
    a      += 5.33740603e-02f;
    a      *= rr;
    a      += 1.333923995e-01f;
    a      *= rr;
    a      += 3.333314036e-01f;
    a      *= rr;
    a      += 1.0f;
    a      *= radians;
    return a;
}

static inline f32 db_arcsin(f32 a)
{
    return db_arctan2(a, db_sqrt((1.0f + a) * (1.0f - a)));
}
static inline f32 db_arccos(f32 a)
{
    return db_arctan2(db_sqrt((1.0f + a) * (1.0 - a)), a);
}

static inline f32 db_arctan(f32 a)
{
    f32 u  = a * a;
    f32 u2 = u * u;
    f32 u3 = u2 * u;
    f32 u4 = u3 * u;
    f32 f  = 1.0f + 0.33288950512027f * u - 0.08467922817644f * u2 + 0.03252232640125f * u3 - 0.00749305860992f * u4;
    return a / f;
}

static inline f32 db_arctan2(f32 y, f32 x)
{
    if (db_abs(x) > db_abs(y))
    {
        f32 a = db_arctan(y / x);
        if (x > 0.0f)
            return a;
        else
            return y > 0.0f ? a + DB_MATH_TAU_OVER_2 : a - DB_MATH_TAU_OVER_2;
    }
    else
    {
        f32 a = db_arctan(x / y);
        if (x > 0.0f)
            return y > 0.0f ? DB_MATH_TAU_OVER_4 - a : -DB_MATH_TAU_OVER_4 - a;
        else
            return y > 0.0f ? DB_MATH_TAU_OVER_4 + a : -DB_MATH_TAU_OVER_4 + a;
    }
}

static inline f32 db_exp(f32 a)
{
    union {
        f32 f;
        int i;
    } u, v;
    u.i = (int)(6051102 * a + 1056478197);
    v.i = (int)(1056478197 - 6051102 * a);
    return u.f / v.f;
}

static inline f32 db_log(f32 a)
{
    union {
        f32 f;
        int i;
    } u = {a};
    return (u.i - 1064866805) * 8.262958405176314e-8f; /* 1 / 12102203.0; */
}

static inline f32 db_pow(f32 a, f32 b)
{
    int flipped = 0, e;
    f32 f, r = 1.0f;
    if (b < 0)
    {
        flipped = 1;
        b       = -b;
    }

    e = (int)b;
    f = db_exp(b - e);

    while (e)
    {
        if (e & 1)
            r *= a;
        a  *= a;
        e >>= 1;
    }

    r *= f;
    return flipped ? 1.0f / r : r;
}

#else

static inline f32 db_rsqrt(f32 a)
{
    return 1.0f / __builtin_sqrt(a);
}
static inline f32 db_sqrt(f32 a)
{
    return __builtin_sqrt(a);
}
static inline f32 db_sin(f32 radians)
{
    return __builtin_sinf(radians);
}
static inline f32 db_cos(f32 radians)
{
    return __builtin_cosf(radians);
}
static inline f32 db_tan(f32 radians)
{
    return __builtin_tanf(radians);
}
static inline f32 db_arcsin(f32 a)
{
    return __builtin_asinf(a);
}
static inline f32 db_arccos(f32 a)
{
    return __builtin_acosf(a);
}
static inline f32 db_arctan(f32 a)
{
    return __builtin_atanf(a);
}
static inline f32 db_arctan2(f32 y, f32 x)
{
    return __builtin_atan2f(y, x);
}

static inline f32 db_exp(f32 x)
{
    return __builtin_expf(x);
}
static inline f32 db_log(f32 x)
{
    return __builtin_logf(x);
}

static inline f32 db_pow(f32 x, f32 y)
{
    return __builtin_powf(x, y);
}

#endif

#else
static inline f32 db_rsqrt(f32 a)
{
    return 1.0f / sqrtf(a);
}
static inline f32 db_sqrt(f32 a)
{
    return sqrtf(a);
};
static inline f32 db_sin(f32 radians)
{
    return sinf(radians);
};
static inline f32 db_cos(f32 radians)
{
    return cosf(radians);
};
static inline f32 db_tan(f32 radians)
{
    return tanf(radians);
};
static inline f32 db_arcsin(f32 a)
{
    return asinf(a);
};
static inline f32 db_arccos(f32 a)
{
    return acosf(a);
};
static inline f32 db_arctan(f32 a)
{
    return atanf(a);
};
static inline f32 db_arctan2(f32 y, f32 x)
{
    return atan2f(y, x);
};

static inline f32 db_exp(f32 x)
{
    return expf(x);
}
static inline f32 db_log(f32 x)
{
    return logf(x);
}
static inline f32 db_pow(f32 x, f32 y)
{
    return powf(x, y);
}
#endif

static inline f32 db_exp2(f32 x)
{
    return db_exp(DB_MATH_LOG_TWO * x);
}
static inline f32 db_log2(f32 x)
{
    return db_log(x) / DB_MATH_LOG_TWO;
}

static inline f32 db_fast_exp(f32 x)
{
    f32 e = 1.0f + x * (1.0f + x * 0.5f * (1.0f + x * 0.3333333333f * (1.0f + x * 0.25f * (1.0f + x * 0.2f))));
    return e;
}

static inline f32 db_fast_exp2(f32 x)
{
    return db_fast_exp(DB_MATH_LOG_TWO * x);
}

static inline f32 db_round(f32 x)
{
    return (f32)((x >= 0.0f) ? db_floor(x + 0.5f) : db_ceil(x - 0.5f));
}
static inline f32 db_floor(f32 x)
{
    return (f32)((x >= 0.0f) ? (int)x : (int)(x - 0.9999999999999999f));
}
static inline f32 db_ceil(f32 x)
{
    return (f32)((x < 0) ? (int)x : ((int)x) + 1);
}

static inline f32 db_half_to_float(db_half value)
{
    union {
        unsigned int i;
        f32          f;
    } result;
    int s = (value >> 15) & 0x001;
    int e = (value >> 10) & 0x01f;
    int m = value & 0x3ff;

    if (e == 0)
    {
        if (m == 0)
        {
            /* Plus or minus zero */
            result.i = (unsigned int)(s << 31);
            return result.f;
        }
        else
        {
            /* Denormalized number */
            while (!(m & 0x00000400))
            {
                m <<= 1;
                e  -= 1;
            }

            e += 1;
            m &= ~0x00000400;
        }
    }
    else if (e == 31)
    {
        if (m == 0)
        {
            /* Positive or negative infinity */
            result.i = (unsigned int)((s << 31) | 0x7f800000);
            return result.f;
        }
        else
        {
            /* Nan */
            result.i = (unsigned int)((s << 31) | 0x7f800000 | (m << 13));
            return result.f;
        }
    }

    e = e + (127 - 15);
    m = m << 13;

    result.i = (unsigned int)((s << 31) | (e << 23) | m);
    return result.f;
}

static inline db_half db_float_to_half(f32 value)
{
    union {
        unsigned int i;
        f32          f;
    } v;
    int i, s, e, m;

    v.f = value;
    i   = (int)v.i;

    s = (i >> 16) & 0x00008000;
    e = ((i >> 23) & 0x000000ff) - (127 - 15);
    m = i & 0x007fffff;

    if (e <= 0)
    {
        if (e < -10)
            return (db_half)s;
        m = (m | 0x00800000) >> (1 - e);

        if (m & 0x00001000)
            m += 0x00002000;

        return (db_half)(s | (m >> 13));
    }
    else if (e == 0xff - (127 - 15))
    {
        if (m == 0)
        {
            return (db_half)(s | 0x7c00);
        }
        else
        {
            m >>= 13;
            return (db_half)(s | 0x7c00 | m | (m == 0));
        }
    }
    else
    {
        if (m & 0x00001000)
        {
            m += 0x00002000;
            if (m & 0x00800000)
            {
                m  = 0;
                e += 1;
            }
        }

        if (e > 30)
        {
            f32 volatile f = 1e12f;
            int j;
            for (j = 0; j < 10; j++)
                f *= f;

            return (db_half)(s | 0x7c00);
        }

        return (db_half)(s | (e << 10) | (m >> 13));
    }
}

#define DB_VEC2_2OP(a, c, post)                                                                                        \
    a->x = c.x post;                                                                                                   \
    a->y = c.y post;

#define DB_VEC2_3OP(a, b, op, c, post)                                                                                 \
    a->x = b.x op c.x post;                                                                                            \
    a->y = b.y op c.y post;

#define DB_VEC3_2OP(a, c, post)                                                                                        \
    a->x = c.x post;                                                                                                   \
    a->y = c.y post;                                                                                                   \
    a->z = c.z post;

#define DB_VEC3_3OP(a, b, op, c, post)                                                                                 \
    a->x = b.x op c.x post;                                                                                            \
    a->y = b.y op c.y post;                                                                                            \
    a->z = b.z op c.z post;

#define DB_VEC4_2OP(a, c, post)                                                                                        \
    a->x = c.x post;                                                                                                   \
    a->y = c.y post;                                                                                                   \
    a->z = c.z post;                                                                                                   \
    a->w = c.w post;

#define DB_VEC4_3OP(a, b, op, c, post)                                                                                 \
    a->x = b.x op c.x post;                                                                                            \
    a->y = b.y op c.y post;                                                                                            \
    a->z = b.z op c.z post;                                                                                            \
    a->w = b.w op c.w post;

static inline db_vector2 db_vector2_zero(void)
{
    db_vector2 v = {0, 0};
    return v;
}
static inline db_vector2 db_vector2_make(f32 x, f32 y)
{
    db_vector2 v;
    v.x = x;
    v.y = y;
    return v;
}
static inline db_vector2 db_vector2_from_array(f32 x[2])
{
    db_vector2 v;
    v.x = x[0];
    v.y = x[1];
    return v;
}

static inline db_vector3 db_vector3_zero(void)
{
    db_vector3 v = {0, 0, 0};
    return v;
}
static inline db_vector3 db_vector3_make(f32 x, f32 y, f32 z)
{
    db_vector3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
static inline db_vector3 db_vector3_from_array(f32 x[3])
{
    db_vector3 v;
    v.x = x[0];
    v.y = x[1];
    v.z = x[2];
    return v;
}

static inline db_vector4 db_vector4_zero(void)
{
    db_vector4 v = {0, 0, 0, 0};
    return v;
}
static inline db_vector4 db_vector4_make(f32 x, f32 y, f32 z, f32 w)
{
    db_vector4 v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}
static inline db_vector4 db_vector4_from_array(f32 x[4])
{
    db_vector4 v;
    v.x = x[0];
    v.y = x[1];
    v.z = x[2];
    v.w = x[3];
    return v;
}

static inline void db__vector2_add(db_vector2 *d, db_vector2 v0, db_vector2 v1)
{
    DB_VEC2_3OP(d, v0, +, v1, +0);
}
static inline void db__vector2_subtract(db_vector2 *d, db_vector2 v0, db_vector2 v1)
{
    DB_VEC2_3OP(d, v0, -, v1, +0);
}
static inline void db__vector2_multiply(db_vector2 *d, db_vector2 v, f32 s)
{
    DB_VEC2_2OP(d, v, *s);
}
static inline void db__vector2_divide(db_vector2 *d, db_vector2 v, f32 s)
{
    DB_VEC2_2OP(d, v, / s);
}

static inline void db__vector3_add(db_vector3 *d, db_vector3 v0, db_vector3 v1)
{
    DB_VEC3_3OP(d, v0, +, v1, +0);
}
static inline void db__vector3_subtract(db_vector3 *d, db_vector3 v0, db_vector3 v1)
{
    DB_VEC3_3OP(d, v0, -, v1, +0);
}
static inline void db__vector3_multiply(db_vector3 *d, db_vector3 v, f32 s)
{
    DB_VEC3_2OP(d, v, *s);
}
static inline void db__vector3_divide(db_vector3 *d, db_vector3 v, f32 s)
{
    DB_VEC3_2OP(d, v, / s);
}

static inline void db__vector4_add(db_vector4 *d, db_vector4 v0, db_vector4 v1)
{
    DB_VEC4_3OP(d, v0, +, v1, +0);
}
static inline void db__vector4_subtract(db_vector4 *d, db_vector4 v0, db_vector4 v1)
{
    DB_VEC4_3OP(d, v0, -, v1, +0);
}
static inline void db__vector4_multiply(db_vector4 *d, db_vector4 v, f32 s)
{
    DB_VEC4_2OP(d, v, *s);
}
static inline void db__vector4_divide(db_vector4 *d, db_vector4 v, f32 s)
{
    DB_VEC4_2OP(d, v, / s);
}

static inline void db_vector2_addeq(db_vector2 *d, db_vector2 v)
{
    DB_VEC2_3OP(d, (*d), +, v, +0);
}
static inline void db_vector2_subeq(db_vector2 *d, db_vector2 v)
{
    DB_VEC2_3OP(d, (*d), -, v, +0);
}
static inline void db_vector2_muleq(db_vector2 *d, f32 s)
{
    DB_VEC2_2OP(d, (*d), *s);
}
static inline void db_vector2_diveq(db_vector2 *d, f32 s)
{
    DB_VEC2_2OP(d, (*d), / s);
}

static inline void db_vector3_addeq(db_vector3 *d, db_vector3 v)
{
    DB_VEC3_3OP(d, (*d), +, v, +0);
}
static inline void db_vector3_subeq(db_vector3 *d, db_vector3 v)
{
    DB_VEC3_3OP(d, (*d), -, v, +0);
}
static inline void db_vector3_muleq(db_vector3 *d, f32 s)
{
    DB_VEC3_2OP(d, (*d), *s);
}
static inline void db_vector3_diveq(db_vector3 *d, f32 s)
{
    DB_VEC3_2OP(d, (*d), / s);
}

static inline void db_vector4_addeq(db_vector4 *d, db_vector4 v)
{
    DB_VEC4_3OP(d, (*d), +, v, +0);
}
static inline void db_vector4_subeq(db_vector4 *d, db_vector4 v)
{
    DB_VEC4_3OP(d, (*d), -, v, +0);
}
static inline void db_vector4_muleq(db_vector4 *d, f32 s)
{
    DB_VEC4_2OP(d, (*d), *s);
}
static inline void db_vector4_diveq(db_vector4 *d, f32 s)
{
    DB_VEC4_2OP(d, (*d), / s);
}

#undef DB_VEC2_2OP
#undef DB_VEC2_3OP
#undef DB_VEC3_3OP
#undef DB_VEC3_2OP
#undef DB_VEC4_2OP
#undef DB_VEC4_3OP

static inline f32 db_vector2_dot_multiplication(db_vector2 v0, db_vector2 v1)
{
    return v0.x * v1.x + v0.y * v1.y;
}
static inline f32 db_vector3_dot_multiplication(db_vector3 v0, db_vector3 v1)
{
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}
static inline f32 db_vector4_dot_multiplication(db_vector4 v0, db_vector4 v1)
{
    return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
}

static inline void db__vector2_cross(f32 *d, db_vector2 v0, db_vector2 v1)
{
    *d = v0.x * v1.y - v1.x * v0.y;
}
static inline void db__vector3_cross(db_vector3 *d, db_vector3 v0, db_vector3 v1)
{
    d->x = v0.y * v1.z - v0.z * v1.y;
    d->y = v0.z * v1.x - v0.x * v1.z;
    d->z = v0.x * v1.y - v0.y * v1.x;
}

static inline f32 db_vector2_magnitude_squared(db_vector2 v)
{
    return db_vector2_dot_multiplication(v, v);
}
static inline f32 db_vector3_magnitude_squared(db_vector3 v)
{
    return db_vector3_dot_multiplication(v, v);
}
static inline f32 db_vector4_magnitude_squared(db_vector4 v)
{
    return db_vector4_dot_multiplication(v, v);
}

static inline f32 db_vector2_magnitude(db_vector2 v)
{
    return db_sqrt(db_vector2_dot_multiplication(v, v));
}
static inline f32 db_vector3_magnitude(db_vector3 v)
{
    return db_sqrt(db_vector3_dot_multiplication(v, v));
}
static inline f32 db_vector4_magnitude(db_vector4 v)
{
    return db_sqrt(db_vector4_dot_multiplication(v, v));
}

static inline void db__vector2_normalize(db_vector2 *d, db_vector2 v)
{
    f32 inv_mag = db_rsqrt(db_vector2_dot_multiplication(v, v));
    db__vector2_multiply(d, v, inv_mag);
}
static inline void db__vector3_normalize(db_vector3 *d, db_vector3 v)
{
    f32 mag = db_vector3_magnitude(v);
    db__vector3_divide(d, v, mag);
}
static inline void db__vector4_normalize(db_vector4 *d, db_vector4 v)
{
    f32 mag = db_vector4_magnitude(v);
    db__vector4_divide(d, v, mag);
}

static inline void db__vector2_normalize_or_zero(db_vector2 *d, db_vector2 v)
{
    f32 mag = db_vector2_magnitude(v);
    if (mag > 0)
        db__vector2_divide(d, v, mag);
    else
        *d = db_vector2_zero();
}
static inline void db__vector3_normalize_or_zero(db_vector3 *d, db_vector3 v)
{
    f32 mag = db_vector3_magnitude(v);
    if (mag > 0)
        db__vector3_divide(d, v, mag);
    else
        *d = db_vector3_zero();
}
static inline void db__vector4_normalize_or_zero(db_vector4 *d, db_vector4 v)
{
    f32 mag = db_vector4_magnitude(v);
    if (mag > 0)
        db__vector4_divide(d, v, mag);
    else
        *d = db_vector4_zero();
}

static inline void db__vector2_reflect(db_vector2 *d, db_vector2 i, db_vector2 n)
{
    db_vector2 b = n;
    db_vector2_muleq(&b, 2.0f * db_vector2_dot_multiplication(n, i));
    db__vector2_subtract(d, i, b);
}

static inline void db__vector3_reflect(db_vector3 *d, db_vector3 i, db_vector3 n)
{
    db_vector3 b = n;
    db_vector3_muleq(&b, 2.0f * db_vector3_dot_multiplication(n, i));
    db__vector3_subtract(d, i, b);
}

static inline void db__vector2_refract(db_vector2 *d, db_vector2 i, db_vector2 n, f32 eta)
{
    db_vector2 a, b;
    f32        dv, k;

    dv = db_vector2_dot_multiplication(n, i);
    k  = 1.0f - eta * eta * (1.0f - dv * dv);
    db__vector2_multiply(&a, i, eta);
    db__vector2_multiply(&b, n, eta * dv * db_sqrt(k));
    db__vector2_subtract(d, a, b);
    db_vector2_muleq(d, (f32)(k >= 0.0f));
}

static inline void db__vector3_refract(db_vector3 *d, db_vector3 i, db_vector3 n, f32 eta)
{
    db_vector3 a, b;
    f32        dv, k;

    dv = db_vector3_dot_multiplication(n, i);
    k  = 1.0f - eta * eta * (1.0f - dv * dv);
    db__vector3_multiply(&a, i, eta);
    db__vector3_multiply(&b, n, eta * dv * db_sqrt(k));
    db__vector3_subtract(d, a, b);
    db_vector3_muleq(d, (f32)(k >= 0.0f));
}

static inline f32 db_vector2_aspect_ratio(db_vector2 v)
{
    return (v.y < 0.0001f) ? 0.0f : v.x / v.y;
}

static inline void db_matrix2_transpose(db_matrix2 *m)
{
    db_float22_transpose(db_float22_m(m));
}
static inline void db_matrix2_identity(db_matrix2 *m)
{
    db_float22_identity(db_float22_m(m));
}
static inline void db_matrix2_mul(db_matrix2 *out, db_matrix2 *m1, db_matrix2 *m2)
{
    db_float22_mul(db_float22_m(out), db_float22_m(m1), db_float22_m(m2));
}

static inline void db_float22_identity(f32 m[2][2])
{
    m[0][0] = 1;
    m[0][1] = 0;
    m[1][0] = 0;
    m[1][1] = 1;
}

static inline void db_matrix2_mul_vector2(db_vector2 *out, db_matrix2 *m, db_vector2 in)
{
    db_float22_mul_vector2(out, db_float22_m(m), in);
}

static inline db_matrix2 *db_matrix2_v(db_vector2 m[2])
{
    return (db_matrix2 *)m;
}
static inline db_matrix2 *db_matrix2_f(f32 m[2][2])
{
    return (db_matrix2 *)m;
}

static inline db_float2 *db_float22_m(db_matrix2 *m)
{
    return (db_float2 *)m;
}
static inline db_float2 *db_float22_v(db_vector2 m[2])
{
    return (db_float2 *)m;
}
static inline db_float2 *db_float22_4(f32 m[4])
{
    return (db_float2 *)m;
}

static inline void db_float22_transpose(f32 (*vec)[2])
{
    int i, j;
    for (j = 0; j < 2; j++)
    {
        for (i = j + 1; i < 2; i++)
        {
            f32 t     = vec[i][j];
            vec[i][j] = vec[j][i];
            vec[j][i] = t;
        }
    }
}

static inline void db_float22_mul(f32 (*out)[2], f32 (*mat1)[2], f32 (*mat2)[2])
{
    int i, j;
    f32 temp1[2][2], temp2[2][2];
    if (mat1 == out)
    {
        db__memcpy_4byte(temp1, mat1, sizeof(temp1));
        mat1 = temp1;
    }
    if (mat2 == out)
    {
        db__memcpy_4byte(temp2, mat2, sizeof(temp2));
        mat2 = temp2;
    }
    for (j = 0; j < 2; j++)
    {
        for (i = 0; i < 2; i++)
        {
            out[j][i] = mat1[0][i] * mat2[j][0] + mat1[1][i] * mat2[j][1];
        }
    }
}

static inline void db_float22_mul_vector2(db_vector2 *out, f32 m[2][2], db_vector2 v)
{
    out->x = m[0][0] * v.x + m[1][0] * v.y;
    out->y = m[0][1] * v.x + m[1][1] * v.y;
}

static inline f32 db_matrix2_determinate(db_matrix2 *m)
{
    db_float2 *e = db_float22_m(m);
    return e[0][0] * e[1][1] - e[1][0] * e[0][1];
}

static inline void db_matrix2_inverse(db_matrix2 *out, db_matrix2 *in)
{
    db_float2 *o = db_float22_m(out);
    db_float2 *i = db_float22_m(in);

    f32 ood = 1.0f / db_matrix2_determinate(in);

    o[0][0] = +i[1][1] * ood;
    o[0][1] = -i[0][1] * ood;
    o[1][0] = -i[1][0] * ood;
    o[1][1] = +i[0][0] * ood;
}

static inline void db_matrix3_transpose(db_matrix3 *m)
{
    db_float33_transpose(db_float33_m(m));
}
static inline void db_matrix3_identity(db_matrix3 *m)
{
    db_float33_identity(db_float33_m(m));
}
static inline void db_matrix3_mul(db_matrix3 *out, db_matrix3 *m1, db_matrix3 *m2)
{
    db_float33_mul(db_float33_m(out), db_float33_m(m1), db_float33_m(m2));
}

static inline void db_float33_identity(f32 m[3][3])
{
    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;
    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;
    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
}

static inline void db_matrix3_mul_vector3(db_vector3 *out, db_matrix3 *m, db_vector3 in)
{
    db_float33_mul_vector3(out, db_float33_m(m), in);
}

static inline db_matrix3 *db_matrix3_v(db_vector3 m[3])
{
    return (db_matrix3 *)m;
}
static inline db_matrix3 *db_matrix3_f(f32 m[3][3])
{
    return (db_matrix3 *)m;
}

static inline db_float3 *db_float33_m(db_matrix3 *m)
{
    return (db_float3 *)m;
}
static inline db_float3 *db_float33_v(db_vector3 m[3])
{
    return (db_float3 *)m;
}
static inline db_float3 *db_float33_9(f32 m[9])
{
    return (db_float3 *)m;
}

static inline void db_float33_transpose(f32 (*vec)[3])
{
    int i, j;
    for (j = 0; j < 3; j++)
    {
        for (i = j + 1; i < 3; i++)
        {
            f32 t     = vec[i][j];
            vec[i][j] = vec[j][i];
            vec[j][i] = t;
        }
    }
}

static inline void db_float33_mul(f32 (*out)[3], f32 (*mat1)[3], f32 (*mat2)[3])
{
    int i, j;
    f32 temp1[3][3], temp2[3][3];
    if (mat1 == out)
    {
        db__memcpy_4byte(temp1, mat1, sizeof(temp1));
        mat1 = temp1;
    }
    if (mat2 == out)
    {
        db__memcpy_4byte(temp2, mat2, sizeof(temp2));
        mat2 = temp2;
    }
    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < 3; i++)
        {
            out[j][i] = mat1[0][i] * mat2[j][0] + mat1[1][i] * mat2[j][1] + mat1[2][i] * mat2[j][2];
        }
    }
}

static inline void db_float33_mul_vector3(db_vector3 *out, f32 m[3][3], db_vector3 v)
{
    out->x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z;
    out->y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z;
    out->z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z;
}

static inline f32 db_matrix3_determinate(db_matrix3 *m)
{
    db_float3 *e = db_float33_m(m);
    f32 d = +e[0][0] * (e[1][1] * e[2][2] - e[1][2] * e[2][1]) - e[0][1] * (e[1][0] * e[2][2] - e[1][2] * e[2][0]) +
            e[0][2] * (e[1][0] * e[2][1] - e[1][1] * e[2][0]);
    return d;
}

static inline void db_matrix3_inverse(db_matrix3 *out, db_matrix3 *in)
{
    db_float3 *o = db_float33_m(out);
    db_float3 *i = db_float33_m(in);

    f32 ood = 1.0f / db_matrix3_determinate(in);

    o[0][0] = +(i[1][1] * i[2][2] - i[2][1] * i[1][2]) * ood;
    o[0][1] = -(i[1][0] * i[2][2] - i[2][0] * i[1][2]) * ood;
    o[0][2] = +(i[1][0] * i[2][1] - i[2][0] * i[1][1]) * ood;
    o[1][0] = -(i[0][1] * i[2][2] - i[2][1] * i[0][2]) * ood;
    o[1][1] = +(i[0][0] * i[2][2] - i[2][0] * i[0][2]) * ood;
    o[1][2] = -(i[0][0] * i[2][1] - i[2][0] * i[0][1]) * ood;
    o[2][0] = +(i[0][1] * i[1][2] - i[1][1] * i[0][2]) * ood;
    o[2][1] = -(i[0][0] * i[1][2] - i[1][0] * i[0][2]) * ood;
    o[2][2] = +(i[0][0] * i[1][1] - i[1][0] * i[0][1]) * ood;
}

static inline void db_matrix4_transpose(db_matrix4 *m)
{
    db_float44_transpose(db_float44_m(m));
}
static inline void db_matrix4_identity(db_matrix4 *m)
{
    db_float44_identity(db_float44_m(m));
}
static inline void db_matrix4_mul(db_matrix4 *out, db_matrix4 *m1, db_matrix4 *m2)
{
    db_float44_mul(db_float44_m(out), db_float44_m(m1), db_float44_m(m2));
}

static inline void db_float44_identity(f32 m[4][4])
{
    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = 0;
    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;
    m[1][3] = 0;
    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
    m[2][3] = 0;
    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
}

static inline void db_matrix4_mul_vector4(db_vector4 *out, db_matrix4 *m, db_vector4 in)
{
    db_float44_mul_vector4(out, db_float44_m(m), in);
}

static inline db_matrix4 *db_matrix4_v(db_vector4 m[4])
{
    return (db_matrix4 *)m;
}
static inline db_matrix4 *db_matrix4_f(f32 m[4][4])
{
    return (db_matrix4 *)m;
}

static inline db_float4 *db_float44_m(db_matrix4 *m)
{
    return (db_float4 *)m;
}
static inline db_float4 *db_float44_v(db_vector4 m[4])
{
    return (db_float4 *)m;
}
static inline db_float4 *db_float44_16(f32 m[16])
{
    return (db_float4 *)m;
}

static inline void db_float44_transpose(f32 (*vec)[4])
{
    f32 tmp;
    tmp       = vec[1][0];
    vec[1][0] = vec[0][1];
    vec[0][1] = tmp;
    tmp       = vec[2][0];
    vec[2][0] = vec[0][2];
    vec[0][2] = tmp;
    tmp       = vec[3][0];
    vec[3][0] = vec[0][3];
    vec[0][3] = tmp;
    tmp       = vec[2][1];
    vec[2][1] = vec[1][2];
    vec[1][2] = tmp;
    tmp       = vec[3][1];
    vec[3][1] = vec[1][3];
    vec[1][3] = tmp;
    tmp       = vec[3][2];
    vec[3][2] = vec[2][3];
    vec[2][3] = tmp;
}

static inline void db_float44_mul(f32 (*out)[4], f32 (*mat1)[4], f32 (*mat2)[4])
{
    int i, j;
    f32 temp1[4][4], temp2[4][4];
    if (mat1 == out)
    {
        db__memcpy_4byte(temp1, mat1, sizeof(temp1));
        mat1 = temp1;
    }
    if (mat2 == out)
    {
        db__memcpy_4byte(temp2, mat2, sizeof(temp2));
        mat2 = temp2;
    }
    for (j = 0; j < 4; j++)
    {
        for (i = 0; i < 4; i++)
        {
            out[j][i] =
                mat1[0][i] * mat2[j][0] + mat1[1][i] * mat2[j][1] + mat1[2][i] * mat2[j][2] + mat1[3][i] * mat2[j][3];
        }
    }
}

static inline void db_float44_mul_vector4(db_vector4 *out, f32 m[4][4], db_vector4 v)
{
    out->x = m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w;
    out->y = m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w;
    out->z = m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w;
    out->w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w;
}

static inline void db_matrix4_inverse(db_matrix4 *out, db_matrix4 *in)
{
    db_float4 *o = db_float44_m(out);
    db_float4 *m = db_float44_m(in);

    f32 ood;

    f32 sf00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    f32 sf01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    f32 sf02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    f32 sf03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
    f32 sf04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    f32 sf05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    f32 sf06 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
    f32 sf07 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
    f32 sf08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
    f32 sf09 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
    f32 sf10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
    f32 sf11 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
    f32 sf12 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    f32 sf13 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
    f32 sf14 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
    f32 sf15 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
    f32 sf16 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
    f32 sf17 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
    f32 sf18 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

    o[0][0] = +(m[1][1] * sf00 - m[1][2] * sf01 + m[1][3] * sf02);
    o[1][0] = -(m[1][0] * sf00 - m[1][2] * sf03 + m[1][3] * sf04);
    o[2][0] = +(m[1][0] * sf01 - m[1][1] * sf03 + m[1][3] * sf05);
    o[3][0] = -(m[1][0] * sf02 - m[1][1] * sf04 + m[1][2] * sf05);

    o[0][1] = -(m[0][1] * sf00 - m[0][2] * sf01 + m[0][3] * sf02);
    o[1][1] = +(m[0][0] * sf00 - m[0][2] * sf03 + m[0][3] * sf04);
    o[2][1] = -(m[0][0] * sf01 - m[0][1] * sf03 + m[0][3] * sf05);
    o[3][1] = +(m[0][0] * sf02 - m[0][1] * sf04 + m[0][2] * sf05);

    o[0][2] = +(m[0][1] * sf06 - m[0][2] * sf07 + m[0][3] * sf08);
    o[1][2] = -(m[0][0] * sf06 - m[0][2] * sf09 + m[0][3] * sf10);
    o[2][2] = +(m[0][0] * sf11 - m[0][1] * sf09 + m[0][3] * sf12);
    o[3][2] = -(m[0][0] * sf08 - m[0][1] * sf10 + m[0][2] * sf12);

    o[0][3] = -(m[0][1] * sf13 - m[0][2] * sf14 + m[0][3] * sf15);
    o[1][3] = +(m[0][0] * sf13 - m[0][2] * sf16 + m[0][3] * sf17);
    o[2][3] = -(m[0][0] * sf14 - m[0][1] * sf16 + m[0][3] * sf18);
    o[3][3] = +(m[0][0] * sf15 - m[0][1] * sf17 + m[0][2] * sf18);

    ood = 1.0f / (m[0][0] * o[0][0] + m[0][1] * o[1][0] + m[0][2] * o[2][0] + m[0][3] * o[3][0]);

    o[0][0] *= ood;
    o[0][1] *= ood;
    o[0][2] *= ood;
    o[0][3] *= ood;
    o[1][0] *= ood;
    o[1][1] *= ood;
    o[1][2] *= ood;
    o[1][3] *= ood;
    o[2][0] *= ood;
    o[2][1] *= ood;
    o[2][2] *= ood;
    o[2][3] *= ood;
    o[3][0] *= ood;
    o[3][1] *= ood;
    o[3][2] *= ood;
    o[3][3] *= ood;
}

static inline void db_matrix4_translate(db_matrix4 *out, db_vector3 v)
{
    db_matrix4_identity(out);
    out->col[3].xyz = v;
    out->col[3].w   = 1;
}

static inline void db_matrix4_rotate(db_matrix4 *out, db_vector3 v, f32 angle_radians)
{
    f32        c, s;
    db_vector3 axis, t;
    db_float4 *rot;

    c = db_cos(angle_radians);
    s = db_sin(angle_radians);

    db__vector3_normalize(&axis, v);
    db__vector3_multiply(&t, axis, 1.0f - c);

    db_matrix4_identity(out);
    rot = db_float44_m(out);

    rot[0][0] = c + t.x * axis.x;
    rot[0][1] = 0 + t.x * axis.y + s * axis.z;
    rot[0][2] = 0 + t.x * axis.z - s * axis.y;
    rot[0][3] = 0;

    rot[1][0] = 0 + t.y * axis.x - s * axis.z;
    rot[1][1] = c + t.y * axis.y;
    rot[1][2] = 0 + t.y * axis.z + s * axis.x;
    rot[1][3] = 0;

    rot[2][0] = 0 + t.z * axis.x + s * axis.y;
    rot[2][1] = 0 + t.z * axis.y - s * axis.x;
    rot[2][2] = c + t.z * axis.z;
    rot[2][3] = 0;
}

static inline void db_matrix4_scale(db_matrix4 *out, db_vector3 v)
{
    db_matrix4_identity(out);
    out->data[0]  = v.x;
    out->data[5]  = v.y;
    out->data[10] = v.z;
}

static inline void db_matrix4_scalef(db_matrix4 *out, f32 s)
{
    db_matrix4_identity(out);
    out->data[0]  = s;
    out->data[5]  = s;
    out->data[10] = s;
}

static inline void db_matrix4_ortho2d(db_matrix4 *out, f32 left, f32 right, f32 bottom, f32 top)
{
    db_float4 *m;
    db_matrix4_identity(out);
    m = db_float44_m(out);

    m[0][0] = 2.0f / (right - left);
    m[1][1] = 2.0f / (top - bottom);
    m[2][2] = -1.0f;
    m[3][0] = -(right + left) / (right - left);
    m[3][1] = -(top + bottom) / (top - bottom);
}

static inline void db_matrix4_ortho3d(db_matrix4 *out, f32 left, f32 right, f32 bottom, f32 top, f32 z_near, f32 z_far)
{
    db_float4 *m;
    db_matrix4_identity(out);
    m = db_float44_m(out);

    m[0][0] = +2.0f / (right - left);
    m[1][1] = +2.0f / (top - bottom);
    m[2][2] = -2.0f / (z_far - z_near);
    m[3][0] = -(right + left) / (right - left);
    m[3][1] = -(top + bottom) / (top - bottom);
    m[3][2] = -(z_far + z_near) / (z_far - z_near);
}

static inline void db_matrix4_perspective(db_matrix4 *out, f32 fovy, f32 aspect, f32 z_near, f32 z_far)
{
    f32        tan_half_fovy = db_tan(0.5f * fovy);
    db_matrix4 zero_mat      = {0};
    db_float4 *m             = db_float44_m(out);
    *out                     = zero_mat;

    m[0][0] = 1.0f / (aspect * tan_half_fovy);
    m[1][1] = 1.0f / (tan_half_fovy);
    m[2][2] = -(z_far + z_near) / (z_far - z_near);
    m[2][3] = -1.0f;
    m[3][2] = -2.0f * z_far * z_near / (z_far - z_near);
}

static inline void db_matrix4_infinite_perspective(db_matrix4 *out, f32 fovy, f32 aspect, f32 z_near)
{
    f32        range    = db_tan(0.5f * fovy) * z_near;
    f32        left     = -range * aspect;
    f32        right    = range * aspect;
    f32        bottom   = -range;
    f32        top      = range;
    db_matrix4 zero_mat = {0};
    db_float4 *m        = db_float44_m(out);
    *out                = zero_mat;

    m[0][0] = (2.0f * z_near) / (right - left);
    m[1][1] = (2.0f * z_near) / (top - bottom);
    m[2][2] = -1.0f;
    m[2][3] = -1.0f;
    m[3][2] = -2.0f * z_near;
}

static inline void db_matrix4_look_at(db_matrix4 *out, db_vector3 eye, db_vector3 centre, db_vector3 up)
{
    db_vector3 f, s, u;
    db_float4 *m;

    db__vector3_subtract(&f, centre, eye);
    db__vector3_normalize(&f, f);

    db__vector3_cross(&s, f, up);
    db__vector3_normalize(&s, s);

    db__vector3_cross(&u, s, f);

    db_matrix4_identity(out);
    m = db_float44_m(out);

    m[0][0] = +s.x;
    m[1][0] = +s.y;
    m[2][0] = +s.z;

    m[0][1] = +u.x;
    m[1][1] = +u.y;
    m[2][1] = +u.z;

    m[0][2] = -f.x;
    m[1][2] = -f.y;
    m[2][2] = -f.z;

    m[3][0] = -db_vector3_dot_multiplication(s, eye);
    m[3][1] = -db_vector3_dot_multiplication(u, eye);
    m[3][2] = +db_vector3_dot_multiplication(f, eye);
}

static inline db_quaternion db_quaternion_make(f32 x, f32 y, f32 z, f32 w)
{
    db_quaternion q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}
static inline db_quaternion db_quaternion_from_array(f32 e[4])
{
    db_quaternion q;
    q.x = e[0];
    q.y = e[1];
    q.z = e[2];
    q.w = e[3];
    return q;
}

static inline db_quaternion db_quaternion_axis_angle(db_vector3 axis, f32 angle_radians)
{
    db_quaternion q;
    db__vector3_normalize(&q.xyz, axis);
    db_vector3_muleq(&q.xyz, db_sin(0.5f * angle_radians));
    q.w = db_cos(0.5f * angle_radians);
    return q;
}

static inline db_quaternion db_quaternion_euler_angles(f32 pitch, f32 yaw, f32 roll)
{
    db_quaternion q, p, y, r;
    p = db_quaternion_axis_angle(db_vector3_make(1, 0, 0), pitch);
    y = db_quaternion_axis_angle(db_vector3_make(0, 1, 0), yaw);
    r = db_quaternion_axis_angle(db_vector3_make(0, 0, 1), roll);

    db_quaternion_mul(&q, y, p);
    db_quaternion_muleq(&q, r);

    return q;
}

static inline db_quaternion db_quaternion_identity(void)
{
    db_quaternion q = {0, 0, 0, 1};
    return q;
}

static inline void db_quaternion_add(db_quaternion *d, db_quaternion q0, db_quaternion q1)
{
    db__vector4_add(&d->xyzw, q0.xyzw, q1.xyzw);
}
static inline void db_quaternion_sub(db_quaternion *d, db_quaternion q0, db_quaternion q1)
{
    db__vector4_subtract(&d->xyzw, q0.xyzw, q1.xyzw);
}

static inline void db_quaternion_mul(db_quaternion *d, db_quaternion q0, db_quaternion q1)
{
    d->x = q0.w * q1.x + q0.x * q1.w + q0.y * q1.z - q0.z * q1.y;
    d->y = q0.w * q1.y - q0.x * q1.z + q0.y * q1.w + q0.z * q1.x;
    d->z = q0.w * q1.z + q0.x * q1.y - q0.y * q1.x + q0.z * q1.w;
    d->w = q0.w * q1.w - q0.x * q1.x - q0.y * q1.y - q0.z * q1.z;
}

static inline void db_quaternion_div(db_quaternion *d, db_quaternion q0, db_quaternion q1)
{
    db_quaternion iq1;
    db_quaternion_inverse(&iq1, q1);
    db_quaternion_mul(d, q0, iq1);
}

static inline void db_quaternion_mulf(db_quaternion *d, db_quaternion q0, f32 s)
{
    db__vector4_multiply(&d->xyzw, q0.xyzw, s);
}
static inline void db_quaternion_divf(db_quaternion *d, db_quaternion q0, f32 s)
{
    db__vector4_divide(&d->xyzw, q0.xyzw, s);
}

static inline void db_quaternion_addeq(db_quaternion *d, db_quaternion q)
{
    db_vector4_addeq(&d->xyzw, q.xyzw);
}
static inline void db_quaternion_subeq(db_quaternion *d, db_quaternion q)
{
    db_vector4_subeq(&d->xyzw, q.xyzw);
}
static inline void db_quaternion_muleq(db_quaternion *d, db_quaternion q)
{
    db_quaternion_mul(d, *d, q);
}
static inline void db_quaternion_diveq(db_quaternion *d, db_quaternion q)
{
    db_quaternion_div(d, *d, q);
}

static inline void db_quaternion_muleqf(db_quaternion *d, f32 s)
{
    db_vector4_muleq(&d->xyzw, s);
}
static inline void db_quaternion_diveqf(db_quaternion *d, f32 s)
{
    db_vector4_diveq(&d->xyzw, s);
}

static inline f32 db_quaternion_dot(db_quaternion q0, db_quaternion q1)
{
    f32 r = db_vector3_dot_multiplication(q0.xyz, q1.xyz) + q0.w * q1.w;
    return r;
}
static inline f32 db_quaternion_mag(db_quaternion q)
{
    f32 r = db_sqrt(db_quaternion_dot(q, q));
    return r;
}

static inline void db_quaternion_norm(db_quaternion *d, db_quaternion q)
{
    db_quaternion_divf(d, q, db_quaternion_mag(q));
}
static inline void db_quaternion_conj(db_quaternion *d, db_quaternion q)
{
    d->xyz = db_vector3_make(-q.x, -q.y, -q.z);
    d->w   = q.w;
}
static inline void db_quaternion_inverse(db_quaternion *d, db_quaternion q)
{
    db_quaternion_conj(d, q);
    db_quaternion_diveqf(d, db_quaternion_dot(q, q));
}

static inline void db_quaternion_axis(db_vector3 *axis, db_quaternion q)
{
    db_quaternion n;
    db_quaternion_norm(&n, q);
    db__vector3_divide(axis, n.xyz, db_sin(db_arccos(q.w)));
}

static inline f32 db_quaternion_angle(db_quaternion q)
{
    f32 mag   = db_quaternion_mag(q);
    f32 c     = q.w * (1.0f / mag);
    f32 angle = 2.0f * db_arccos(c);
    return angle;
}

static inline f32 db_quaternion_roll(db_quaternion q)
{
    return db_arctan2(2.0f * q.x * q.y + q.z * q.w, q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z);
}
static inline f32 db_quaternion_pitch(db_quaternion q)
{
    return db_arctan2(2.0f * q.y * q.z + q.w * q.x, q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
}
static inline f32 db_quaternion_yaw(db_quaternion q)
{
    return db_arcsin(-2.0f * (q.x * q.z - q.w * q.y));
}

static inline void db_quaternion_rotate_vector3(db_vector3 *d, db_quaternion q, db_vector3 v)
{
    /* db_vector3 t = 2.0f * cross(q.xyz, v);
     * *d = q.w*t + v + cross(q.xyz, t);
     */
    db_vector3 t, p;
    db__vector3_cross(&t, q.xyz, v);
    db_vector3_muleq(&t, 2.0f);

    db__vector3_cross(&p, q.xyz, t);

    db__vector3_multiply(d, t, q.w);
    db_vector3_addeq(d, v);
    db_vector3_addeq(d, p);
}

static inline void db_matrix4_from_quat(db_matrix4 *out, db_quaternion q)
{
    db_float4    *m;
    db_quaternion a;
    f32           xx, yy, zz, xy, xz, yz, wx, wy, wz;

    db_quaternion_norm(&a, q);
    xx = a.x * a.x;
    yy = a.y * a.y;
    zz = a.z * a.z;
    xy = a.x * a.y;
    xz = a.x * a.z;
    yz = a.y * a.z;
    wx = a.w * a.x;
    wy = a.w * a.y;
    wz = a.w * a.z;

    db_matrix4_identity(out);
    m = db_float44_m(out);

    m[0][0] = 1.0f - 2.0f * (yy + zz);
    m[0][1] = 2.0f * (xy + wz);
    m[0][2] = 2.0f * (xz - wy);

    m[1][0] = 2.0f * (xy - wz);
    m[1][1] = 1.0f - 2.0f * (xx + zz);
    m[1][2] = 2.0f * (yz + wx);

    m[2][0] = 2.0f * (xz + wy);
    m[2][1] = 2.0f * (yz - wx);
    m[2][2] = 1.0f - 2.0f * (xx + yy);
}

static inline void db_quaternion_from_mat4(db_quaternion *out, db_matrix4 *mat)
{
    db_float4 *m;
    f32        four_x_squared_minus_1, four_y_squared_minus_1, four_z_squared_minus_1, four_w_squared_minus_1,
        four_biggest_squared_minus_1;
    int biggest_index = 0;
    f32 biggest_value, mult;

    m = db_float44_m(mat);

    four_x_squared_minus_1 = m[0][0] - m[1][1] - m[2][2];
    four_y_squared_minus_1 = m[1][1] - m[0][0] - m[2][2];
    four_z_squared_minus_1 = m[2][2] - m[0][0] - m[1][1];
    four_w_squared_minus_1 = m[0][0] + m[1][1] + m[2][2];

    four_biggest_squared_minus_1 = four_w_squared_minus_1;
    if (four_x_squared_minus_1 > four_biggest_squared_minus_1)
    {
        four_biggest_squared_minus_1 = four_x_squared_minus_1;
        biggest_index                = 1;
    }
    if (four_y_squared_minus_1 > four_biggest_squared_minus_1)
    {
        four_biggest_squared_minus_1 = four_y_squared_minus_1;
        biggest_index                = 2;
    }
    if (four_z_squared_minus_1 > four_biggest_squared_minus_1)
    {
        four_biggest_squared_minus_1 = four_z_squared_minus_1;
        biggest_index                = 3;
    }

    biggest_value = db_sqrt(four_biggest_squared_minus_1 + 1.0f) * 0.5f;
    mult          = 0.25f / biggest_value;

    switch (biggest_index)
    {
        case 0:
            out->w = biggest_value;
            out->x = (m[1][2] - m[2][1]) * mult;
            out->y = (m[2][0] - m[0][2]) * mult;
            out->z = (m[0][1] - m[1][0]) * mult;
            break;
        case 1:
            out->w = (m[1][2] - m[2][1]) * mult;
            out->x = biggest_value;
            out->y = (m[0][1] + m[1][0]) * mult;
            out->z = (m[2][0] + m[0][2]) * mult;
            break;
        case 2:
            out->w = (m[2][0] - m[0][2]) * mult;
            out->x = (m[0][1] + m[1][0]) * mult;
            out->y = biggest_value;
            out->z = (m[1][2] + m[2][1]) * mult;
            break;
        case 3:
            out->w = (m[0][1] - m[1][0]) * mult;
            out->x = (m[2][0] + m[0][2]) * mult;
            out->y = (m[1][2] + m[2][1]) * mult;
            out->z = biggest_value;
            break;
        default:
            break;
    }
}

static inline f32 db_lerp(f32 a, f32 b, f32 t)
{
    return a * (1.0f - t) + b * t;
}
static inline f32 db_unlerp(f32 t, f32 a, f32 b)
{
    return (t - a) / (b - a);
}
static inline f32 db_smooth_step(f32 a, f32 b, f32 t)
{
    f32 x = (t - a) / (b - a);
    return x * x * (3.0f - 2.0f * x);
}
static inline f32 db_smoother_step(f32 a, f32 b, f32 t)
{
    f32 x = (t - a) / (b - a);
    return x * x * x * (x * (6.0f * x - 15.0f) + 10.0f);
}

#define DB_VEC_LERPN(N, d, a, b, t)                                                                                    \
    db_vector##N db;                                                                                                   \
    db__vector##N##_subtract(&db, b, a);                                                                               \
    db_vector##N##_muleq(&db, t);                                                                                      \
    db__vector##N##_add(d, a, db)
static inline void db__vector2_lerp(db_vector2 *d, db_vector2 a, db_vector2 b, f32 t)
{
    DB_VEC_LERPN(2, d, a, b, t);
}
static inline void db__vector3_lerp(db_vector3 *d, db_vector3 a, db_vector3 b, f32 t)
{
    DB_VEC_LERPN(3, d, a, b, t);
}
static inline void db__vector4_lerp(db_vector4 *d, db_vector4 a, db_vector4 b, f32 t)
{
    DB_VEC_LERPN(4, d, a, b, t);
}

#undef DB_VEC_LERPN

static inline void db_quaternion_lerp(db_quaternion *d, db_quaternion a, db_quaternion b, f32 t)
{
    db__vector4_lerp(&d->xyzw, a.xyzw, b.xyzw, t);
}
static inline void db_quaternion_nlerp(db_quaternion *d, db_quaternion a, db_quaternion b, f32 t)
{
    db_quaternion_lerp(d, a, b, t);
    db_quaternion_norm(d, *d);
}

static inline void db_quaternion_slerp(db_quaternion *d, db_quaternion a, db_quaternion b, f32 t)
{
    db_quaternion x, y, z;
    f32           cos_theta, angle;
    f32           s1, s0, is;

    z         = b;
    cos_theta = db_quaternion_dot(a, b);

    if (cos_theta < 0.0f)
    {
        z         = db_quaternion_make(-b.x, -b.y, -b.z, -b.w);
        cos_theta = -cos_theta;
    }

    if (cos_theta > 1.0f)
    {
        db_quaternion_lerp(d, a, b, t);
    }

    angle = db_arccos(cos_theta);

    s1 = db_sin((1.0f - t) * angle);
    s0 = db_sin(t * angle);
    is = 1.0f / db_sin(angle);
    db_quaternion_mulf(&x, a, s1);
    db_quaternion_mulf(&y, z, s0);
    db_quaternion_add(d, x, y);
    db_quaternion_muleqf(d, is);
}

static inline void db_quaternion_slerp_approx(db_quaternion *d, db_quaternion a, db_quaternion b, f32 t)
{
    f32 tp = t + (1.0f - db_quaternion_dot(a, b)) / 3.0f * t * (-2.0f * t * t + 3.0f * t - 1.0f);
    db_quaternion_nlerp(d, a, b, tp);
}

static inline void db_quaternion_nquad(db_quaternion *d, db_quaternion p, db_quaternion a, db_quaternion b,
                                       db_quaternion q, f32 t)
{
    db_quaternion x, y;
    db_quaternion_nlerp(&x, p, q, t);
    db_quaternion_nlerp(&y, a, b, t);
    db_quaternion_nlerp(d, x, y, 2.0f * t * (1.0f - t));
}

static inline void db_quaternion_squad(db_quaternion *d, db_quaternion p, db_quaternion a, db_quaternion b,
                                       db_quaternion q, f32 t)
{
    db_quaternion x, y;
    db_quaternion_slerp(&x, p, q, t);
    db_quaternion_slerp(&y, a, b, t);
    db_quaternion_slerp(d, x, y, 2.0f * t * (1.0f - t));
}

static inline void db_quaternion_squad_approx(db_quaternion *d, db_quaternion p, db_quaternion a, db_quaternion b,
                                              db_quaternion q, f32 t)
{
    db_quaternion x, y;
    db_quaternion_slerp_approx(&x, p, q, t);
    db_quaternion_slerp_approx(&y, a, b, t);
    db_quaternion_slerp_approx(d, x, y, 2.0f * t * (1.0f - t));
}

static inline db_rect2 db_rect2_make(db_vector2 pos, db_vector2 dim)
{
    db_rect2 r;
    r.pos = pos;
    r.dim = dim;
    return r;
}

static inline db_rect2 db_rect2v(f32 v[4])
{
    db_rect2 r;
    r.pos = db_vector2_from_array(&v[0]);
    r.dim = db_vector2_from_array(&v[2]);
    return r;
}

static inline db_rect3 db_rect3_make(db_vector3 pos, db_vector3 dim)
{
    db_rect3 r;
    r.pos = pos;
    r.dim = dim;
    return r;
}

static inline db_rect3 db_rect3v(f32 v[6])
{
    db_rect3 r;
    r.pos = db_vector3_from_array(&v[0]);
    r.dim = db_vector3_from_array(&v[3]);
    return r;
}

static inline int db_rect2_contains(db_rect2 a, f32 x, f32 y)
{
    f32 min_x  = db_min(a.pos.x, a.pos.x + a.dim.x);
    f32 max_x  = db_max(a.pos.x, a.pos.x + a.dim.x);
    f32 min_y  = db_min(a.pos.y, a.pos.y + a.dim.y);
    f32 max_y  = db_max(a.pos.y, a.pos.y + a.dim.y);
    int result = (x >= min_x) & (x < max_x) & (y >= min_y) & (y < max_y);
    return result;
}

static inline int db_rect2_contains_vector2(db_rect2 a, db_vector2 p)
{
    return db_rect2_contains(a, p.x, p.y);
}

static inline int db_rect2_intersects(db_rect2 a, db_rect2 b)
{
    db_rect2 r = {0};
    return db_rect2_intersection_result(a, b, &r);
}

static inline int db_rect2_intersection_result(db_rect2 a, db_rect2 b, db_rect2 *intersection)
{
    f32 a_min_x = db_min(a.pos.x, a.pos.x + a.dim.x);
    f32 a_max_x = db_max(a.pos.x, a.pos.x + a.dim.x);
    f32 a_min_y = db_min(a.pos.y, a.pos.y + a.dim.y);
    f32 a_max_y = db_max(a.pos.y, a.pos.y + a.dim.y);

    f32 b_min_x = db_min(b.pos.x, b.pos.x + b.dim.x);
    f32 b_max_x = db_max(b.pos.x, b.pos.x + b.dim.x);
    f32 b_min_y = db_min(b.pos.y, b.pos.y + b.dim.y);
    f32 b_max_y = db_max(b.pos.y, b.pos.y + b.dim.y);

    f32 x0 = db_max(a_min_x, b_min_x);
    f32 y0 = db_max(a_min_y, b_min_y);
    f32 x1 = db_min(a_max_x, b_max_x);
    f32 y1 = db_min(a_max_y, b_max_y);

    if ((x0 < x1) && (y0 < y1))
    {
        db_rect2 r    = db_rect2_make(db_vector2_make(x0, y0), db_vector2_make(x1 - x0, y1 - y0));
        *intersection = r;
        return 1;
    }
    else
    {
        db_rect2 r    = {0};
        *intersection = r;
        return 0;
    }
}

#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__)
static inline db_math_u64 db_hash_murmur64(void const *key, size_t num_bytes, db_math_u64 seed)
{
    db_math_u64 const m = 0xc6a4a7935bd1e995ULL;
    db_math_u64 const r = 47;

    db_math_u64 h = seed ^ (num_bytes * m);

    db_math_u64   *data = (db_math_u64 *)(key);
    db_math_u64   *end  = data + (num_bytes / 8);
    unsigned char *data2;

    while (data != end)
    {
        db_math_u64 k  = *data++;
        k             *= m;
        k             ^= k >> r;
        k             *= m;
        h             ^= k;
        h             *= m;
    }

    data2 = (unsigned char *)data;

    switch (num_bytes & 7)
    {
        case 7:
            h ^= (db_math_u64)data2[6] << 48;
        case 6:
            h ^= (db_math_u64)data2[5] << 40;
        case 5:
            h ^= (db_math_u64)data2[4] << 32;
        case 4:
            h ^= (db_math_u64)data2[3] << 24;
        case 3:
            h ^= (db_math_u64)data2[2] << 16;
        case 2:
            h ^= (db_math_u64)data2[1] << 8;
        case 1:
            h ^= (db_math_u64)data2[0];
            h *= m;
    };

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}
#else
static inline db_math_u64 db_hash_murmur64(void const *key, size_t num_bytes, db_math_u64 seed)
{
    db_math_u32 const m = 0x5bd1e995;
    db_math_u32 const r = 24;

    db_math_u64 h  = 0;
    db_math_u32 h1 = (db_math_u32)seed ^ (db_math_u32)num_bytes;
    db_math_u32 h2 = (db_math_u32)((db_math_u64)seed >> 32);

    db_math_u32 *data = (db_math_u32 *)key;

    while (num_bytes >= 8)
    {
        db_math_u32 k1, k2;
        k1         = *data++;
        k1        *= m;
        k1        ^= k1 >> r;
        k1        *= m;
        h1        *= m;
        h1        ^= k1;
        num_bytes -= 4;

        k2         = *data++;
        k2        *= m;
        k2        ^= k2 >> r;
        k2        *= m;
        h2        *= m;
        h2        ^= k2;
        num_bytes -= 4;
    }

    if (num_bytes >= 4)
    {
        db_math_u32 k1  = *data++;
        k1             *= m;
        k1             ^= k1 >> r;
        k1             *= m;
        h1             *= m;
        h1             ^= k1;
        num_bytes      -= 4;
    }

    switch (num_bytes)
    {
        db_math_u32 a, b, c;
        case 3:
            c   = data[2];
            h2 ^= c << 16;
        case 2:
            b   = data[1];
            h2 ^= b << 8;
        case 1:
            a   = data[0];
            h2 ^= a << 0;
            h2 *= m;
    };

    h1 ^= h2 >> 18;
    h1 *= m;
    h2 ^= h1 >> 22;
    h2 *= m;
    h1 ^= h2 >> 17;
    h1 *= m;
    h2 ^= h1 >> 19;
    h2 *= m;

    h = (db_math_u64)(h << 32) | (db_math_u64)h2;

    return h;
}
#endif

static inline f32 db_random_range_float(f32 min_inc, f32 max_inc)
{
    int int_result  = db_random_range_int(0, 2147483646); /* Prevent integer overflow */
    f32 result      = int_result / (f32)2147483646;
    result         *= max_inc - min_inc;
    result         += min_inc;
    return result;
}

static inline int db_random_range_int(int min_inc, int max_inc)
{
    static unsigned int random_value = 0xdeadbeef; /* Random Value */
    unsigned int        diff, result;
    random_value  = random_value * 2147001325 + 715136305; /* BCPL generator */
    diff          = max_inc - min_inc + 1;
    result        = random_value % diff;
    result       += min_inc;

    return result;
}

static inline f32 db_random01(void)
{
    return db_random_range_float(0.0f, 1.0f);
}

#if defined(__GCC__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

static inline db_vector2 db_vector2_add(db_vector2 v0, db_vector2 v1)
{
    db_vector2 result;
    db__vector2_add(&result, v0, v1);
    return result;
}
static inline db_vector2 db_vector2_subtract(db_vector2 v0, db_vector2 v1)
{
    db_vector2 result;
    db__vector2_subtract(&result, v0, v1);
    return result;
}
static inline db_vector2 db_vector2_multiply(db_vector2 v, f32 s)
{
    db_vector2 result;
    db__vector2_multiply(&result, v, s);
    return result;
}
static inline db_vector2 db_vector2_divide(db_vector2 v, f32 s)
{
    db_vector2 result;
    db__vector2_divide(&result, v, s);
    return result;
}
static inline db_vector3 db_vector3_add(db_vector3 v0, db_vector3 v1)
{
    db_vector3 result;
    db__vector3_add(&result, v0, v1);
    return result;
}
static inline db_vector3 db_vector3_subtract(db_vector3 v0, db_vector3 v1)
{
    db_vector3 result;
    db__vector3_subtract(&result, v0, v1);
    return result;
}
static inline db_vector3 db_vector3_multiply(db_vector3 v, f32 s)
{
    db_vector3 result;
    db__vector3_multiply(&result, v, s);
    return result;
}
static inline db_vector3 db_vector3_divide(db_vector3 v, f32 s)
{
    db_vector3 result;
    db__vector3_divide(&result, v, s);
    return result;
}
static inline db_vector4 db_vector4_add(db_vector4 v0, db_vector4 v1)
{
    db_vector4 result;
    db__vector4_add(&result, v0, v1);
    return result;
}
static inline db_vector4 db_vector4_subtract(db_vector4 v0, db_vector4 v1)
{
    db_vector4 result;
    db__vector4_subtract(&result, v0, v1);
    return result;
}
static inline db_vector4 db_vector4_multiply(db_vector4 v, f32 s)
{
    db_vector4 result;
    db__vector4_multiply(&result, v, s);
    return result;
}
static inline db_vector4 db_vector4_divide(db_vector4 v, f32 s)
{
    db_vector4 result;
    db__vector4_divide(&result, v, s);
    return result;
}
static inline f32 db_vector2_cross(db_vector2 v0, db_vector2 v1)
{
    f32 result;
    db__vector2_cross(&result, v0, v1);
    return result;
}
static inline db_vector3 db_vector3_cross(db_vector3 v0, db_vector3 v1)
{
    db_vector3 result;
    db__vector3_cross(&result, v0, v1);
    return result;
}
static inline db_vector2 db_vector2_normalize(db_vector2 v)
{
    db_vector2 result;
    db__vector2_normalize(&result, v);
    return result;
}
static inline db_vector3 db_vector3_normalize(db_vector3 v)
{
    db_vector3 result;
    db__vector3_normalize(&result, v);
    return result;
}
static inline db_vector4 db_vector4_normalize(db_vector4 v)
{
    db_vector4 result;
    db__vector4_normalize(&result, v);
    return result;
}
static inline db_vector2 db_vector2_normalize_or_zero(db_vector2 v)
{
    db_vector2 result;
    db__vector2_normalize_or_zero(&result, v);
    return result;
}
static inline db_vector3 db_vector3_normalize_or_zero(db_vector3 v)
{
    db_vector3 result;
    db__vector3_normalize_or_zero(&result, v);
    return result;
}
static inline db_vector4 db_vector4_normalize_or_zero(db_vector4 v)
{
    db_vector4 result;
    db__vector4_normalize_or_zero(&result, v);
    return result;
}
static inline db_vector2 db_vector2_reflect(db_vector2 i, db_vector2 n)
{
    db_vector2 result;
    db__vector2_reflect(&result, i, n);
    return result;
}
static inline db_vector3 db_vector3_reflect(db_vector3 i, db_vector3 n)
{
    db_vector3 result;
    db__vector3_reflect(&result, i, n);
    return result;
}
static inline db_vector2 db_vector2_refract(db_vector2 i, db_vector2 n, f32 eta)
{
    db_vector2 result;
    db__vector2_refract(&result, i, n, eta);
    return result;
}
static inline db_vector3 db_vector3_refract(db_vector3 i, db_vector3 n, f32 eta)
{
    db_vector3 result;
    db__vector3_refract(&result, i, n, eta);
    return result;
}
static inline db_vector2 db_vector2_lerp(db_vector2 a, db_vector2 b, f32 t)
{
    db_vector2 result;
    db__vector2_lerp(&result, a, b, t);
    return result;
}
static inline db_vector3 db_vector3_lerp(db_vector3 a, db_vector3 b, f32 t)
{
    db_vector3 result;
    db__vector3_lerp(&result, a, b, t);
    return result;
}
static inline db_vector4 db_vector4_lerp(db_vector4 a, db_vector4 b, f32 t)
{
    db_vector4 result;
    db__vector4_lerp(&result, a, b, t);
    return result;
}
static inline db_vector2 db_vector2_hadamard(db_vector2 a, db_vector2 b)
{
    db_vector2 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}
static inline db_vector3 db_vector3_hadamard(db_vector3 a, db_vector3 b)
{
    db_vector3 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}
static inline db_vector4 db_vector4_hadamard(db_vector4 a, db_vector4 b)
{
    db_vector4 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;
    return result;
}
static inline db_vector2 db_vector2_rotate(db_vector2 a, f32 degrees)
{
    f32 radians = db_to_radians(degrees);
    f32 c       = db_cos(radians);
    f32 s       = db_sin(radians);
    return db_vector2_make(a.x * c - a.y * s, a.x * s + a.y * c);
}

#endif /* DB_MATH_IMPLEMENTATION */
