#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif
struct S_movAgordo{
bool Aktivida;
int Sentemo;
int FinaTempo;
int SenaktivaTempo;
int MinZonoj;
int MaxZonoj;
bool StokPicOn;
bool StokMovOn;
bool SciiMailOn;
char * SciiMailAl;
bool SciiTelOn;
char * SciiTelAl;
bool SciiUrlOn;
char * SciiUrlUrl;
bool SciiOrdOn;
char * SciiOrdOrd;
};
extern struct S_movAgordo movAgordo;

void movoLegilo();

#ifdef __cplusplus
}
#endif
