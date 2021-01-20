/*  cryosat20hz.c

 For reading Cryosat-2 Baseline-D data and dumping the 20Hz values we want.
 Created by Walter H F Smith on Tuesday 30 July 2019.
 Modified by H Harper 20 Feb 2019
 */

#include "cryosat20hz.h"
#include <netcdf.h>

size_t    handle_one_file (char *fname);

int main (int argc, char **argv) {

    size_t k, n_out = 0;
    for (k = 1; k < argc; k++) {
        n_out += handle_one_file (argv[k]);
    }
    if (n_out == 0) {
        fprintf (stderr, "usage: cryosat20hz file1.nc file2.nc ... > output_binary_cryosat20hz_structures\n");
        exit (EXIT_FAILURE);
    }
    fprintf (stderr, "cryosat20hz wrote %zu records to stdout.\n", n_out);
    exit (EXIT_SUCCESS);
}

size_t    handle_one_file (char *fname) {

    /* Returns number of records successfully processed. */

    char varname[256];
    struct CRYOSAT20HZ *data;
    int *i;
    long long *l;
    short int *h;
    unsigned int *u;
    unsigned short int *us;
    char *s;
    double  *t;
    void    *work;
    size_t  retval = 0, n20hz_ku = 0, ncor_01 = 0, k, j;
    size_t  n20hz_wfm = 0;
    int     nc_err, ncfid, ncvid, ncdimid;

    /* Open the file: */
    nc_err = nc_open (fname, NC_NOWRITE, &ncfid);
    if (nc_err != NC_NOERR) {
        fprintf (stderr, "Failed to open %s\n", fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        return (retval);
    }

    fprintf (stderr, "Opened %s\n", fname);

    /* Ask if it has a dimension called "time_20_ku" and get the dimension ID: */
    nc_err = nc_inq_dimid (ncfid, "time_20_ku", &ncdimid);
    if (nc_err != NC_NOERR) {
        fprintf (stderr, "Failed to get time_20_ku dimension ID in %s\n", fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        return (retval);
    }

    /* Find out how many 20hz ku records the file has */
    nc_err = nc_inq_dimlen (ncfid, ncdimid, &n20hz_ku);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get time_20_ku dimension length in %s\n", fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        return (retval);
    }

    /* Ask if it has a dimension called "time_cor_ku" and get the dimension ID: */
    nc_err = nc_inq_dimid (ncfid, "time_cor_01", &ncdimid);
    if (nc_err != NC_NOERR) {
        fprintf (stderr, "Failed to get time_cor_01 dimension ID in %s\n", fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        return (retval);
    }

    /* Find out how many 01 cor records the file has */
    nc_err = nc_inq_dimlen (ncfid, ncdimid, &ncor_01);
    if ( (nc_err != NC_NOERR) || (ncor_01 == 0) ) {
        fprintf (stderr, "Failed to get time_cor_01 dimension length in %s\n", fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        return (retval);
    }

    /* ask if it has the waveform dimension and get the ID */
    nc_err = nc_inq_dimid (ncfid, "ns_20_ku", &ncdimid);
    if (nc_err != NC_NOERR) {
        fprintf (stderr, "Failed to get ns_20_ku dimension ID in %s\n", fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        return (retval);
    }

    /* find out the length of the waveform dimension */
    nc_err = nc_inq_dimlen (ncfid, ncdimid, &n20hz_wfm);
    if ( (nc_err != NC_NOERR) || (n20hz_wfm == 0) ) {
        fprintf (stderr, "Failed to get ns_20_ku dimension length in %s\n", fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        return (retval);
    }

    /* Allocate some workspace:  */
    data = (struct CRYOSAT20HZ *) malloc (n20hz_ku * sizeof(struct CRYOSAT20HZ));
    if (data == NULL) {
        fprintf (stderr, "Failed to malloc data structure for %s\n", fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        return (retval);
    }

    work = malloc (4 * 128 * n20hz_ku); /* Enough space to hold waveforms also */
    if (work == NULL) { /* changed data == Null to work, think was a mistake -hugh  */
        fprintf (stderr, "Failed to malloc workspace for %s\n", fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        return (retval);
    }

    /* Load time and convert from real*8 to our types: */
    strcpy (varname, "time_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
       return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    t = (double *)work; /* in netcdf, time is a double but we will store as uint*/
    for (k = 0; k < n20hz_ku; k++) {
        if (isnan(t[k])) {
            data[k].sec2000 = I4NaN;
            data[k].microsec = I4NaN;
        } else {
            data[k].sec2000 = floor(t[k]); /* double to unsigned int */
            data[k].microsec = floor(1000000.0 * (t[k] - data[k].sec2000));
        }
    }

    /* Load lat data */
    strcpy (varname, "lat_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    i = (int *)work;
    for (k = 0; k < n20hz_ku; k++) data[k].lat = (int)floor(i[k]/10. + .5);

    /* load lon data */
    strcpy (varname, "lon_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) {
        data[k].lon = (int)floor(i[k]/10. + .5);
        if (data[k].lon < 0) data[k].lon = data[k].lon + 360000000;
    }
    /* altitude of CoM above ref ellipsoid */
    strcpy (varname, "alt_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].alt = i[k]; /* int to unsigned int */

    /* Calibrated 2 way window delay, "range", needs scaling */
    strcpy (varname, "window_del_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    l = (long long *)work;
    for (k = 0; k < n20hz_ku; k++) data[k].range = (unsigned int) floor(0.5 + l[k]*1.e-9*SOL/2.);

    /* Source sequence counter, kframe */
    strcpy (varname, "rec_count_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].kframe = i[k]; /* int to unsigned int */

    /* altitude rate */
    strcpy (varname, "orb_alt_rate_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].alt_rate = i[k];

    /* echo scale factor */
    strcpy (varname, "echo_scale_factor_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].esf_A = i[k];

    /* echo scale power */
    strcpy (varname, "echo_scale_pwr_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].esf_B = i[k];

    /* AGC gain */
    strcpy (varname, "agc_ch1_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].pamp[0] = i[k];

    /* Doppler height correction */
    strcpy (varname, "dop_cor_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].pamp[1] = i[k];

    /* baseline, off-nadir roll angle */
    strcpy (varname, "off_nadir_roll_angle_str_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].baseline[0] = i[k];

    /* baseline, off-nadir pitch angle */
    strcpy (varname, "off_nadir_pitch_angle_str_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].baseline[1] = i[k];

    /* baseline, off-nadir yaw angle */
    strcpy (varname, "off_nadir_yaw_angle_str_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].baseline[2] = i[k];

    /* Surface type flag, this has a different dimension from n20hz_ku, so need to fix it */
    strcpy (varname, "surf_type_01");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    fprintf (stderr, "loading surf flag\n");
    s = (char *)work;
    for (k = 0; k < n20hz_ku; k++) {
        j = (int)floor( (k+1) / 20. );
        data[k].surf_flag = (unsigned short)s[j];
    }

    /* for (k = 0; k < ncor_01; k++) {
	fprintf (stderr, "surf flag: %d\n", (unsigned short)s[k]);
        for (j = 0; j < 20; j++) {
            data[k * 20 + j].surf_flag = (unsigned short)s[k];
        }
    } */

    fprintf (stderr, "got surf flag\n");

    /* number of average echoes */
    strcpy (varname, "echo_numval_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    u = (unsigned int *)work;
    for (k = 0; k < n20hz_ku; k++) data[k].n_echo = u[k];

    /* ocean tide */
    strcpy (varname, "ocean_tide_01");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    h = (short int *)work;
    for (k = 0; k < n20hz_ku; k++) {
        j = (int)floor( (k+1) / 20. );
        data[k].hotide = h[j];
    }
    /*for (k = 0; k < ncor_01; k++) {
        for (j = 0; j < 20; j++) {
            data[k * 20 + j].hotide = h[k];
        }
    }*/

    /* ocean loading tide */
    strcpy (varname, "load_tide_01");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) {
        j = (int)floor( (k+1) / 20. );
        data[k].hltide = h[j];
    }
    /*for (k = 0; k < ncor_01; k++) {
        for (j = 0; j < 20; j++) {
            data[k * 20 + j].hltide = h[k];
        }
    }*/

    /* solid earth tide */
    strcpy (varname, "solid_earth_tide_01");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) {
        j = (int)floor( (k+1) / 20. );
        data[k].hstide = h[j];
    }
    /*for (k = 0; k < ncor_01; k++) {
        for (j = 0; j < 20; j++) {
            data[k * 20 + j].hstide = h[k];
        }
    }*/

    /* polar geocentric tide */
    strcpy (varname, "pole_tide_01");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) {
        j = (int)floor( (k+1) / 20. );
        data[k].hptide = h[j];
    }
    /*for (k = 0; k < ncor_01; k++) {
        for (j = 0; j < 20; j++) {
            data[k * 20 + j].hptide = h[k];
        }
    }*/

    /* ionospheric correction */
    strcpy (varname, "iono_cor_01");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) {
        j = (int)floor( (k+1) / 20. );
        data[k].hiono = h[j];
    }
    /*for (k = 0; k < ncor_01; k++) {
        for (j = 0; j < 20; j++) {
            data[k * 20 + j].hiono = h[k];
        }
    }*/

    /* wet troposphere correction */
    strcpy (varname, "mod_wet_tropo_cor_01");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) {
        j = (int)floor( (k+1) / 20. );
        data[k].hwet = h[j];
    }
    /*for (k = 0; k < ncor_01; k++) {
        for (j = 0; j < 20; j++) {
            data[k * 20 + j].hwet = h[k];
        }
    }*/

    /* dry troposphere correction */
    strcpy (varname, "mod_dry_tropo_cor_01");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) {
        j = (int)floor( (k+1) / 20. );
        data[k].hdry = h[j];
    }
    /*for (k = 0; k < ncor_01; k++) {
        for (j = 0; j < 20; j++) {
            data[k * 20 + j].hdry = h[k];
        }
    }*/

    /* inverse barometric correction */
    strcpy (varname, "inv_bar_cor_01");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) {
        j = (int)floor( (k+1) / 20. );
        data[k].hinvb = h[j];
    }
    /*for (k = 0; k < ncor_01; k++) {
        for (j = 0; j < 20; j++) {
            data[k * 20 + j].hinvb = h[k];
        }
    }*/

    /* doppler correction */
    strcpy (varname, "dop_cor_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) data[k].hdopp = i[k];

    /* csum and full waveform */
    strcpy (varname, "pwr_waveform_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    /* try this one-- read it all in at once */
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    us = (unsigned short int *)work;
    for (k = 0; k < n20hz_ku; k++) {
        data[k].csum = 0;
        for (j = 0; j < 128; j++){
            data[k].wave[j] = us[k * 128 + j];
            data[k].csum = data[k].csum + data[k].wave[j];
        }
    }

    /* Beam direction vector */
    /* This is a vector, so I'll need to fix it */
    strcpy (varname, "beam_dir_vec_20_ku");
    nc_err = nc_inq_varid (ncfid, varname, &ncvid);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to get %s ID in %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    nc_err = nc_get_var (ncfid, ncvid, work);
    if ( (nc_err != NC_NOERR) || (n20hz_ku == 0) ) {
        fprintf (stderr, "Failed to load %s from %s\n", varname, fname);
        fprintf (stderr, "NetCDF Error Message %s\n", nc_strerror(nc_err));
        nc_close (ncfid);
        free ( (void *)data);
        free ( (void *)work);
        return (retval);
    }
    for (k = 0; k < n20hz_ku; k++) {
        for (j = 0; j < 3; j++) {
            data[k].beam_dir[j] = i[k * 3 + j];
        }
    }

    /* fill in remaining cryosat20hz fields with zeros */
    for (k = 0; k < n20hz_ku; k++) {
        data[k].range_s = 0;
        data[k].trackbits = 0;
        if(data[k].sec2000 <= 0) data[k].trackbits = 1;
        data[k].mss = 0;
        data[k].pswh[0] = 0;
        data[k].pswh[1] = 0;
        data[k].rchisq[0] = 0;
        data[k].rchisq[1] = 0;
        data[k].pnoise[0] = 0;
        data[k].pnoise[1] = 0;
        data[k].drange[0] = 0;
        data[k].drange[1] = 0;
        data[k].drange[2] = 0;
        data[k].decay[0] = 0;
        data[k].decay[1] = 0;
        data[k].beam[0] = 0;
        data[k].beam[1] = 0;
        data[k].beam[2] = 0;
        data[k].beam[3] = 0;
        data[k].beam[4] = 0;
        data[k].new_tide = 0;
    }
    /* -------------------------------------------------------------------- */
    /* Done reading the NetCDF file. Close it: */
    nc_close (ncfid);

    /* Write to stdout: */
    j = fwrite ((void *)data, sizeof (struct CRYOSAT20HZ), n20hz_ku, stdout);
    if (j != n20hz_ku) {
        fprintf (stderr, "Failure writing output for file %s\n", fname);
    }
    free ( (void *)data);
    free ( (void *)work);
    return (j);
}
