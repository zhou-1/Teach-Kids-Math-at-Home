/*----------------------------------------------------------------------------
**                   http://www.PowerAVR.com
**                   http://www.PowerMCU.com
**--------------File Info-----------------------------------------------------
** File name:           adc.h
** Last modified Date:  2010-05-12
** Last Version:        V1.00
** Descriptions:        
**
**----------------------------------------------------------------------------
** Created by:          PowerAVR
** Created date:        2010-05-10
** Version:             V1.00
** Descriptions:        
**
**----------------------------------------------------------------------------*/

#ifndef __ADC_H
#define __ADC_H

#define ADC_VALUE_MAX 0xFFF

void     ADC_Init    (void);
void     ADC_StartCnv(void);
void     ADC_StopCnv (void);
uint32_t ADC_GetCnv  (void);
uint16_t ADC_Get (void);

#endif 
