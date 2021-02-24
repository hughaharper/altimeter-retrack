#include "cdr.h"
#include <stdio.h>
#include <stdlib.h>

/* void read_cdr(FILE *, struct CDR *, nd_in); */
int read_cdr(FILE *cdrfile, struct CDR_HEAD *cdr) {
	int n;
	int isat, iasc, ifile, idt;
	float time, lat, lon, ht, sh, dsh, cor;

	/* open each cdrfile and read into structure r */
  /* just reads the first line which has some different values */
	fscanf(cdrfile, "%d %d %d %d", &isat, &iasc, &ifile, &idt);
  printf("isat: %d iasc: %d ifile: %d dt: %d \n",isat,iasc,ifile,idt);
	cdr->isat = isat;
	cdr->iasc = iasc;
	cdr->ifile = ifile;
	cdr->idt = idt;

  fseek(cdrfile,28,SEEK_SET);
	cdr->points = (struct CDR *)malloc(20000 * sizeof(struct CDR));
	n = 0;
	while (fscanf(cdrfile, "%f %f %f %f %f %f %f", &time, &lat, &lon, &ht, &sh, &dsh, &cor) != EOF) {
		cdr->points[n].time = time;
		cdr->points[n].lat = lat;
		cdr->points[n].lon = lon;
		cdr->points[n].ht = ht;
		cdr->points[n].sh = sh;
		cdr->points[n].dsh = dsh;
		cdr->points[n].cor = cor;
		n++;
	}

	fprintf(stderr,"debugging the readleader file\n");
	for (n=0; n<10; n++){
		fprintf(stderr, "%d %f %f %f %f %f %f %f\n",n,cdr->points[n].time, cdr->points[n].lat, \
	  	cdr->points[n].lon,cdr->points[n].ht,cdr->points[n].sh,cdr->points[n].dsh,cdr->points[n].cor);
	}
  return n;
}
