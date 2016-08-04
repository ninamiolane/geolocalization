/* Lear's GIST implementation, version 1.1, (c) INRIA 2009, Licence: GPL */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>


#include "gist.h"

#define CV_IMWRITE_JPEG_QUALITY 1
#define CV_IMWRITE_PNG_COMPRESSION 16
#define CV_IMWRITE_PXM_BINARY 32

static float distance2(float desc1[], float desc2[], int descsize) {
    float l = 0;
    int i;
    for (i = 0; i < descsize; i++) {
        l += pow(desc1[i] - desc2[i], 2);
    }
    l = sqrt(l);
    return l;
}

static color_image_t *load_ppm(const char *fname) {
    FILE *f = fopen(fname, "r");
    if (!f) {
        perror("could not open infile");
        exit(1);
    }
    int px, width, height, maxval;
    if (fscanf(f, "P%d %d %d %d", &px, &width, &height, &maxval) != 4 ||
            maxval != 255 || (px != 6 && px != 5)) {
        fprintf(stderr, "Error: input not a raw PGM/PPM with maxval 255\n");
        exit(1);
    }
    fgetc(f); /* eat the newline */
    color_image_t *im = color_image_new(width, height);

    int i;
    for (i = 0; i < width * height; i++) {
        im->c1[i] = fgetc(f);
        if (px == 6) {
            im->c2[i] = fgetc(f);
            im->c3[i] = fgetc(f);
        } else {
            im->c2[i] = im->c1[i];
            im->c3[i] = im->c1[i];
        }
    }

    fclose(f);
    return im;
}

static void convertToPPM(const char* infilename, char** output) {
    char *filename = malloc(100 * sizeof (char));
    filename = strcpy(filename, infilename);
    char* pch = strtok(filename, ".");
    int count = 0;
    int needConvert = 1;
    while (pch != NULL) {
        const char* aux0 = pch;
        if (strcmp(aux0, "ppm") == 0) {
            needConvert = 0;
        }
        pch = strtok(NULL, ".");
        count++;
    }
    if (needConvert == 1) {
        filename = strcpy(filename, infilename);
        pch = strtok(filename, ".");
        int i;
        for (i = 0; i < count - 1; i++) {
            const char* aux = pch;
            strcat(*output, aux);
            strcat(*output, ".");
            pch = strtok(NULL, ".");
        }

        strcat(*output, "ppm");
        char* commande = malloc(200 * sizeof (char));
        strcat(commande, "anytopnm ");
        strcat(commande, infilename);
        strcat(commande, " | pnmscale -xysize 1000 750 > ");
        const char* aux1 = *output;
        strcat(commande, aux1);
        system(commande);
        free(commande);
    } else {
        strcpy(*output, infilename);
    }
    free(filename);
}

static void compileAndPrintGist(const char *infilename, int nblocks, int n_scale,
        int *orientations_per_scale) {
    //char* infilenamePPM = convertToPPM(infilename);


    color_image_t *im = load_ppm(infilename);
    float *desc = color_gist_scaletab(im, nblocks, n_scale, orientations_per_scale);

    int i;

    int descsize = 0;
    /* compute descriptor size */
    for (i = 0; i < n_scale; i++)
        descsize += nblocks * nblocks * orientations_per_scale[i];

    descsize *= 3; /* color */

    /* print descriptor */
    for (i = 0; i < descsize; i++) {
        printf("%.4f ", desc[i]);
    }

    printf("\n\n");
    printf("\n");

    free(desc);
    color_image_delete(im);

}

static void compileAndPrintGist2(const char *infilename, const char *infilename2, int nblocks, int n_scale,
        int *orientations_per_scale) {

    //char* infilenamePPM = convertToPPM(infilename);
    //char* infilenamePPM2 = convertToPPM(infilename2);

    color_image_t *im = load_ppm(infilename);
    color_image_t *im2 = load_ppm(infilename2);

    float *desc = color_gist_scaletab(im, nblocks, n_scale, orientations_per_scale);
    float *desc2 = color_gist_scaletab(im2, nblocks, n_scale, orientations_per_scale);

    int i;

    int descsize = 0;
    /* compute descriptor size */
    for (i = 0; i < n_scale; i++)
        descsize += nblocks * nblocks * orientations_per_scale[i];

    descsize *= 3; /* color */

    /* print descriptor */
    for (i = 0; i < descsize; i++) {
        printf("%.4f ", desc[i]);
    }

    printf("\n\n");

    for (i = 0; i < descsize; i++) {
        printf("%.4f ", desc2[i]);
    }

    printf("\n\n");

    printf("%f", distance2(desc, desc2, descsize));

    printf("\n");

    free(desc);
    free(desc2);

    color_image_delete(im);
    color_image_delete(im2);
}

static void compileAndCompareGist(const char *infilename, const char *base, const char *basePSC, const char *resultatsGIST, int nblocks, int n_scale,
        int *orientations_per_scale, float threshold) {

    char *infilenameAux = malloc(100 * sizeof (char));
    convertToPPM(infilename, &infilenameAux);
    const char* infilenamePPM = infilenameAux;
    printf("Loading %s\n", infilenamePPM);
    color_image_t *im = load_ppm(infilenamePPM);
    printf("Processing GIST...\n");
    float *desc = color_gist_scaletab(im, nblocks, n_scale, orientations_per_scale);
    printf("GIST done. Comparing...\n");

    int i;

    int descsize = 0;
    /* compute descriptor size */
    for (i = 0; i < n_scale; i++)
        descsize += nblocks * nblocks * orientations_per_scale[i];

    descsize *= 3; /* color */

    color_image_delete(im);

    char* photos[100];
    int count = 0;
    char ligne[2000] = "";
    char *name = malloc(100 * sizeof (char));
    float *descBase;
    descBase = malloc(descsize * sizeof (float));

    FILE *dataBase = NULL;
    FILE *results = NULL;
    FILE *dataBasePSC = NULL;
    dataBase = fopen(base, "r");
    if (dataBase != NULL) {
        while (fgets(ligne, 2000, dataBase) != NULL && count < 100) {
            char* pch = strtok(ligne, " ");
            const char* aux = pch;
            strcpy(name, aux);
            pch = strtok(NULL, " ");
            for (i = 0; pch != NULL; i++) {
                descBase[i] = atof(pch);
                pch = strtok(NULL, " ");

            }
            float b = distance2(desc, descBase, descsize);
            if (b < threshold) {
                const char* aux1 = name;
                photos[count] = malloc(50 * sizeof (char));
                strcpy(photos[count], aux1);
                count++;
            }
        }
        fclose(dataBase);
        results = fopen(resultatsGIST, "w+");
        dataBasePSC = fopen(basePSC, "r");
        if (results != NULL && dataBasePSC != NULL) {
            fprintf(results, "%d\n", count);
            while(fgets(ligne, 2000, dataBasePSC) != NULL){
                char ligne2[500];
                strcpy(ligne2, ligne);
                const char* aux2 = strtok(ligne, ":");
               for (i = 0; i < count; i++) {
                    const char* aux3 = photos[i];
                    if(strcmp(aux2, aux3)==0){
                        fprintf(results, "%s", ligne2);
                    }
                }
            }
            fclose(dataBasePSC);
            fclose(results);
        } else {
            printf("Could not open %s or %s or %s\n", base, basePSC, resultatsGIST);
        }

    } else {
        printf("Could not open %s\n", base);
    }
    free(name);
    free(descBase);
    free(infilenameAux);
    free(desc);
}

static void cleanName(const char* infilename, char* *output) {
    char * pch;
    char * pch1;
    pch = strrchr(infilename, '/');
    pch1 = strrchr(infilename, '.');
    int a = pch1 - pch;
    int i = *++pch;
    for (i = 0; i < a - 1; i++) {
        *(*output + i) = *pch;
        if (!*++pch) {
            exit(1);
        }
    }
    *(*output + a - 1) = '\0';
}

static void compileAndAddToBaseGist(const char *infilename, const char *base, int nblocks, int n_scale,
        int *orientations_per_scale) {

    char *infilenameAux = malloc(100 * sizeof (char));
    convertToPPM(infilename, &infilenameAux);
    const char* infilenamePPM = infilenameAux;


    color_image_t *im = load_ppm(infilenamePPM);
    float *desc = color_gist_scaletab(im, nblocks, n_scale, orientations_per_scale);

    int i;

    int descsize = 0;
    /* compute descriptor size */
    for (i = 0; i < n_scale; i++)
        descsize += nblocks * nblocks * orientations_per_scale[i];

    descsize *= 3; /* color */

    FILE *dataBase = NULL;
    dataBase = fopen(base, "a");
    /* write descriptor */
    if (dataBase != NULL) {
        char *filename;
        filename = malloc(100 * sizeof (char));
        cleanName(infilenamePPM, &filename);
        fprintf(dataBase, "%s ", filename);
        free(filename);
        for (i = 0; i < descsize; i++) {
            fprintf(dataBase, "%.4f ", desc[i]);
        }
        fprintf(dataBase, "\n");
        printf("%s added to %s\n", infilename, base);
    } else {
        printf("Could not open %s\n", base);
    }
    fclose(dataBase);

    free(desc);
    free(infilenameAux);
    color_image_delete(im);

}

static void usage(void) {
    fprintf(stderr, "compute_gist options... [infilename]\n"
            "infile is a PPM raw file\n"
            "options:\n"
            "[-nblocks nb] use a grid of nb*nb cells (default 4)\n"
            "[-orientationsPerScale o_1,..,o_n] use n scales and compute o_i orientations for scale i\n"
            "[options][-addtobase img] add image to database base.txt\n"
            "[options][-comparetobase img] compare image to database base.txt\n"
            );

    exit(1);
}

int main(int argc, char **args) {
    const char *infilename = "/dev/stdin";
    const char *infilename2 = "/dev/stdin";
    //Paramètres par défaut
    int nblocks = 2;
    int n_scale = 3;
    int orientations_per_scale[50] = {8, 8, 4};
    const char *base = "baseGIST.txt";
    const char *basePSC = "PSC_DonnéesPhotos.txt";
    const char *resultatsGIST = "resultatsGIST.txt";
    int mode = 0;
    float threshold = 0.3;

    while (*++args) {
        const char *a = *args;
        //Demande l'aide
        if (!strcmp(a, "-h")) usage();
            //Stocke un double dans -nblocks
        else if (!strcmp(a, "-nblocks")) {
            if (!sscanf(*++args, "%d", &nblocks)) {
                fprintf(stderr, "could not parse %s argument", a);
                usage();
            }

        } else if (!strcmp(a, "-threshold")) {
            if (!sscanf(*++args, "%f", &threshold)) {
                fprintf(stderr, "could not parse %s argument", a);
                usage();
            }

        } else if (!strcmp(a, "-addtobase")) {
            if (*++args) {
                infilename = *args;
                mode = 1;
            } else {
                usage();
            }

        } else if (!strcmp(a, "-comparetobase")) {
            if (*++args) {
                infilename = *args;
                mode = 2;
            } else {
                usage();
            }
            //Stocke les argument orientation per scale dans orientations_per_scale
        } else if (!strcmp(a, "-orientationsPerScale")) {
            char *c;
            n_scale = 0;
            for (c = strtok(*++args, ","); c; c = strtok(NULL, ",")) {
                if (!sscanf(c, "%d", &orientations_per_scale[n_scale++])) {
                    fprintf(stderr, "could not parse %s argument", a);
                    usage();
                }
            }
            //Condition pour vérifier qu'on ne change qu'une fois les noms des fichiers images
        } else if (!strcmp(infilename, "/dev/stdin")) {
            infilename = a;
        } else if (!strcmp(infilename, "/dev/stdin")) {
            infilename2 = a;
        }
    }
    if (mode == 0) {
        if (strcmp(infilename2, "/dev/stdin")) {
            compileAndPrintGist2(infilename, infilename2, nblocks, n_scale, orientations_per_scale);
        } else {
            compileAndPrintGist(infilename, nblocks, n_scale, orientations_per_scale);
        }
    } else if (mode == 1) {
        compileAndAddToBaseGist(infilename, base, nblocks, n_scale, orientations_per_scale);
    } else if (mode == 2) {
        compileAndCompareGist(infilename, base, basePSC, resultatsGIST, nblocks, n_scale, orientations_per_scale, threshold);
    }


    return 0;
}
