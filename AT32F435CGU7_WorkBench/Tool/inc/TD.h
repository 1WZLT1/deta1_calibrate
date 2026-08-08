#ifndef _TD_H_
#define _TD_H_
#include "stdint.h"

typedef struct
{
/*****安排过度过程*******/
float v1;//跟踪微分期状态量
float v2;//跟踪微分期状态量微分项
float r0;//时间尺度
float h;//ADRC系统积分时间
float h0;//h0=n*h,n>=1
}TD_Param;

extern void adrc_td_init(TD_Param* td_t, float h, float r0, float h0);
extern void adrc_td(TD_Param* td, float v);

#endif
