#ifndef _BUZZER_H
#define _BUZZER_H
#define NULL_POINTER_EXCEPTION 1
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif
typedef struct buzzer_struct *buzzer;
buzzer buzzer_ctor(void(*buzzer_on)(),void(*buzzer_off)());
void buzzer_beep( buzzer this, uint32_t duration_ms);
void buzzer_decrement_duration(buzzer this);
#endif // _BUZZER_H
