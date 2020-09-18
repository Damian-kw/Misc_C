#include <string.h>
#include "CException.h"
#include "salloc.h"
#include "cppm_receiver.h"
typedef struct cppm_receiver_struct {
	volatile uint32_t *channels;
	volatile uint32_t *channels_image;
	uint32_t previous_timer_value;
	uint8_t channels_number;
	uint8_t pulse_number;
	bool beginning_of_frame;
	bool is_new_frame_available;
} cppm_receiver_t;
STATIC bool check_for_start_of_frame(uint32_t timer_value_now,
		uint32_t timer_value_previous);
STATIC void process_pulse(cppm_receiver this, uint32_t timer_value);
STATIC void end_collecting_data(cppm_receiver this);
cppm_receiver cppm_receiver_ctor(uint32_t channels_number) {
	salloc_enable();
	cppm_receiver this;
	this = salloc(sizeof(cppm_receiver_t));
	if (is_null(this)) {
		Throw(NULL_POINTER_EXCEPTION);
	}
	this->channels_number = channels_number;
	this->channels = salloc(sizeof(uint32_t) * this->channels_number);
	if (is_null(this->channels)) {
		Throw(NULL_POINTER_EXCEPTION);
	}
	this->channels_image = salloc(sizeof(uint32_t) * this->channels_number);
	if (is_null(this->channels_image)) {
		Throw(NULL_POINTER_EXCEPTION);
	}
	salloc_disable();
	this->is_new_frame_available = false;
	this->beginning_of_frame = false;
	return this;
}
uint32_t cppm_receiver_get_channel(cppm_receiver this, uint8_t channel) {
	if (channel >= this->channels_number) {
		channel = this->channels_number - 1;
	}
	this->is_new_frame_available = false;
	return this->channels[channel];
}
void cppm_receiver_collect_data(cppm_receiver this, uint32_t timer_value) {
	if (check_for_start_of_frame(timer_value, this->previous_timer_value)) {
		this->pulse_number = 0;
		this->beginning_of_frame = true;
	} else if (this->beginning_of_frame) {
		process_pulse(this, timer_value);
	}
	this->previous_timer_value = timer_value;
	if (this->pulse_number == this->channels_number) {
		end_collecting_data(this);
	}
}
bool cppm_receiver_is_new_frame_available(cppm_receiver this) {
	return this->is_new_frame_available;
}
STATIC bool check_for_start_of_frame(uint32_t timer_value_now,
		uint32_t timer_value_previous) {
	if ((timer_value_now - timer_value_previous) > CHANNEL_GAP_IN_US) {
		return true;
	} else {
		return false;
	}
}
STATIC void process_pulse(cppm_receiver this, uint32_t timer_value) {
	this->channels[this->pulse_number] = timer_value
			- this->previous_timer_value;
	this->pulse_number++;
}
STATIC void end_collecting_data(cppm_receiver this) {
	this->beginning_of_frame = false;
	this->pulse_number = 0;
	this->is_new_frame_available = true;
	for (size_t i = 0; i < this->channels_number; i++) {
		this->channels_image[i] = this->channels[i];
	}
}
