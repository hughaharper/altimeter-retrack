/* cdr.h */
/* structure to hold a condensed data record (CDR) */

struct CDR {
	double time_1;
	float lat;
	float lon;
	float sh;
	float dsh;
	float cor;
};

struct CDR_HEAD {
    int isat;
    int iasc;
    int ifile;
    int idt;
    int junk_1;
    int junk_2;
    int junk_3;
};
