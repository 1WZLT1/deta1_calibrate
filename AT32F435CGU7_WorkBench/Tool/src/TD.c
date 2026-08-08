/*
 * File      : adrc.c
 *
 *
 * Change Logs:
 * Date           Author       	Notes
 * 2018-03-18     zoujiachi   	the first version
 */
#include "TD.h"
#include <math.h>

static float adrc_sign(float val)
{
	if(val >= 0.0f)
		return 1.0f;
	else
		return -1.0f;
}

float adrc_fhan(float v1, float v2, float r0, float h0)
{
	float d = h0 * h0 * r0;
	float a0 = h0 * v2;
	float y = v1 + a0;
	float a1 = sqrtf(d*(d + 8.0f*fabsf(y)));
	float a2 = a0 + adrc_sign(y)*(a1-d)*0.5f;
	float sy = (adrc_sign(y+d) - adrc_sign(y-d))*0.5f;
	float a = (a0 + y - a2)*sy + a2;
	float sa = (adrc_sign(a+d) - adrc_sign(a-d))*0.5f;
	
	return -r0*(a/d - adrc_sign(a))*sa - r0*adrc_sign(a);
}

void adrc_td_init(TD_Param* td_t, float h, float r0, float h0)
{
	td_t->h = h;
	td_t->r0 = r0;
	td_t->h0 = h0;
	td_t->v1 = td_t->v2 = 0.0f;
}

void adrc_td(TD_Param* td, float v)
{
	float fv = adrc_fhan(td->v1 - v, td->v2, td->r0, td->h0);
	
	td->v1 += td->h * td->v2;
	td->v2 += td->h * fv;
}
