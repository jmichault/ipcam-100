#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_framesource.h>
#include <imp/imp_isp.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_ivs.h>
#include <imp/imp_ivs_move.h>

#define SENSOR_CUBS_TYPE        TX_SENSOR_CONTROL_INTERFACE_I2C
#define TAG "testimp"

int main(int argc, char **argv)
{
    int ret = 0;
    if(argc<2)
    {
      printf("uzado : %s addr nomo\n",argv[0]);
      printf("  ekz. : %s 55 gc2053\n",argv[0]);
      exit(0);
    }

    IMPSensorInfo sensor_info[2];
    char * sensorName=argv[2];

    memset(&sensor_info, 0, sizeof(sensor_info));
    memcpy(sensor_info[0].name, sensorName, strlen(sensorName));
    sensor_info[0].cbus_type = SENSOR_CUBS_TYPE;
    memcpy(sensor_info[0].i2c.type, sensorName, strlen(sensorName));
    sensor_info[0].i2c.addr = atoi(argv[1]);
    sensor_info[0].i2c.i2c_adapter_id = 0;

    ret = IMP_ISP_Open();
    if (ret < 0) {
        printf("failed to open ISP\n");
        return -1;
    }
    else
        printf("open ISP : OK\n");
      


    ret = IMP_ISP_AddSensor(&sensor_info[0]);
    if (ret < 0) {
        printf("failed to AddSensor\n");
        exit(-1);
        return -1;

    }
    else
        printf("AddSensor : OK\n");




}
