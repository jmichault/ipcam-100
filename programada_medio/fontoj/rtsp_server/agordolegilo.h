#include <stdbool.h>

struct sensilo_agordo
{
  short cbus_type; // 1=I2C , 2=SPI
  char name[32];
  int addr;
  short width;
  short heigth;
};

struct kanalo_agordo
{
  short picWidth;
  short picHeight;
  short format; //
  short minQP;
  short maxQP;
  int bitrate;
  short fpnum;
};

#ifdef __cplusplus
extern "C"
{
#endif

void agordoLegilo();

extern struct sensilo_agordo SenAgordo;
extern struct kanalo_agordo KanAgordo[2];
extern bool AgordoNeedReset;

#ifdef __cplusplus
}
#endif
