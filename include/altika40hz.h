/* saral40hz.h

Structure for Saral data

*/

#define SOL 299792458.0                 /* speed of light in vacuum */

/* find center of mass correction */

struct SARAL40HZ {	/* */

	unsigned int	sec2000;	/* atomic seconds since 2000  */
	unsigned int	microsec;	/* microseconds*/
	unsigned int    kframe;		/* relative source sequence number */
	unsigned int	alt;		/* orbit altitude, COG above reference ellipsoid, mm offset 130000 m*/
	unsigned int	alt_rate;	/* orbit altitude rate, mm/s */
	unsigned int	range;		/* range, COG at Ku band, mm; (from uncorrected two-way window delay) offset 130000 m */
	unsigned int	range_c;	/* range, COG at S band, mm, offset 130000 m */
	unsigned int	trackbits;	/* Useful flags to be defined as we go along; initially zero 
						1	          1 - record empty based on no time or range record 
						2	          2 - dry land idetermined from iwetdry.c 
						1024             11 - standard deviation out of range (e.g. LRM 0.3 to 0.8)
						2048             12 - drange is out of bounds
						4096             13 - swh is out of range (e.g. LRM 1 to 10 m)
						8192             14 - retrack failed least squares
						16384            15 - amp out of range (e.g. LRM 50000 to 80000) */
	unsigned int csum; 		/* computed sum of counts of power for all range gates */

	int		lon;		/* units 10^-6 deg, 0 to 360  */
	int		lat;		/* units 10^-6 deg  */
	int		mss;		/* model mean sea surface, mm EGM2008 */
	int		agc;		/* AGC 1.e-2 dB */
	int		agc_c;		/* AGC_c  1.e-2 dB */
	int		pamp[2];	/* waveform power  estimated by retracking  */
	int		off_nadir[3];	/* square of off nadir angle from platform 1.e-4 deg */
	int		baseline[3];	/* interferometer baseline */
	
	unsigned short int surf_flag;	/* surface flag type: 0-ocean; 1-closed sea; 2-continental ice; 3-land   */
	unsigned short int pswh[2];	/* pseudo-swh, waveform spread estimated by retracking, mm  */
	unsigned short int rchisq[2];	/* root sum of squares of misfit error from tracker  */
	unsigned short int pnoise[2];	/* noise level preceding leading edge, est by retracking  */
	unsigned short int n_echo;	/* number of echoes averaged into this record */
	short int	drange[3];	/* range correction from retracking, mm  */
	short int	decay[2];	/* plateau decay rate est by retrack, prop to xi  */
	short int	beam[5];	/* std, mean, amp, skew, kurt */
	short int	hotide;		/* total tide effect on the ocean surface mm. model sol1*/
	short int	hltide;		/* load tide height, mm model sol1 */
	short int	hstide;		/* solid earth tide, mm model */
	short int	hptide;		/* height correction for pole tide, mm.  From model.  */
	short int	hiono;		/* height correction for iono delay, mm.  From GIM  model.  */
	short int	hwet;		/* height correction for wet tropo delay, mm.  From a model.  */
	short int	hdry;		/* height correction for dry tropo delay, mm.  From a model.  */
	short int	hinvb;		/* height corr for inverse barometer, mm.  From model  */
	short int	hdopp;		/* height correction for doppler effect, radial component, mm. */
	short int	new_tide;	/* new total tide model from CSR4.0, mm  */

	unsigned short int  wave[128];	/* Ka-band waveform data. Only the first 104 are used. */
                                        /* Gate 32 is the track location and the gate spacing is is 0.4656  */
      /*unsigned short int  wave_c[64];	 C-band waveform data. Not available for AltiKa */
};

