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
#define MOTORO_MOVIGXI		0x3

struct motoro_movigxo_st {
	int direkto;
	int h_amplexo;
	int v_amplexo;
	int rapideco;
};

void movigxi(int dir,int amp, int rapideco)
{
  int cmd= MOTORO_MOVIGXI ;
  int fd = open("/dev/motor", O_WRONLY);
  struct motoro_movigxo_st movigxo;
  movigxo.direkto=dir;
  movigxo.h_amplexo=amp;
  movigxo.v_amplexo=amp;
  movigxo.rapideco=rapideco;
  ioctl(fd, cmd,(void *) &movigxo);
  close(fd);
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
  printf("    -r : rapideco (defaŭlte : 1000)\n");
}

int main(int argc, char *argv[])
{
 int direkto=0;
 int amplekso = 100;
 int rapideco = 1000;

  // Parse Arguments:
  int c;
  while ((c = getopt(argc, argv, "d:a:r:")) != -1) {
    switch (c) {
     case 'd':
      for (int i=0; optarg[i] ; i++ )
      {
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
     case 'r':
      rapideco = atoi(optarg);
      break;
     default:
      printf("Invalid Argument %c\n", c);
      helpo();
      exit(EXIT_FAILURE);
    }
  }
  movigxi(direkto,amplekso,rapideco);
}
