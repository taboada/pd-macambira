/*
 *   Pure Data Packet system implementation: control object
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


/* this is an actual pd class that is used for communication with the
   pdp framework */

#include "pdp.h"
#include "pdp_internals.h"
#include <stdio.h>

/* all symbols are C style */
#ifdef __cplusplus
extern "C"
{
#endif



static long dropped_packets;

static t_class* pdp_control_class;


/* pdp control instance data */

struct pdp_control_struct;
typedef struct pdp_control_struct
{
    t_object x_obj;
    t_outlet *x_outlet0;
    struct pdp_control_struct *x_next;

} t_pdp_control;

typedef void (t_pdp_control_method_notify)(t_pdp_control *x);


static t_pdp_control *pdp_control_list;

static void pdp_control_info(t_pdp_control *x)
{
}

static void pdp_control_thread(t_pdp_control *x, t_floatarg f)
{
    int t = (int)f;

    if (t){
	post("pdp_control: switching on processing in thread");
	pdp_queue_use_thread(1);
    }
    else {
	post("pdp_control: switching off processing in thread");
	pdp_queue_use_thread(0);
    }
}


static void pdp_control_send_drop_message(t_pdp_control *x)
{
    t_atom atom[1];
    t_symbol *s = gensym("pdp_drop");

    SETFLOAT(atom+0, (float)dropped_packets);
    outlet_anything(x->x_outlet0, s, 1, atom);
}


static void pdp_control_free(t_pdp_control *x)
{
    /* remove from linked list */
    t_pdp_control *curr = pdp_control_list;
    if (pdp_control_list == x) pdp_control_list = x->x_next;
    else while (curr){
	if (curr->x_next == x) {
	    curr->x_next = x->x_next;
	    break;
	}
	else {
	    curr = curr->x_next;
	}
	
    }
}


static void *pdp_control_new(void)
{
    t_pdp_control *x = (t_pdp_control *)pd_new(pdp_control_class);
    x->x_outlet0 = outlet_new(&x->x_obj, &s_anything);

    /* add to list */
    x->x_next = pdp_control_list;
    pdp_control_list = x;
    return x;
}

/************************* class methods ***************************************/


void pdp_control_setup(void)
{

    pdp_control_list = 0;
    dropped_packets = 0;

    /* setup pd class data */
    pdp_control_class = class_new(gensym("pdp_control"), (t_newmethod)pdp_control_new,
    	(t_method)pdp_control_free, sizeof(t_pdp_control), 0, A_NULL);


    class_addmethod(pdp_control_class, (t_method)pdp_control_info, gensym("info"), A_NULL);   
    class_addmethod(pdp_control_class, (t_method)pdp_control_thread, gensym("thread"),  A_DEFFLOAT, A_NULL);   
}



void pdp_control_notify_broadcast(t_pdp_control_method_notify *notify)
{
    t_pdp_control *curr = pdp_control_list;
    while (curr){
	(*notify)(curr);
	curr = curr->x_next;
    }
}



/************************* notify class methods  *************************/

void pdp_control_notify_drop(int packet)
{
    dropped_packets++;

    /* send drop notify to controller class instances */
    pdp_control_notify_broadcast(pdp_control_send_drop_message);
    //post("dropped packet");
}



#ifdef __cplusplus
}
#endif
