//
//  softlight - softlight overlay of two images
//  
//	written by Olaf Matthes <olaf.matthes@gmx.de>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//   
//  usage: softlight
//

#include <stdlib.h>
#include "plugin.h"
#include "tools.h"

INFO("softlight overlay of two images")

void perform_effect(struct frame f, struct args a)
{
	printf("Using softlight as effect does nothing!\n");
}

void perform_copy(struct frame f1, struct frame f2, struct args a)
{
	short x, y, w, h;
	pixel16 *pix1_16, *pix2_16;
	pixel24 *pix1_24, *pix2_24;
	pixel32 *pix1_32, *pix2_32;
	byte r, g, b;
	char *t;

	w = f1.width<f2.width ? f1.width : f2.width;
	h = f1.height<f2.height ? f1.height : f2.height;

	printf("soft light\n");

	switch(f1.pixelformat)
	{
		case 16:
			// compare images pixel by pixel
			for(y = 0; y < h; y++) 
			{
				pix1_16 = scanline16(f1, y);
				pix2_16 = scanline16(f2, y);
				for(x = 0; x < w; x++)
				{	
					r = klamp255(r16(pix1_16[x]) < 128 ? 2*scl(r16(pix2_16[x]),0,255,64,192)*r16(pix1_16[x])/255 : 255-(2*(255-scl(r16(pix2_16[x]),0,255,64,192))*(255-r16(pix1_16[x]))/255));
					g = klamp255(g16(pix1_16[x]) < 128 ? 2*scl(g16(pix2_16[x]),0,255,64,192)*g16(pix1_16[x])/255 : 255-(2*(255-scl(g16(pix2_16[x]),0,255,64,192))*(255-g16(pix1_16[x]))/255));
					b = klamp255(b16(pix1_16[x]) < 128 ? 2*scl(b16(pix2_16[x]),0,255,64,192)*b16(pix1_16[x])/255 : 255-(2*(255-scl(b16(pix2_16[x]),0,255,64,192))*(255-b16(pix1_16[x]))/255));
					pix2_16[x] = rgbtocolor16(r, g, b);
				}
			}
			break;
		case 24:
			for(y = 0; y < h; y++) 
			{
				pix1_24 = scanline24(f1, y);
				pix2_24 = scanline24(f2, y);
				for(x = 0; x < w; x++)
				{
					r = klamp255(r24(pix1_24[x]) < 128 ? 2*scl(r24(pix2_24[x]),0,255,64,192)*r24(pix1_24[x])/255 : 255-(2*(255-scl(r24(pix2_24[x]),0,255,64,192))*(255-r24(pix1_24[x]))/255));
					g = klamp255(g24(pix1_24[x]) < 128 ? 2*scl(g24(pix2_24[x]),0,255,64,192)*g24(pix1_24[x])/255 : 255-(2*(255-scl(g24(pix2_24[x]),0,255,64,192))*(255-g24(pix1_24[x]))/255));
					b = klamp255(b24(pix1_24[x]) < 128 ? 2*scl(b24(pix2_24[x]),0,255,64,192)*b24(pix1_24[x])/255 : 255-(2*(255-scl(b24(pix2_24[x]),0,255,64,192))*(255-b24(pix1_24[x]))/255));
					pix2_24[x] = rgbtocolor24(r, g, b);
				}
			}
			break;
		case 32:
			for(y = 0; y < h; y++) 
			{
				pix1_32 = scanline32(f1, y);
				pix2_32 = scanline32(f2, y);
				for(x = 0; x < w; x++)
				{
					r = klamp255(r32(pix1_32[x]) < 128 ? 2*scl(r32(pix2_32[x]),0,255,64,192)*r32(pix1_32[x])/255 : 255-(2*(255-scl(r32(pix2_32[x]),0,255,64,192))*(255-r32(pix1_32[x]))/255));
					g = klamp255(g32(pix1_32[x]) < 128 ? 2*scl(g32(pix2_32[x]),0,255,64,192)*g32(pix1_32[x])/255 : 255-(2*(255-scl(g32(pix2_32[x]),0,255,64,192))*(255-g32(pix1_32[x]))/255));
					b = klamp255(b32(pix1_32[x]) < 128 ? 2*scl(b32(pix2_32[x]),0,255,64,192)*b32(pix1_32[x])/255 : 255-(2*(255-scl(b32(pix2_32[x]),0,255,64,192))*(255-b32(pix1_32[x]))/255));
					pix2_32[x] = rgbtocolor32(r, g, b);
				}
			}
			break;
	}
}
