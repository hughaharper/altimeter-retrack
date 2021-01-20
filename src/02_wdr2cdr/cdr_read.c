/************************************************************************
 ***********************************************************************/
/************************************************************************
 * Creator: Hugh Harper                                                 *
 * Date   : 02/13/2019                                                  *
 ************************************************************************/
/************************************************************************
 * Modification History:                                                 *
 *                                                                       *
 * Date   :                                                             *
 *                                                                       *
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cdr.h"

/* void read_cdr(FILE *, struct CDR *, int nd); */
/*int read_cdr(FILE *, struct CDR *);
void write_cdr(FILE *, struct CDR *, int nd); */

int main(int argc, char **argv) {
    struct CDR cdr_in;
    struct CDR_HEAD cdr_head;
    struct CDR *cdr_out;
    FILE *infile, *outfile;
    int isat, iasc, ifile;
    int nd_in, nd_out, np_max;
    int k;
    float dt;

    if ((argc != 3)) {
            fprintf(stderr, "\n");
            fprintf(stderr, "Usage: cdr_read file_in.cdr file_out.cdr\n");
            fprintf(stderr, "\n");
            return (0);
    }

    if ((infile = fopen(argv[1], "rb")) == NULL)
            die("Can't open ", argv[1]);
    outfile = fopen(argv[2], "w");

	/* read the first record of the file to determine type, asc, ifile, dt */
    /* printf("skip a few records ahead in the file\n");
    fseek(infile,28,SEEK_SET);*/
    printf("reading first record of file\n");
	fread(&cdr_head,sizeof(struct CDR_HEAD),1,infile);
    /* .asc, .des, or .int, are integer CDRs and so must be read as ints, *.edt are reals */ 
    /* fscanf(infile,"%f %f %f %f %f %f %f",&isat,&iasc,&ifile,&idtt,&junk_a,&junk_b,&junk_c); */
    
	/* write this same information to the output file */
    printf("writing first record to output file\n");
	fwrite(&cdr_head,sizeof(struct CDR_HEAD),1,outfile);

    printf("extract needed params from the header\n");
	/* extract the needed parameters from the file header */
    printf("isat: %d, iasc: %d, ifile: %d, dt: %d, junk: %d, junk: %d, junk: %d\n",cdr_head.isat,cdr_head.iasc,cdr_head.ifile,cdr_head.idt,cdr_head.junk_1,cdr_head.junk_2,cdr_head.junk_3);

    /* because struct is 32 bytes due to padding, need to make the record size only 28 bytes */
    while(fread(&cdr_in,28,1,infile) == 1){
        printf("%lf, %f, %f, %f, %f, %f\n",cdr_in.time_1,cdr_in.lat,cdr_in.lon,cdr_in.sh,cdr_in.dsh,cdr_in.cor);
        /*k++;
        fseek(infile,28*k,SEEK_SET); */
    }


    /*read another record to display */

	/* malloc enough memory for the longest possible pass */
    /*
	np_max = (int)4000./dt;
    cdr_in = (struct CDR *)malloc(sizeof(struct CDR) * np_max);
    cdr_out = (struct CDR *)malloc(sizeof(struct CDR) * np_max);
    */
	/* read an entire pass */
    /*while ( (nd_in = read_cdr(infile, cdr_in)) > 0 ) { */
	/* do something */
    /*
	    for (k=0;k<nd_in;k++){
			cdr_out[k].time = cdr_in[k].time;
		}
		write_cdr(outfile, cdr_out, nd_in);
	}
    */
    fclose(infile);
    fclose(outfile);
    exit(0);
}
