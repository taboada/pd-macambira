//
// fs2vf.c
// put image data to vframe
//
// 1st argument is name of shared memory
// to struct vframeimage (see Patches\vframe.h)
//

#include <memory.h>
#include "plugin.h"
#include "sharemem.h"
#include "vframe.h"

INFO("copy image data to PD's memory space")

void perform_effect(_frame f, _args a)
{
	HANDLE hlvframe=NULL, hl=NULL;
	LPVOID p=NULL;
	struct vframeimage *vfp=NULL;
	unsigned long c;

	if(!a.s) return;

	vfp = (struct vframeimage *)smopen(&hlvframe, a.s);
	if(vfp==NULL) return;

//	printf("name %s width %d height %d pf %d bits %s\n",
//	 a.s, vfp->f.width, vfp->f.height, vfp->f.pixelformat, vfp->bitsname);

	if(f.pixelformat != vfp->f.pixelformat)
	{
		printf("fs2vf: pixelformats are different. no resampling available.\n");
		return;
	}

	p = smopen(&hl, vfp->bitsname);
	if(p==NULL)
	{
		CloseHandle(hlvframe);
		return;
	}

	c = (f.height*f.lpitch < vfp->f.height*vfp->f.lpitch) ?
	 f.height*f.lpitch : vfp->f.height*vfp->f.lpitch;

	memcpy(p, f.bits, c);

	smfree(&hl, p);
	smfree(&hlvframe, vfp);
}
