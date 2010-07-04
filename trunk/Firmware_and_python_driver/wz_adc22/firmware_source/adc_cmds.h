#include <stdint.h>
#include "common_types.h"
#include <pic18fregs.h>
/*
  This file defines commands used by the ADC
  1) Set sampling parameters:
     Sampling frequency, and list of sampled channels.
  2) Start sampling
  3) Stop sampling

  
 */

#define ADC_ID (0)
#define ADC_CONF (1)
#define ADC_START (2)
#define ADC_STOP (3)
#define ADC_QUERY (4)
#define ADC_READCFG (5)

void adc_start(void);
void adc_init(void);
void adc_stop(void);
void adc_dispatch(void);
void adc_reset(void);

typedef struct {
  uchar freq[3];
  uchar chns[8];
} adc_cfg_t ;

#define ADC_CONF_SIZE (sizeof(adc_cfg_t))

extern adc_cfg_t adc_cfg;
extern int16_t adc_int_nr;
typedef union {
  struct {
  unsigned adc_sampling:1;
  unsigned adc_overrun:1;
  };
} adc_flags_t;
extern adc_flags_t adc_flags;
