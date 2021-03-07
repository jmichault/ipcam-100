#include <stdio.h>
#include <stdint.h>

#include <imp/imp_log.h>
#include <sysutils/su_adc.h>

/* lecture de l'infrarouge : */
int SU_ADC_system_init()
{
  SU_ADC_Init();
  return SU_ADC_EnableChn(0);
}

int SU_ADC_system_exit()
{
  SU_ADC_DisableChn(0);
  return SU_ADC_Init();
}

int main(int argc, char *argv[])
{
  int val, ret;
  ret = SU_ADC_system_init();
  ret = SU_ADC_GetChnValue(0,&val);
  printf("infrarouge lu = %d\n",val);
  ret = SU_ADC_system_exit();
}
