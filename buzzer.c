#include "CException.h"
#include "salloc.h"
#include <stdint.h>
#include "buzzer.h"
typedef struct buzzer_struct
{
    uint32_t duration_ms;
    void (*buzzer_on)();
    void (*buzzer_off)();
} buzzer_t;
STATIC void buzzer_on(buzzer this);
STATIC void buzzer_off(buzzer this);
buzzer buzzer_ctor(void (*buzzer_on)(), void (*buzzer_off)())
{
    salloc_enable();
    buzzer this;
    this = salloc(sizeof(buzzer_t));
    if (is_null(this))
    {
        Throw(NULL_POINTER_EXCEPTION);
    }
    this->buzzer_on = buzzer_on;
    this->buzzer_off = buzzer_off;
    buzzer_off(this);
    salloc_disable();
    return this;
}
void buzzer_beep(buzzer this, uint32_t duration_ms)
{
    this->duration_ms = duration_ms;
    buzzer_on(this);
}
void buzzer_decrement_duration(buzzer this)
{
    this->duration_ms--;
    if (this->duration_ms <= 0)
    {
        this->duration_ms = 0;
        buzzer_off(this);
    }
}
STATIC void buzzer_on(buzzer this)
{
    this->buzzer_on();
}
STATIC void buzzer_off(buzzer this)
{
    this->buzzer_off();
}
