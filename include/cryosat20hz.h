/* cryosat20hz.h
Structure for Cryosat data
*/
#ifndef cryosat20hz_h
#define cryosat20hz_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netcdf.h>

#define I4NaN 2147483647    /* If an int is set to this it means it is NaN */
#define I2NaN 32767         /* If a short int is set to this it means it is NaN */
#define SOL 299792458.0                 /* speed of light in vacuum */

/* find center of mass correction */

struct CRYOSAT20HZ{	/* */

	unsigned int	sec2000;	/* atomic seconds since 2000  */
	unsigned int	microsec;	/* microseconds*/
	unsigned int  kframe;		/* source sequence number 0-16383 then cycles to 0*/
	unsigned int	alt;		/* orbit altitude, COG above reference ellipsoid, mm */
	unsigned int	alt_rate;	/* orbit altitude rate, mm/s */
	unsigned int	range;		/* range, COG at Ku band, mm; (Cryosat from uncorrected two-way window delay) */
	unsigned int	range_s;	/* range, COG at S band, mm */
	unsigned int	trackbits;	/* Useful flags to be defined as we go along; initially zero
						1			      1 - record empty based on no time record
						2			      2 - dry land idetermined from iwetdry.c
						1024             11 - standard deviation out of range (e.g. LRM 0.3 to 0.8)
						2048             12 - drange is out of bounds
						4096             13 - swh is out of range (e.g. LRM 1 to 10 m)
						8192             14 - retrack failed least squares
						16384            15 - amp out of range (e.g. LRM 50000 to 80000) */
	unsigned int csum; 		/* computed sum of counts of power for all range gates */

	int		lon;		/* units 10^-6 deg, 0 to 360  */
	int		lat;		/* units 10^-6 deg  */
	int		mss;		/* model mean sea surface, mm EGM2008 */
	int		esf_A;		/* echo scale factor A */
	int		esf_B;		/* echo scale factor B - echo_power in watts = waveform_counts*(A*10^-9)*2^B */
	int		pamp[2];	/* waveform power  estimated by retracking  */
	int		beam_dir[3];	/* beam direction vector in micrometers */
	int		baseline[3];	/* interferometer baseline */

	unsigned short int surf_flag;	/* surface flag type: 0-ocean; 1-closed sea; 2-continental ice; 3-land   */
	unsigned short int pswh[2];	/* pseudo-swh, waveform spread estimated by retracking, mm  */
	unsigned short int rchisq[2];	/* root sum of squares of misfit error from tracker  */
	unsigned short int pnoise[2];	/* noise level preceding leading edge, est by retracking  */
	unsigned short int n_echo;	/* number of echoes averaged into this record */
	short int	drange[3];	/* range correction from retracking, mm  */
	short int	decay[2];	/* plateau decay rate est by retrack, prop to xi  */
	short int	beam[5];	/* std, mean, amp, skew, kurt */
	short int	hotide;		/* (pure, only) ocean tide height, mm. */
	short int	hltide;		/* load tide height, mm */
	short int	hstide;		/* solid earth tide, mm */
	short int	hptide;		/* height correction for pole tide, mm.  From model.  */
	short int	hiono;		/* height correction for iono delay, mm.  From a model.  */
	short int	hwet;		/* height correction for wet tropo delay, mm.  From a model.  */
	short int	hdry;		/* height correction for dry tropo delay, mm.  From a model.  */
	short int	hinvb;		/* height corr for inverse barometer, mm.  From model  */
	short int	hdopp;		/* height correction for doppler effect, radial component, mm. */
	short int	new_tide;	/* new total tide model from CSR4.0, mm  */

	unsigned short int	wave[128];	/* Ku-band waveform data.  */
      /*unsigned short int	wave_s[64];	 S-band waveform data. Not available for CryoSAT */
};
#endif /* cryosat20hz_h */
