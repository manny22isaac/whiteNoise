#include "userosc.h"
#include "fixed_math.h"
#include "int_math.h"
#include "fixed_math.h"

typedef struct State {
	float angle;
	float duty;
	float phase;
	float w0;
	uint8_t flags;
};

static State s_state;

enum {
  k_flags_none = 0,
  k_flag_reset = 1<<0,
};

void OSC_INIT(uint32_t platform, uint32_t api)

{
	s_state.duty = 0.f;
	s_state.angle = 0.f;
	s_state.w0 = 0.f;
	s_state.phase = 0.f;
	s_state.flags = k_flags_none;
}

void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames) 
{
	const uint8_t flags = s_state.flags;
	s_state.flags = k_flags_none;

	const float w0 = s_state.w0 = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF);
	float phase = (flags & k_flag_reset) ? 0.f : s_state.phase;

  const float duty = s_state.duty;
  const float angle = s_state.angle;

	q31_t * __restrict y = (q31_t *)yn;
	const q31_t * y_e = y + frames;

	for (; y != y_e; ) {
		float sig = _osc_white();

		*(y++) = f32_to_q31(sig);

		phase += w0;
		phase -= (uint32_t)phase;

	}
	s_state.phase = phase;
}

void OSC_NOTEON(const user_osc_param_t * const params)
{
	s_state.flags |= k_flag_reset;
}

void OSC_NOTEOFF(const user_osc_param_t * const params)
{
	(void)params;
}

void OSC_PARAM(uint16_t index, uint16_t(value))
{
	const float valf = param_val_to_f32(value);

	switch (index) {
  case k_user_osc_param_id1:
  case k_user_osc_param_id2:
  case k_user_osc_param_id3:
  case k_user_osc_param_id4:
  case k_user_osc_param_id5:
  case k_user_osc_param_id6:
    break;
  case k_user_osc_param_shape:
    s_state.duty = 0.1f + valf * 0.8f;
    break;
  case k_user_osc_param_shiftshape:
    s_state.angle = 0.8f * valf;
    break;
  default:
    break;	
  }
}




