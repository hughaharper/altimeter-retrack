#include <stdio.h>
/*---------------------------------------------------------------*/
/* write out error message and exit                             */
/* use two strings to allow more complicated error messages     */
void die(char *s1, char *s2) {
        fprintf(stderr, " %s %s \n", s1, s2);
        exit(1);
}
