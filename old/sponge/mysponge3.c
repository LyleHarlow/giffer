/****************************************************************************

gifsponge.c - skeleton file for generic GIF `sponge' program

Slurp a GIF into core, operate on it, spew it out again.  Most of the
junk above `int main' isn't needed for the skeleton, but is likely to
be for what you'll do with it.

If you compile this, it will turn into an expensive GIF copying routine;
stdin to stdout with no changes and minimal validation.  Well, it's a
decent test of DGifSlurp() and EGifSpew(), anyway.

Note: due to the vicissitudes of Lempel-Ziv compression, the output of this
copier may not be bitwise identical to its input.  This can happen if you
copy an image from a much more (or much *less*) memory-limited system; your
compression may use more (or fewer) bits.  The uncompressed rasters should,
however, be identical (you can check this with gifbuild -d).

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "getarg.h"
#include "gif_lib.h"
#include "makeColorMap.h"

#define PROGRAM_NAME	"gifsponge"

int main(int argc, char **argv)
{
    int	i, ErrorCode;
    GifFileType *GifFileIn, *GifFileOut = (GifFileType *)NULL;

    if ((GifFileIn = DGifOpenFileHandle(0, &ErrorCode)) == NULL) {
	PrintGifError(ErrorCode);
	exit(EXIT_FAILURE);
    }
    if (DGifSlurp(GifFileIn) == GIF_ERROR) {
	PrintGifError(GifFileIn->Error);
	exit(EXIT_FAILURE);
    }
    if ((GifFileOut = EGifOpenFileHandle(1, &ErrorCode)) == NULL) {
	PrintGifError(ErrorCode);
	exit(EXIT_FAILURE);
    }


    

    /*
     * Your operations on in-core structures go here.  
     */

    HexColor hexColors[] = {0xFF0000, // red
			    0x00FF00, // green
			    0x0000FF, // blue
			    0xFFFF00, // yellow
			    0x00FFFF, // cyan
			    0xFF00FF, // magenta
			    0xFFFFFF, // white
			    0xFF6600, // orange
			    0x000000}; // black

    
    // assuming last color is black, find number of colors

    for (i=0; hexColors[i]!=0x000000; i++)
      ; // keep looking until i is the last element

    int numColors = i+1;

    ColorMapObject *palette = makeColorMap(hexColors, numColors);

    fprintf(stderr,"Sucessfully created color map\n");



    if (GifFileIn->ImageCount > 0) {
      fprintf(stderr,"Putting test in image \n");
      SavedImage *image = &(GifFileIn->SavedImages[0]);
      int left = image->ImageDesc.Left;
      int top = image->ImageDesc.Top;
      int w = image->ImageDesc.Width;
      int h = image->ImageDesc.Height;

      fprintf(stderr,"image->ImageDesc.Left=%d\n",image->ImageDesc.Left);
      fprintf(stderr,"image->ImageDesc.Top=%d\n",image->ImageDesc.Top);
      fprintf(stderr,"image->ImageDesc.Width=%d\n",image->ImageDesc.Width);
      fprintf(stderr,"image->ImageDesc.Height=%d\n",image->ImageDesc.Height);
      GifDrawRectangle(image, left + w/4, top+h/4, w/8, h/8,1);
    } else {
      fprintf(stderr,"No saved images\n");
    }

     /* This code just copies the header and each image from the incoming file.
     */

    GifFileOut->SWidth = GifFileIn->SWidth;
    GifFileOut->SHeight = GifFileIn->SHeight;
    GifFileOut->SColorResolution = GifFileIn->SColorResolution;
    GifFileOut->SBackGroundColor = GifFileIn->SBackGroundColor;
    GifFileOut->SColorMap = GifMakeMapObject(
				 GifFileIn->SColorMap->ColorCount,
				 GifFileIn->SColorMap->Colors);

    for (i = 0; i < GifFileIn->ImageCount; i++)
	(void) GifMakeSavedImage(GifFileOut, &GifFileIn->SavedImages[i]);

    /*
     * Note: don't do DGifCloseFile early, as this will
     * deallocate all the memory containing the GIF data!
     *
     * Further note: EGifSpew() doesn't try to validity-check any of this
     * data; it's *your* responsibility to keep your changes consistent.
     * Caveat hacker!
     */
    if (EGifSpew(GifFileOut) == GIF_ERROR)
	PrintGifError(GifFileOut->Error);
    else if (DGifCloseFile(GifFileIn) == GIF_ERROR)
	PrintGifError(GifFileIn->Error);

    return 0;
}

/* end */