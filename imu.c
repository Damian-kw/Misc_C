#include <math.h>
#include "imu.h"
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif
STATIC float convert_radians_to_degrees(float radians);
float imu_get_pitch_angle(float accel_x, float accel_y, float accel_z)
{
    float magnitude = sqrtf(accel_y * accel_y) + (accel_z * accel_z);
    float angle = convert_radians_to_degrees(atanf(accel_x / magnitude));
    return angle;
}
float imu_get_roll_angle(float accel_x, float accel_y, float accel_z)
{
    float magnitude = sqrtf((accel_x * accel_x) + (accel_z * accel_z));
    float angle=convert_radians_to_degrees(atanf(accel_y/magnitude));
    return angle;
}
STATIC float convert_radians_to_degrees(float radians)
{
    return radians * 57.3f;
}