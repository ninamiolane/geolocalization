/************************************************************************
Demo software: Invariant keypoint matching.
Author: David Lowe

match.c:
This file contains a sample program to read images and keypoints, then
   draw lines connecting matched keypoints.
*************************************************************************/


#include "defs.h"
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h> 

/* -------------------- Local function prototypes ------------------------ */

void FindMatches(Image im1, Keypoint keys1, Image im2, Keypoint keys2);
Keypoint CheckForMatch(Keypoint key, Keypoint klist);
int DistSquared(Keypoint k1, Keypoint k2);
Image CombineImagesVertically(Image im1, Image im2);
Image SortiePanoramique(Image im1);


/*----------------------------- Routines ----------------------------------*/

/* Top level routine.  Read PGM images and keypoints from files given
   in command line arguments, then call FindMatches.
*/
int main (int argc, char **argv)
{
    int arg = 0;
    Image im1 = NULL, im2 = NULL;
    Keypoint k1 = NULL, k2 = NULL;

    /* Parse command line arguments and read given files.  The command
       line must specify two input images and two files of keypoints
       using command line arguments as follows:
          match -im1 i1.pgm -k1 k1.key -im2 i2.pgm -k2 k2.key > result.v
    */
    while (++arg < argc) {
      if (! strcmp(argv[arg], "-im1")) 
	im1 = ReadPGMFile(argv[++arg]);
      else if (! strcmp(argv[arg], "-im2")) 
	im2 = ReadPGMFile(argv[++arg]);
      else if (! strcmp(argv[arg], "-k1"))
	k1 = ReadKeyFile(argv[++arg]);
      else if (! strcmp(argv[arg], "-k2"))
	k2 = ReadKeyFile(argv[++arg]);
      else
	FatalError("Invalid command line argument: %s", argv[arg]);
    }
    if (im1 == NULL || im2 == NULL || k1 == NULL || k2 == NULL)
      FatalError("Command line does not specify all images and keys.");

    FindMatches(im1, k1, im2, k2);
    exit(0);
}


/* Given a pair of images and their keypoints, pick the first keypoint
   from one image and find its closest match in the second set of
   keypoints.  Then write the result to a file.
*/
void FindMatches(Image im1, Keypoint keys1, Image im2, Keypoint keys2)
{
    Keypoint k, match;
    Image result;
	Image pano;
    int count = 0, j, p=0, ro =0, co = 0;
	int minh, maxh, minl, maxl;
	float a=0,b=0,c=0,d=0;
	float a2=0,b2=0,c2=0,d2=0;
	float a1=0,b1=0,c1=0,d1=0;
	float a12=0,b12=0,c12=0,d12=0;
	FILE * fmatch;
	FILE * nbm;
	FILE * ori;
	FILE * photo;

    /* Create a new image that joins the two images vertically. */
    result = CombineImagesVertically(im1, im2);

    /* Match the keys in list keys1 to their best matches in keys2.
    */
		fmatch = fopen ("fmatch.key", "w");
		ori = fopen("orientation.txt", "w");
		fprintf(fmatch, "      128\n");
    for (k= keys1; k != NULL; k = k->next) {
      match = CheckForMatch(k, keys2);  

	  


      /* Draw a line on the image from keys1 to match.  Note that we
	 must add row count of first image to row position in second so
	 that line ends at correct location in second image.
      */

      if (match != NULL) {
		  // Test
		  if (count == 0){
		  minh = k->row;
		  maxh = k->row;
		  minl = k->col;
		  maxl = k->col;
		  }
	  else {
		  minh = min(k->row,minh);
		  maxh = max(k->row, maxh);
		  minl = min(k->col, minl);
		  maxl = max(k->col, maxl);
	  }


		  a = (k->row);
		  b = (k->col);
		  c = (k->scale);
		  d = (k->ori);
		  a1 = (match->row);
		  b1 = (match->col);
		  c1 = (match->scale);
		  d1 = (match->ori);
	fprintf(fmatch, "%f %f %f %f\n", k->row, k->col, k->scale, k->ori);
	fprintf(ori, "%f %f \n", fabs((a-a2)/(a1-a12)), acos(min((a-a2)/(a1-a12)*(b1-b12)/(b-b2),1/((a-a2)/(a1-a12))*(b-b2)/(b1-b12))));
	p = 1;
	for (j = 0; j < 128; j++) {
	
	fprintf(fmatch, " %d",  (int)match->descrip[j]);
	if (p % 20 == 0) {
		fprintf(fmatch, "\n ");
		p = 1;}
	p++;
      }
	fprintf(fmatch, "\n");
	
		// Fin du test
	count++;
	DrawLine(result, (int) k->row, (int) k->col,
		 (int) (match->row + im1->rows), (int) match->col);
			a2 = a;
		  b2 = b;
		 c2 = c;
		  d2 = d;
		  	a12 = a1;
		  b12 = b1;
		 c12 = c1;
		  d12 = d1;
      }
    }
	rewind(fmatch);
	fprintf(fmatch, "%d", count);
	fclose(ori);
	fclose(fmatch);
    /* Write result image to standard output. */
  //  WritePGM(stdout, result);
	pano = SortiePanoramique(im1);	
	photo = fopen("phototestee.pgm", "w");
	DrawLine(pano, minh, minl, minh, maxl);
	DrawLine(pano, minh, maxl, maxh, maxl);
	DrawLine(pano, maxh, maxl, maxh, minl);
	DrawLine(pano, maxh, minl, minh, minl);
	WritePGM(stdout, pano);
	fclose(photo);
    fprintf(stderr,"Found %d matches.\n", count);
	nbm = fopen ("nbm.txt", "w");
	fprintf(nbm, "%d", count);
	fclose(nbm);
}


/* This searches through the keypoints in klist for the two closest
   matches to key.  If the closest is less than 0.6 times distance to
   second closest, then return the closest match.  Otherwise, return
   NULL.
*/
Keypoint CheckForMatch(Keypoint key, Keypoint klist)
{
    int dsq, distsq1 = 100000000, distsq2 = 100000000;
    Keypoint k, minkey = NULL;

    /* Find the two closest matches, and put their squared distances in
       distsq1 and distsq2.
    */
    for (k = klist; k != NULL; k = k->next) {
      dsq = DistSquared(key, k);

      if (dsq < distsq1) {
	distsq2 = distsq1;
	distsq1 = dsq;
	minkey = k;
      } else if (dsq < distsq2) {
	distsq2 = dsq;
      }
    }

    /* Check whether closest distance is less than 0.6 of second. Correction : essai à 0.3*/
	if (10 * 10 * distsq1 < 3*3* distsq2)
      return minkey;
    else return NULL;
}


/* Return squared distance between two keypoint descriptors.
*/
int DistSquared(Keypoint k1, Keypoint k2)
{
    int i, dif, distsq = 0;
    unsigned char *pk1, *pk2;

    pk1 = k1->descrip;
    pk2 = k2->descrip;

    for (i = 0; i < 128; i++) {
      dif = (int) *pk1++ - (int) *pk2++;
      distsq += dif * dif;
    }
    return distsq;
}


/* Return a new image that contains the two images with im1 above im2.
*/
Image CombineImagesVertically(Image im1, Image im2)
{
    int rows, cols, r, c;
    Image result;

    rows = im1->rows + im2->rows;
    cols = MAX(im1->cols, im2->cols);
    result = CreateImage(rows, cols);

    /* Set all pixels to 0,5, so that blank regions are grey. */
    for (r = 0; r < rows; r++)
      for (c = 0; c < cols; c++)
	result->pixels[r][c] = 0.5;

    /* Copy images into result. */
    for (r = 0; r < im1->rows; r++)
      for (c = 0; c < im1->cols; c++)
	result->pixels[r][c] = im1->pixels[r][c];
    for (r = 0; r < im2->rows; r++)
      for (c = 0; c < im2->cols; c++)
	result->pixels[r + im1->rows][c] = im2->pixels[r][c];
    
    return result;
}

Image SortiePanoramique(Image imag){
	Image res;
	int ro, co;

	res = CreateImage(imag->rows, imag->cols);
	for (ro = 0; ro < imag->rows; ro++)
      for (co = 0; co < imag->cols; co++)
	res->pixels[ro][co] = imag->pixels[ro][co];



	return res;
}


