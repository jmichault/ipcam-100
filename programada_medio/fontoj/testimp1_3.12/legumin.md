
sample-common.c : komunaj funkcioj

getsnap.c : capture une image vers la sortie standard
	fonctionne, mais crashe souvent en fin de programme


lit_infrarouge.c : lit la valeur du capteur CMOS (en bas à gauche de la caméra)
	fonctionne OK.

sample-Encoder-jpeg.c
   fonctionne.

sample-Audio.c
  l'enregistrement marche un peu, mais il y a des coupures.
    essai de lecture avec : ffplay -f s16le -ar 8k -ac 1 record_file.pcm
  la lecture provoque des grésillements et c'est tout.

sample-Encoder-h264.c
  tourne, mais le résultat n'est pas visible.



