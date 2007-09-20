/* ------------------------------------------------------------------------ */
/* Copyright (c) 2007 Federico Ferri.                                       */
/* For information on usage and redistribution, and for a DISCLAIMER OF ALL */
/* WARRANTIES, see the file, "LICENSE.txt," in this distribution.           */
/*                                                                          */
/* tracker: a general purpose gbrid-like widget                                */
/*                                                                          */
/* This program is free software; you can redistribute it and/or            */
/* modify it under the terms of the GNU General Public License              */
/* as published by the Free Software Foundation; either version 2           */
/* of the License, or (at your option) any later version.                   */
/*                                                                          */
/* See file LICENSE for further informations on licensing terms.            */
/*                                                                          */
/* This program is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/* GNU General Public License for more details.                             */
/*                                                                          */
/* You should have received a copy of the GNU General Public License        */
/* along with this program; if not, write to the Free Software Foundation,  */
/* Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.          */
/*                                                                          */
/* Based on PureData by Miller Puckette and others.                         */
/* Many thanks to Yves Degoyon for its externals.                           */
/* ------------------------------------------------------------------------ */

#include <stdlib.h>
#include "tracker.h"

t_widgetbehavior tracker_widgetbehavior;
static t_class* tracker_class;
static t_symbol* keyname_sym;

#define MARGIN_X 3
#define MARGIN_Y 2

static const char* tracker_isedititem(t_tracker* x, int r, int c) {
    if(x->x_active_row == r && x->x_active_row >= 0 &&
       x->x_active_column == c && x->x_active_column >= 0 &&
       x->x_buf[0]) return ">";
    else return "";
}

static int tracker_getdisplayval(t_tracker* x, int r, int c) {
    if(tracker_isedititem(x, r, c) == ">") {
        return atoi(x->x_buf);
    } else {
        return x->x_data[r][c];
    }
}

/* ---- drawing stuff ------------------------------------------------------ */

static void tracker_draw_new(t_tracker* x, t_glist* glist) {
    int r,c;
    t_canvas* canvas = glist_getcanvas(glist);
    sys_vgui(".x%x.c create rectangle %d %d %d %d -outline black -fill gray -tags %xSHAPE\n", canvas, text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist), text_xpix(&x->x_obj, glist)+x->x_columns*x->x_cell_width+2*MARGIN_X, text_ypix(&x->x_obj, glist)+x->x_rows*x->x_cell_height+2*MARGIN_Y, x);
    sys_vgui(".x%x.c create rectangle %d %d %d %d -fill #dddddd -outline {} -tags {%xCURSOR}\n", canvas, 0, 0, 0, 0, x);
    for(r = 0; r < x->x_rows; r++) {
        for(c = 0; c < x->x_columns; c++) {
            sys_vgui(".x%x.c create text %d %d -text %d%s -font -*-courier-bold--normal--%d-* -anchor nw -tags {%xTEXT %xTEXT.%d.%d} -fill black\n", canvas, text_xpix(&x->x_obj, glist)+c*x->x_cell_width+MARGIN_X, text_ypix(&x->x_obj, glist)+r*x->x_cell_height+MARGIN_Y, tracker_getdisplayval(x, r, c), tracker_isedititem(x, r, c), glist_getfont(glist), x, x, r, c);
        }
    }
    //canvas_fixlinesfor(canvas, (t_text*)x);
    tracker_draw_update(x);
}

static void tracker_draw_update(t_tracker* x) {
    int r,c;
    t_canvas* canvas = glist_getcanvas(x->x_glist);
    for(r = 0; r < x->x_rows; r++) {
        for(c = 0; c < x->x_columns; c++) {
            sys_vgui(".x%x.c itemconfigure %xTEXT.%d.%d -text %d%s\n", canvas, x, r, c, tracker_getdisplayval(x, r, c), tracker_isedititem(x, r, c));
        }
    }
    canvas_fixlinesfor(canvas, (t_text*)x);
}

static void tracker_draw_update_row(t_tracker* x, int r) {
    int c;
    t_canvas* canvas = glist_getcanvas(x->x_glist);
    for(c = 0; c < x->x_columns; c++) {
        sys_vgui(".x%x.c itemconfigure %xTEXT.%d.%d -text %d%s\n", canvas, x, r, c, tracker_getdisplayval(x, r, c), tracker_isedititem(x, r, c));
    }
    canvas_fixlinesfor(canvas, (t_text*)x);
}

static void tracker_draw_update_single(t_tracker* x, int r, int c) {
    t_canvas* canvas = glist_getcanvas(x->x_glist);
    sys_vgui(".x%x.c itemconfigure %xTEXT.%d.%d -text %d%s\n", canvas, x, r, c, tracker_getdisplayval(x, r, c), tracker_isedititem(x, r, c));
    canvas_fixlinesfor(canvas, (t_text*)x);
}

static void tracker_draw_update_cursor_pos(t_tracker* x) {
    t_canvas* canvas = glist_getcanvas(x->x_glist);
    if(x->x_cursor_pos >= 0) {
        sys_vgui(".x%x.c coords %xCURSOR %d %d %d %d\n", canvas, x, text_xpix(&x->x_obj, x->x_glist)+MARGIN_X-1, text_ypix(&x->x_obj, x->x_glist)+x->x_cursor_pos*x->x_cell_height+MARGIN_Y-1, text_xpix(&x->x_obj, x->x_glist)+x->x_columns*x->x_cell_width+MARGIN_X+1, text_ypix(&x->x_obj, x->x_glist)+(x->x_cursor_pos+1)*x->x_cell_height+MARGIN_Y+1);
    } else {
        sys_vgui(".x%x.c coords %xCURSOR %d %d %d %d\n", canvas, x, 0, 0, 0, 0);
    }
    canvas_fixlinesfor(canvas, (t_text*)x);
}

static void tracker_draw_move(t_tracker* x, t_glist* glist) {
    int r,c;
    t_canvas* canvas = glist_getcanvas(x->x_glist);
    sys_vgui(".x%x.c coords %xSHAPE %d %d %d %d\n", canvas, x, text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist), text_xpix(&x->x_obj, glist)+x->x_columns*x->x_cell_width+2*MARGIN_X, text_ypix(&x->x_obj, glist)+x->x_rows*x->x_cell_height+2*MARGIN_Y);
    if(x->x_cursor_pos >= 0) {
        sys_vgui(".x%x.c coords %xCURSOR %d %d %d %d\n", canvas, x, text_xpix(&x->x_obj, x->x_glist)+MARGIN_X-1, text_ypix(&x->x_obj, x->x_glist)+x->x_cursor_pos*x->x_cell_height+MARGIN_Y-1, text_xpix(&x->x_obj, x->x_glist)+x->x_columns*x->x_cell_width+MARGIN_X+1, text_ypix(&x->x_obj, x->x_glist)+(x->x_cursor_pos+1)*x->x_cell_height+MARGIN_Y+1);
    } else {
        sys_vgui(".x%x.c coords %xCURSOR %d %d %d %d\n", canvas, x, 0, 0, 0, 0);
    }
    for(r = 0; r < x->x_rows; r++) {
        for(c = 0; c < x->x_columns; c++) {
            sys_vgui(".x%x.c coords %xTEXT.%d.%d %d %d\n", canvas, x, r, c, text_xpix(&x->x_obj, glist)+c*x->x_cell_width+MARGIN_X, text_ypix(&x->x_obj, glist)+r*x->x_cell_height+MARGIN_Y);
        }
    }
    canvas_fixlinesfor(canvas, (t_text*)x);
}

static void tracker_draw_erase(t_tracker* x, t_glist* glist) {
    t_canvas* canvas = glist_getcanvas(x->x_glist);
    sys_vgui(".x%x.c delete %xSHAPE\n", canvas, x);
    sys_vgui(".x%x.c delete %xTEXT\n", canvas, x);
    sys_vgui(".x%x.c delete %xCURSOR\n", canvas, x);
}

static void tracker_draw_select(t_tracker* x, t_glist* glist) {
    t_canvas* canvas = glist_getcanvas(x->x_glist);
    if(x->x_selected) {
        sys_vgui(".x%x.c itemconfigure %xSHAPE -fill blue\n", canvas, x);
        sys_vgui(".x%x.c itemconfigure %xTEXT -fill white\n", canvas, x);
        sys_vgui(".x%x.c itemconfigure %xCURSOR -fill blue\n", canvas, x);
    } else {
        sys_vgui(".x%x.c itemconfigure %xSHAPE -fill gray\n", canvas, x);
        sys_vgui(".x%x.c itemconfigure %xTEXT -fill black\n", canvas, x);
        sys_vgui(".x%x.c itemconfigure %xCURSOR -fill #dddddd\n", canvas, x);
    }
    if(x->x_active_row >= 0 && x->x_active_column >= 0)
        sys_vgui(".x%x.c itemconfigure %xTEXT.%d.%d -fill red\n", canvas, x, x->x_active_row, x->x_active_column);
}

/* ---- widget behavior stuff ---------------------------------------------- */

static void tracker_getrect(t_gobj* z, t_glist* owner, int* xp1, int* yp1, int* xp2, int* yp2) {
    t_tracker* x = (t_tracker*)z;

    *xp1 = text_xpix(&x->x_obj, owner);
    *yp1 = text_ypix(&x->x_obj, owner);
    *xp2 = text_xpix(&x->x_obj, owner)+x->x_columns*x->x_cell_width+2*MARGIN_X;
    *yp2 = text_ypix(&x->x_obj, owner)+x->x_rows*x->x_cell_height+2*MARGIN_Y;
}

static void tracker_save(t_gobj* z, t_binbuf* b) {
    t_tracker* x = (t_tracker*)z;
    int ei,gi;

    binbuf_addv(b, "ssiisii", gensym("#X"), gensym("obj"),
        (t_int)x->x_obj.te_xpix, (t_int)x->x_obj.te_ypix,
        gensym("tracker"), x->x_columns, x->x_rows);

    /* save data: */
    int tmp;
    for (ei = 0; ei < x->x_rows; ei++) {
        for (gi = 0; gi < x->x_columns; gi++) {
            tmp = (int)x->x_data[ei][gi];
            binbuf_addv(b, "i", tmp);
        }
    }

    binbuf_addv(b, ";");
}

static void tracker_properties(t_gobj* z, t_glist* owner) {
    char buf[800];
    t_tracker* x = (t_tracker*)z;

    sprintf(buf, "pdtk_tracker_dialog %%s %d %d\n",
        (int)x->x_columns, (int)x->x_rows);
    //post("buf=%s", buf);
    gfxstub_new(&x->x_obj.ob_pd, x, buf);
}

static void tracker_dialog(t_tracker* x, t_symbol* s, int argc, t_atom* argv) {
    t_canvas* canvas = glist_getcanvas(x->x_glist);
    int ei;
    if(!x) {
        post("tracker: error: trying to set properties on an unexisting object");
        return;
    }
    if(argc != 2) {
        post("tracker: error in number of arguments (%d)", argc);
        return;
    }
    if(argv[0].a_type != A_FLOAT || argv[1].a_type != A_FLOAT) {
        post("tracker: error in format of arguments");
        return;
    }

    tracker_draw_erase(x, x->x_glist);

    /*
    x->x_columns = argv[0].a_w.w_float;
    if(x->x_columns <= 0) x->x_columns = 100;
    x->x_rows = argv[1].a_w.w_float;
    if(x->x_rows <= 0) x->x_rows = 100;
    */

    tracker_resize(x, (int)argv[1].a_w.w_float, (int)argv[0].a_w.w_float);

    tracker_draw_new(x, x->x_glist);
    canvas_fixlinesfor(canvas, (t_text*)x);
}

static void tracker_select(t_gobj* z, t_glist* glist, int selected) {
    t_tracker* x = (t_tracker*)z;

    x->x_selected = selected;
    tracker_draw_select(x, glist);
}

static void tracker_activate(t_gobj* z, t_glist* glist, int state) {
    post("tracker: activate(%d)", state);
}

static void tracker_vis(t_gobj* z, t_glist* glist, int vis) {
    t_tracker* x = (t_tracker*)z;
    t_rtext* y;

    if(vis) {
        tracker_draw_new(x, glist);
    } else {
        tracker_draw_erase(x, glist);
    }
}

static void tracker_delete(t_gobj* z, t_glist* glist) {
    canvas_deletelinesfor(glist_getcanvas(glist), (t_text*)z);
}

static void tracker_displace(t_gobj* z, t_glist* glist, int dx, int dy) {
    t_tracker* x = (t_tracker*)z;
    int oldx = text_xpix(&x->x_obj, glist);
    int oldy = text_ypix(&x->x_obj, glist);
    x->x_obj.te_xpix += dx;
    x->x_obj.te_ypix += dy;
    if(oldx != x->x_obj.te_xpix || oldy != x->x_obj.te_ypix) {
        tracker_draw_move(x, x->x_glist);
    }
}

static void tracker_reset_buffer(t_tracker* x) {
    int n = MAX_BUF;
    while(n--) x->x_buf[n] = 0;
    x->x_buf[MAX_BUF] = 0;
}

static void tracker_key(void* z, t_floatarg fkey) {
    t_tracker* x = (t_tracker*)z;
    int k = (int)fkey, n = 0;

    //if(k) post("key=%d (0x%.2x) ('%c')", k, k, k);

    if(k >= '0' && k <= '9') {
        //k -= '0';
        if(x->x_active_row >= 0 && x->x_active_column >= 0) {
            while(x->x_buf[n]) n++;
            if (n < MAX_BUF && n < x->x_ndigits) x->x_buf[n] = (char)k;
            tracker_draw_update_single(x, x->x_active_row, x->x_active_column);
        }
    } else if(k == '\n') {
        n = atoi(x->x_buf);
        if(x->x_active_row >= 0 && x->x_active_column >= 0) {
            int oldr,oldc;
            tracker_setitem(x, x->x_active_row, x->x_active_column, n);
            tracker_reset_buffer(x);
            tracker_draw_update_single(x, x->x_active_row, x->x_active_column);
            //tracker_select_item(x, -1, -1);
        }
    } else if(k == '\x7f' || k == 8) {
        if(x->x_active_row >= 0 && x->x_active_column >= 0) {
            while(x->x_buf[n]) n++;
            n--;
            if(n >= 0) x->x_buf[n] = 0;
            tracker_draw_update_single(x, x->x_active_row, x->x_active_column);
        }
    } else if(k == '\x1b' || k == 27) {
        tracker_reset_buffer(x);
        tracker_draw_update_single(x, x->x_active_row, x->x_active_column);
    } else if(k == '+') {
        tracker_reset_buffer(x);
        tracker_motion(x, 0, -1);
    } else if(k == '-') {
        tracker_reset_buffer(x);
        tracker_motion(x, 0, 1);
    // VIM-STYLE CURSOR MOVEMENT (how to use arrow keys?)
    } else if(k == 'J' || k == 'j') { tracker_select_movecur(x,  0, -1);
    } else if(k == 'K' || k == 'k') { tracker_select_movecur(x,  0,  1);
    } else if(k == 'H' || k == 'h') { tracker_select_movecur(x, -1,  0);
    } else if(k == 'L' || k == 'l') { tracker_select_movecur(x,  1,  0);
    }
}

static void tracker_motion(t_tracker* x, t_floatarg dx, t_floatarg dy) {
    if(x->x_active_row >= 0 && x->x_active_column >= 0) {
        if(dy == 1) {
            x->x_data[x->x_active_row][x->x_active_column]--;
            tracker_draw_update_single(x, x->x_active_row, x->x_active_column);
        } else if(dy == -1) {
            x->x_data[x->x_active_row][x->x_active_column]++;
            tracker_draw_update_single(x, x->x_active_row, x->x_active_column);
        }
    }
}

static void tracker_select_movecur(t_tracker* x, int dx, int dy) {
    int r = x->x_active_row, c = x->x_active_column;
    if(r >= 0 && c >= 0) {
        r += dy;
        if(r < 0) r = 0;
        if(r >= x->x_rows) r = x->x_rows - 1;
        c += dx;
        if(c < 0) c = 0;
        if(c >= x->x_columns) c = x->x_columns - 1;
        tracker_select_item(x, r, c);
    }
}

static void tracker_select_item(t_tracker* x, int r, int c) {
    t_canvas* canvas = glist_getcanvas(x->x_glist);
    // -1, -1 unselects
    if(r >= 0 && c >= 0) {
        if(r != x->x_active_row || c != x->x_active_column) {
            if(x->x_active_row >= 0 && x->x_active_column >= 0) {
                sys_vgui(".x%x.c itemconfigure %xTEXT.%d.%d -fill black\n", canvas, x, x->x_active_row, x->x_active_column);
                tracker_reset_buffer(x);
                tracker_draw_update_single(x, x->x_active_row, x->x_active_column);
            }
            x->x_active_row = r;
            x->x_active_column = c;
            sys_vgui(".x%x.c itemconfigure %xTEXT.%d.%d -fill red\n", canvas, x, x->x_active_row, x->x_active_column);
        }
    } else {
        if(x->x_active_row >= 0 && x->x_active_column >= 0) {
            sys_vgui(".x%x.c itemconfigure %xTEXT.%d.%d -fill black\n", canvas, x, x->x_active_row, x->x_active_column);
        }
        x->x_active_row = -1;
        x->x_active_column = -1;
    }
}

static int tracker_click(t_gobj* z, t_glist* glist, int xpix, int ypix, int shift, int alt, int dbl, int doit) {
    t_tracker* x = (t_tracker*)z;
    t_canvas* canvas = glist_getcanvas(glist);

    if(doit) {
        glist_grab(glist, &x->x_obj.te_g,
            (t_glistmotionfn)tracker_motion, tracker_key, xpix, ypix);

        int r,c,rx,ry;
        rx = xpix - text_xpix(&x->x_obj, glist) - MARGIN_X;
        ry = ypix - text_ypix(&x->x_obj, glist) - MARGIN_Y;
        c = rx / x->x_cell_width;
        r = ry / x->x_cell_height;
        tracker_select_item(x, r, c);
        tracker_reset_buffer(x);
    }

    return 1;
}

/* ------------------------------------------------------------------------- */

static void tracker_update(t_tracker* x, t_symbol* s, int argc, t_atom* argv) {
    tracker_draw_update(x);
}

static void tracker_resize(t_tracker* x, int newrows, int newcols) {
    int ei,ej;

    t_float** oldbuf = x->x_data;
    int oldrows = x->x_rows;
    int oldcols = x->x_columns;

    x->x_data = (t_float**)getbytes(newrows*sizeof(t_float*));
    for(ei = 0; ei < newrows; ei++) {
        x->x_data[ei] = (t_float*)getbytes(newcols*sizeof(t_float));
        for(ej = 0; ej < newcols; ej++) {
            x->x_data[ei][ej] = oldbuf[ei % oldrows][ej % oldcols];
        }
    }

    for(ei = 0; ei < oldrows; ei++) {
        freebytes(oldbuf[ei], oldcols*sizeof(t_float));
    }
    freebytes(oldbuf, oldrows*sizeof(t_float));

    x->x_rows = newrows;
    x->x_columns = newcols;
}

static void tracker_free(t_tracker* x) {
    int ei;
    for(ei = 0; ei < x->x_rows; ei++) {
        freebytes(x->x_data[ei], x->x_columns*sizeof(t_float));
    }
    freebytes(x->x_data, x->x_rows*sizeof(t_float));
    //pd_unbind(&x->x_obj.ob_pd, keyname_sym);
}

static t_tracker* tracker_new(t_symbol* s, int argc, t_atom* argv) {
    t_tracker* x;
    t_pd* x2;
    int ei;
    
    //post("tracker_new: create %s argc=%d", s->s_name, argc);

    x = (t_tracker*)pd_new(tracker_class);
    x->outlet0 = outlet_new(&x->x_obj, &s_list); //&s_float ??

    tracker_reset_buffer(x);

    //pd_bind(&x->x_obj.ob_pd, keyname_sym);

    x->x_data = (t_float**)getbytes(sizeof(t_float*));
    x->x_data[0] = (t_float*)getbytes(sizeof(t_float));
    x->x_data[0][0] = 0;
    x->x_columns = 1;
    x->x_rows = 1;
    x->x_ndigits = 3;
    x->x_cursor_pos = -1;

    int cols,rows;
    if(argc == 0) {
        // set default values
        cols = 10;
        rows = 10;
    } else {
        if(argc <= 2) {
            post("tracker: error: wrong # of arguments (<2)");
            return NULL;
        }
        cols = argv[0].a_w.w_float;
        rows = argv[1].a_w.w_float;
    }
    tracker_resize(x, rows, cols);

    if(argc == (2+cols*rows)) {
        // restore saved data
        for(ei = 2; ei < argc; ei++) {
            x->x_data[(ei-2)/cols][(ei-2)%cols] = argv[ei].a_w.w_float;
        }
    }
    x->x_glist = (t_glist*)canvas_getcurrent();

    /* calculate font metrics */
    int font = glist_getfont(x->x_glist);
    int width = sys_fontwidth(font);
    int height = sys_fontheight(font);
    x->x_cell_width = width*(x->x_ndigits+1);
    x->x_cell_height = height;

    return x;
}

static void tracker_getrow(t_tracker* x, t_float row) {
    int ei,introw;

    introw = (int)row;
    introw = introw % x->x_rows;
    if(introw < 0) introw = 0;

    t_atom *atombuf = (t_atom*)getbytes(sizeof(t_atom)*x->x_columns);
    for(ei = 0; ei < x->x_columns; ei++) {
        SETFLOAT(&atombuf[ei], x->x_data[introw][ei]);
    }
    outlet_list(x->outlet0, &s_list, x->x_columns, atombuf);
    freebytes(atombuf, sizeof(t_atom)*x->x_columns);

    if(1) {
        x->x_cursor_pos = ((t_int)row) % x->x_rows;
        if(x->x_cursor_pos < 0) x->x_cursor_pos = 0;
        tracker_draw_update_cursor_pos(x);
    }
}

static void tracker_setrow(t_tracker* x, t_symbol* s, int argc, t_atom* argv) {
    if(argc < 2) {
        post("tracker: setrow: too few arguments");
        return;
    }
    int ei,introw;
    
    introw = (int)argv[0].a_w.w_float;
    introw = introw % x->x_rows;
    if(introw < 0) introw = 0;

    for(ei = 1; ei < argc; ei++) {
        if((ei-1) >= x->x_columns) break;

        if(argv[ei].a_type == A_FLOAT) {
            x->x_data[introw][ei-1] = argv[ei].a_w.w_float;
        } else {
            post("tracker: warning: non-float atom converted to zero-value");
            x->x_data[introw][ei-1] = 0;
        }
    }

    tracker_draw_update_row(x, introw);
}

static void tracker_getitem(t_tracker* x, t_float row, t_float col) {
    t_atom a;
    int introw,intcol;

    introw = (int)row;
    introw = introw % x->x_rows;
    if(introw < 0) introw = 0;
    intcol = (int)col;
    intcol = introw % x->x_columns;
    if(intcol < 0) intcol = 0;

    SETFLOAT(&a, x->x_data[introw][intcol]);

    outlet_list(x->outlet0, &s_list, 1, &a);
}

static void tracker_setitem(t_tracker* x, t_float row, t_float col, t_float val) {
    int introw,intcol;
    
    introw = (int)row;
    introw = introw % x->x_rows;
    if(introw < 0) introw = 0;
    intcol = (int)col;
    intcol = intcol % x->x_columns;
    if(intcol < 0) intcol = 0;

    x->x_data[introw][intcol] = val;

    tracker_draw_update_single(x, introw, intcol);
}

static void tracker_list(t_tracker* x, t_symbol* s, int ac, t_atom* av) {
//    startpost("tracker: list> %s", s->s_name);
//    postatom(ac, av);
//    endpost();
}

void tracker_setup(void) {
keyname_sym = gensym("#keyname");
#include "tracker.tk2c"
    tracker_class = class_new(
        gensym("tracker"),
        (t_newmethod)tracker_new,
        (t_method)tracker_free,
        sizeof(t_tracker),
        0,
        A_GIMME,
        0
    );
    class_addlist(tracker_class, tracker_list);
    class_addmethod(tracker_class, (t_method)tracker_update, gensym("update"), A_GIMME, 0);
    class_addmethod(tracker_class, (t_method)tracker_dialog, gensym("dialog"), A_GIMME, 0);
    class_addmethod(tracker_class, (t_method)tracker_setrow, gensym("setrow"), A_GIMME, 0);
    class_addmethod(tracker_class, (t_method)tracker_getrow, gensym("getrow"), A_FLOAT, 0);
    class_addmethod(tracker_class, (t_method)tracker_setitem, gensym("set"), A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(tracker_class, (t_method)tracker_getitem, gensym("get"), A_FLOAT, A_FLOAT, 0);

    tracker_widgetbehavior.w_getrectfn =    tracker_getrect;
    tracker_widgetbehavior.w_displacefn =   tracker_displace;
    tracker_widgetbehavior.w_selectfn =     tracker_select;
    //tracker_widgetbehavior.w_activatefn =   NULL;
    tracker_widgetbehavior.w_activatefn =   tracker_activate;
    tracker_widgetbehavior.w_deletefn =     tracker_delete;
    tracker_widgetbehavior.w_visfn =        tracker_vis;
    tracker_widgetbehavior.w_clickfn =      tracker_click;

#if PD_MINOR_VERSION >= 37
    class_setpropertiesfn(tracker_class, tracker_properties);
    class_setsavefn(tracker_class, tracker_save);
#else
    tracker_widgetbehavior.w_propertiesfn = tracker_properties;
    tracker_widgetbehavior.w_savefn = tracker_save;
#endif

    class_setwidget(tracker_class, &tracker_widgetbehavior);
    class_sethelpsymbol(tracker_class, gensym("tracker.pd"));
}
