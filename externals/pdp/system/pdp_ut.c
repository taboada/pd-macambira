/*
 *   Pure Data Packet - Utility toolkit objects.
 *   Copyright (c) by Tom Schouten <pdp@zzz.kotnet.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


/* This file contains some small utility pd objects that make working with 
   pdp objects a lot easier. Mainly as glue to be used in the abstractions 
   in the distro. */

#include "pdp.h"
#include <math.h>

/* this object does an add, scale, clip operation */

t_class *pdp_ut_addscaleclip_class;

typedef struct pdp_ut_addscaleclip_struct
{
    t_object x_obj;
    t_outlet *x_outlet0;
    t_float x_min;
    t_float x_max;
    t_float x_offset;
    t_float x_scale;
} t_pdp_ut_addscaleclip;


static void pdp_ut_addscaleclip_float(t_pdp_ut_addscaleclip *x, t_floatarg f)
{
    f += x->x_offset;
    f *= x->x_scale;
    f = (f < x->x_min) ? x->x_min : f;
    f = (f > x->x_max) ? x->x_max : f;
    outlet_float(x->x_outlet0, f);
}

static void pdp_ut_addscaleclip_free(t_pdp_ut_addscaleclip *x){}

void *pdp_ut_addscaleclip_new(t_floatarg offset, t_floatarg scale, t_floatarg min, t_floatarg max)
{
    t_pdp_ut_addscaleclip *x = (t_pdp_ut_addscaleclip *)pd_new(pdp_ut_addscaleclip_class);
    x->x_outlet0 = outlet_new(&x->x_obj, &s_float); 
    x->x_offset = offset;
    x->x_scale = scale;
    x->x_min = min;
    x->x_max = max;
    return (void *)x;
}

void pdp_ut_addscaleclip_setup(void)
{
    pdp_ut_addscaleclip_class = class_new(gensym("pdp_ut_addscaleclip"), (t_newmethod)pdp_ut_addscaleclip_new,
			      (t_method)pdp_ut_addscaleclip_free, sizeof(t_pdp_ut_addscaleclip), 0, 
					 A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_NULL);
    class_addfloat(pdp_ut_addscaleclip_class,  pdp_ut_addscaleclip_float);
}


/* pdp_ut_logmap does a logarithmic parameter mapping [0->1] x -> min(max/min)^x max an add, scale, clip operation */
/* pdp_ut_logmap_comp does x -> min(max/min)^(1-x) */
/* pdp_ut_linmap dos x -> min + (max - min * x */

t_class *pdp_ut_linmap_class;
t_class *pdp_ut_logmap_class;
t_class *pdp_ut_logmap_comp_class;

typedef struct pdp_ut_map_struct
{
    t_object x_obj;
    t_outlet *x_outlet0;
    t_float x_min;
    t_float x_max;
} t_pdp_ut_map;


static void pdp_ut_logmap_float(t_pdp_ut_map *x, t_floatarg f)
{
    f = (f < 0.0f) ? 0.0f : f;
    f = (f > 1.0f) ? 1.0f : f;

    f = x->x_min * pow((x->x_max / x->x_min), f);

    outlet_float(x->x_outlet0, f);
}

static void pdp_ut_linmap_float(t_pdp_ut_map *x, t_floatarg f)
{
    f = (f < 0.0f) ? 0.0f : f;
    f = (f > 1.0f) ? 1.0f : f;

    f = x->x_min + ((x->x_max - x->x_min) * f);

    outlet_float(x->x_outlet0, f);
}

static void pdp_ut_logmap_comp_float(t_pdp_ut_map *x, t_floatarg f)
{
    f = (f < 0.0f) ? 0.0f : f;
    f = (f > 1.0f) ? 1.0f : f;

    f = x->x_min * pow((x->x_max / x->x_min), (1.0f - f));

    outlet_float(x->x_outlet0, f);
}

static void pdp_ut_map_free(t_pdp_ut_map *x){}


void pdp_ut_map_init(t_pdp_ut_map *x, t_floatarg min, t_floatarg max)
{
    x->x_outlet0 = outlet_new(&x->x_obj, &s_float); 
    x->x_min = min;
    x->x_max = max;
}

void *pdp_ut_logmap_new(t_floatarg min, t_floatarg max)
{
    t_pdp_ut_map *x = (t_pdp_ut_map *)pd_new(pdp_ut_logmap_class);
    pdp_ut_map_init(x, min, max);
    return (void *)x;
}

void *pdp_ut_linmap_new(t_floatarg min, t_floatarg max)
{
    t_pdp_ut_map *x = (t_pdp_ut_map *)pd_new(pdp_ut_linmap_class);
    pdp_ut_map_init(x, min, max);
    return (void *)x;
}

void *pdp_ut_logmap_comp_new(t_floatarg min, t_floatarg max)
{
    t_pdp_ut_map *x = (t_pdp_ut_map *)pd_new(pdp_ut_logmap_comp_class);
    pdp_ut_map_init(x, min, max);
    return (void *)x;
}

void pdp_ut_logmap_setup(void)
{
    pdp_ut_logmap_class = class_new(gensym("pdp_ut_logmap"), (t_newmethod)pdp_ut_logmap_new,
			      (t_method)pdp_ut_map_free, sizeof(t_pdp_ut_map), 0, 
					 A_FLOAT, A_FLOAT, A_NULL);
    class_addfloat(pdp_ut_logmap_class,  pdp_ut_logmap_float);
}

void pdp_ut_logmap_comp_setup(void)
{
    pdp_ut_logmap_comp_class = class_new(gensym("pdp_ut_logmap_comp"), (t_newmethod)pdp_ut_logmap_comp_new,
			      (t_method)pdp_ut_map_free, sizeof(t_pdp_ut_map), 0, 
					 A_FLOAT, A_FLOAT, A_NULL);
    class_addfloat(pdp_ut_logmap_comp_class,  pdp_ut_logmap_comp_float);
}

void pdp_ut_linmap_setup(void)
{
    pdp_ut_linmap_class = class_new(gensym("pdp_ut_linmap"), (t_newmethod)pdp_ut_linmap_new,
			      (t_method)pdp_ut_map_free, sizeof(t_pdp_ut_map), 0, 
					 A_FLOAT, A_FLOAT, A_NULL);
    class_addfloat(pdp_ut_linmap_class,  pdp_ut_linmap_float);
}



#ifdef __cplusplus
extern "C"
{
#endif

void pdp_ut_setup(void)
{
    pdp_ut_addscaleclip_setup();
    pdp_ut_logmap_setup();
    pdp_ut_logmap_comp_setup();
    pdp_ut_linmap_setup();
}


#ifdef __cplusplus
}
#endif
