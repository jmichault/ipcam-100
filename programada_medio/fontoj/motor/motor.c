#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include <sys/stat.h>
#include <fcntl.h>

/* direkto */
/*
 *   1 = droite
 *   2 = gauche
 *   4 = bas
 *   5 = diagonale droite-bas
 *   6 = diagonale gauche-bas
 *   8 = haut
 *   9 = diagonale droite-haut
 *   10 = diagonale gauche-haut
 */
#define MOTORO_DIR_DEKSTRA	0x1
#define MOTORO_DIR_MALDEKSTRA	0x2
#define MOTORO_DIR_MALSUPRA	0x4
#define MOTORO_DIR_SUPRA	0x8

/* ioctl cmd */
#define MOTORO_HALTI		0x1
#define MOTORO_MOVIGXI		0x3
#define MOTORO_SXTATI		0x4

struct motoro_movigxo_st {
	int direkto;
	int h_amplexo;
	int v_amplexo;
	int rapideco;
} Movigxo;
// déplacement horizontal max : ~3220
#define MAX_X 3180
// déplacement vertical max : ~1300
#define MAX_Y 1260

struct motoro_sxtato_st {
	int direkto;
	int h_amplexo;
	int v_amplexo;
	int infoH;
	int infoV;
} Sxtato;

int Fd=0;
int EstisPravalorizita = 0;
int PozicioX=0;
int PozicioY=0;

void movigxi(int dir,int ampx,int ampy, int rapideco)
{
  int cmd= MOTORO_MOVIGXI ;
  Movigxo.direkto=dir;
  Movigxo.h_amplexo=ampx;
  Movigxo.v_amplexo=ampy;
  Movigxo.rapideco=rapideco;
  ioctl(Fd, cmd,(void *) &Movigxo);
  if( dir & MOTORO_DIR_SUPRA) PozicioY += ampy;
  if( dir & MOTORO_DIR_MALSUPRA) PozicioY -= ampy;
  if( dir & MOTORO_DIR_DEKSTRA) PozicioX += ampx;
  if( dir & MOTORO_DIR_MALDEKSTRA) PozicioX -= ampx;
}

void sxtati()
{
  int cmd= MOTORO_SXTATI ;
  ioctl(Fd, cmd,(void *) &Sxtato);
}

void halti()
{
  int cmd= MOTORO_HALTI ;
  if( Sxtato.direkto & MOTORO_DIR_SUPRA) PozicioY -= Sxtato.v_amplexo;
  if( Sxtato.direkto & MOTORO_DIR_MALSUPRA) PozicioY += Sxtato.v_amplexo;
  if( Sxtato.direkto & MOTORO_DIR_DEKSTRA) PozicioX -= Sxtato.h_amplexo;
  if( Sxtato.direkto & MOTORO_DIR_MALDEKSTRA) PozicioX += Sxtato.h_amplexo;
  ioctl(Fd, cmd,(void *) &Sxtato);
  
}

void pravalorizi()
{
  int cmd= MOTORO_MOVIGXI ;
  Movigxo.direkto=MOTORO_DIR_MALDEKSTRA|MOTORO_DIR_MALSUPRA;
  Movigxo.h_amplexo=MAX_X*13/10;
  Movigxo.v_amplexo=MAX_Y*13/10;
  Movigxo.rapideco=10000;
  ioctl(Fd, cmd,(void *) &Movigxo);
  sleep(5);
  Movigxo.direkto=MOTORO_DIR_DEKSTRA|MOTORO_DIR_SUPRA;
  Movigxo.h_amplexo=20;
  Movigxo.v_amplexo=20;
  ioctl(Fd, cmd,(void *) &Movigxo);
  EstisPravalorizita = 1;
  PozicioX=0;
  PozicioY=0;
  skribiVal();

  close(Fd);
}

void skribiVal()
{
 FILE * val=fopen("/tmp/motoraValoro","w");
 fprintf(val,"%d %d %d\n",EstisPravalorizita,PozicioX,PozicioY);
 fclose(val);
}

void legiVal()
{
 FILE * val=fopen("/tmp/motoraValoro","r");
 if(!val) return;
 fscanf(val,"%d %d %d\n",&EstisPravalorizita,&PozicioX,&PozicioY);
 fclose(val);
}

void helpo()
{
  printf("    -d : direkto\n");
  printf("	S = Supra\n");
  printf("	s = Malsupra\n");
  printf("	D = Dextra\n");
  printf("	d = Maldextra\n");
  printf("	SD = Supra-Dextra\n");
  printf("	...\n");
  printf("    -a : amplexo (defaŭlte : 100)\n");
  printf("    -r : rapideco (defaŭlte : 500)\n");
  printf("    -s : ŝtato\n");
  printf("    -p : pravalorizi\n");
  printf("    -h : halti\n");
  printf("    -x : iri al x=\n");
  printf("    -y : iri al y=\n");
}

int main(int argc, char *argv[])
{
 int direkto=0;
 int amplekso = 0;
 int rapideco = 500;
 int x=-1;
 int y=-1;
  Fd = open("/dev/motor", O_WRONLY);
  legiVal();
  sxtati();

  // Parse Arguments:
  int c;
  while ((c = getopt(argc, argv, "d:a:r:hspx:y:")) != -1) {
    switch (c) {
     case 'd':
      for (int i=0; optarg[i] ; i++ )
      {
	if (!amplekso) amplekso=100;
        switch ( optarg [i] )
	{
	 case 'D' :
	  direkto |= MOTORO_DIR_DEKSTRA;
	  break;
	 case 'd' :
	  direkto |= MOTORO_DIR_MALDEKSTRA;
	  break;
	 case 'S' :
	  direkto |= MOTORO_DIR_SUPRA;
	  break;
	 case 's' :
	  direkto |= MOTORO_DIR_MALSUPRA;
	  break;
         default:
          printf("Invalid Argument %c\n", c);
	  helpo();
          exit(EXIT_FAILURE);
	}
      }
      break;
     case 'a':
      amplekso = atoi(optarg);
      break;
     case 'x':
      x = atoi(optarg);
      break;
     case 'y':
      y = atoi(optarg);
      break;
     case 'r':
      rapideco = atoi(optarg);
      break;
     case 'h':
      halti();
      break;
     case 'p':
      pravalorizi();
      break;
     case 's':
      {
       sxtati();
       printf("dir %d H %d V %d IH %d IV %d\n"
		,Sxtato.direkto
		,Sxtato.h_amplexo
		,Sxtato.v_amplexo
		,Sxtato.infoH
		,Sxtato.infoV);
       close(Fd);
       exit(0);
      }
     default:
      printf("Invalid Argument %c\n", c);
      helpo();
      exit(EXIT_FAILURE);
    }
  }
  if(amplekso)
  {
   if( EstisPravalorizita
	&& (direkto & MOTORO_DIR_SUPRA)
	&& amplekso > (MAX_Y - PozicioY))
    amplekso = (MAX_Y - PozicioY);
   if( EstisPravalorizita
	&& (direkto & MOTORO_DIR_MALSUPRA)
	&& amplekso > PozicioY)
    amplekso = PozicioY;
   if( EstisPravalorizita
	&& (direkto & MOTORO_DIR_DEKSTRA)
	&& amplekso > (MAX_X - PozicioX))
    amplekso = (MAX_X - PozicioX);
   if( EstisPravalorizita
	&& (direkto & MOTORO_DIR_MALDEKSTRA)
	&& amplekso > PozicioX)
    amplekso = PozicioX;
   movigxi(direkto,amplekso,amplekso,rapideco);
  }
  if( x>=0  || y>=0 )
  { 
    if (x> MAX_X) x=MAX_X;
    if (y> MAX_Y) y=MAX_Y;
    if(!EstisPravalorizita) pravalorizi();
    int deltax= x-PozicioX;
    int deltay= y-PozicioY;
    direkto = 0;
    if(deltax>0) direkto|=MOTORO_DIR_DEKSTRA;
    else if(deltax<0) direkto|=MOTORO_DIR_MALDEKSTRA;
    if(deltay>0) direkto|=MOTORO_DIR_SUPRA;
    else if(deltay<0) direkto|=MOTORO_DIR_MALSUPRA;
    movigxi(direkto,abs(deltax),abs(deltay),rapideco);
  }
  close(Fd);
  skribiVal();
}
