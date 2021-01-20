/*  s3ab20hz.h
    For reading Sentinel 3A or 3B data and dumping the 20Hz values we want.
    Created by Walter H F Smith on Tuesday 30 July 2019.
*/

#ifndef s3ab20hz_h
#define s3ab20hz_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netcdf.h>

#define I4NaN 2147483647    /* If an int is set to this it means it is NaN */
#define I2NaN 32767         /* If a short int is set to this it means it is NaN */

/*  Note on the altitude and range units:
 The data variables called alt_20_ku, tracker_range_20_ku, and range_*_20_ku
 in struct S3AB20HZ use the same scale_factor = 0.0001 and add_offset = 700000
 as are used in the S3A/B *_measurement.nc files. This means that the total
 length is reduced by 700 km, then the remaining length in meters is multiplied
 by 10 000 and the result rounded to an integer and stored (or I4NaN is stored
 if the data is missing).
 To create the total range or height in meters in floating point, do this:
 double_value_in_meters = 700000.0 + 0.0001 * integer_value_in_struct_S3AB20HZ;
 But note that to create a height above ellipsoid as (orbit height) - (range)
 the constants of 700 km will cancel, and the expression evaluates most accurately
 if computed as:
 double_h_above_e_in_meters = 0.0001 * (alt_20_ku - range_20_ku). */

/*  Note on the corrections to range:
    The net instrumental range corrections are already applied to range_*_20_ku.
    This includes the Doppler correction to range. Do not apply this again when
    making the CDR.  */

struct S3AB20HZ {
    int    utcsec2000; /* S3 products give UTC time, not Atomic time */
    int    microsec;
    int    lon_20_ku;   /* Longitude degrees E times 1.0e06 */
    int    lat_20_ku;   /* Latitude degrees N times 1.0e06 */
    int    krecord;
    int    alt_20_ku;  /* Altitude of s/c COM above ref ellipsoid */
    int    range_ocean_20_ku;  /* SAR: Range from ocean retracker */
    int    range_ocog_20_ku;  /* SAR: Range from OCOG retracker */
    int    mss;     /* Units 0.1 mm, DTU DNSC 15 */
    int    mqe;     /* 'Mean quadratic error between waveform and model */
    int    flagbits;    /* 0 = ok, 1=range_ocean bad, 2=sig0_ocean bad, 4=swh_ocean bad */
    short int   surf_type_1;    /* Derived from model mask (?) 0=ocean,1=lake,2=contl ice,3=land */
    short int   surf_type_2;    /* Derived from altimeter 0=ocean,1=sea ice,2=lead ice,3=unclassified */
    short int   alt_rate;  /* Units are cm/sec */
    short int   swh;  /* SWH from Ocean SAR retracker, mm */
    short int   sig0;  /* 'corrected ocean backscatter coefficient, 0.01dB */
    short int   sparei2[3];  /* Round out structure to 60 bytes total size */
};

#endif /* s3ab20hz_h */
