#ifndef _CPPM_RECEIVER_H
#define _CPPM_RECEIVER_H
#define NULL_POINTER_EXCEPTION 1
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif
#include <stdbool.h>
#define CHANNEL_GAP_IN_US 2050
typedef struct cppm_receiver_struct *cppm_receiver;
cppm_receiver cppm_receiver_ctor(uint32_t channels_number);
uint32_t cppm_receiver_get_channel(cppm_receiver this, uint8_t channel);
void cppm_receiver_collect_data(cppm_receiver this, uint32_t timer_value);
bool cppm_receiver_is_new_frame_available(cppm_receiver this);
#endif // _CPPM_RECEIVER_H
