/******************************************************
 *
 * zexy - implementation file
 *
 * copyleft (c) IOhannes m zm�lnig
 *
 *   1999:forum::f�r::uml�ute:2004
 *
 *   institute of electronic music and acoustics (iem)
 *
 ******************************************************
 *
 * license: GNU General Public License v.2
 *
 ******************************************************/

/*
	finally :: some of the missing binops for signals :: <~

	1302:forum::f�r::uml�ute:2000
*/

#include "zexy.h"

/* ------------------------ relational~ ----------------------------- */


/* ----------------------------- lt_tilde ----------------------------- */
static t_class *lt_tilde_class, *scalarlt_tilde_class;

typedef struct _lt_tilde
{
  t_object x_obj;
  float x_f;
} t_lt_tilde;

typedef struct _scalarlt_tilde
{
  t_object x_obj;
  float x_f;
  t_float x_g;    	    /* inlet value */
} t_scalarlt_tilde;

static void *lt_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
  if (argc > 1) post("<~: extra arguments ignored");
  if (argc) 
    {
      t_scalarlt_tilde *x = (t_scalarlt_tilde *)pd_new(scalarlt_tilde_class);
      floatinlet_new(&x->x_obj, &x->x_g);
      x->x_g = atom_getfloatarg(0, argc, argv);
      outlet_new(&x->x_obj, &s_signal);
      x->x_f = 0;
      return (x);
    }
  else
    {
      t_lt_tilde *x = (t_lt_tilde *)pd_new(lt_tilde_class);
      inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
      outlet_new(&x->x_obj, &s_signal);
      x->x_f = 0;
      return (x);
    }
}

static t_int *lt_tilde_perform(t_int *w)
{
  t_float *in1 = (t_float *)(w[1]);
  t_float *in2 = (t_float *)(w[2]);
  t_float *out = (t_float *)(w[3]);
  int n = (int)(w[4]);
  while (n--) *out++ = *in1++ < *in2++; 
  return (w+5);
}

static t_int *lt_tilde_perf8(t_int *w)
{
  t_float *in1 = (t_float *)(w[1]);
  t_float *in2 = (t_float *)(w[2]);
  t_float *out = (t_float *)(w[3]);
  int n = (int)(w[4]);
  for (; n; n -= 8, in1 += 8, in2 += 8, out += 8)
    {
      float f0 = in1[0], f1 = in1[1], f2 = in1[2], f3 = in1[3];
      float f4 = in1[4], f5 = in1[5], f6 = in1[6], f7 = in1[7];
      
      float g0 = in2[0], g1 = in2[1], g2 = in2[2], g3 = in2[3];
      float g4 = in2[4], g5 = in2[5], g6 = in2[6], g7 = in2[7];
      
      out[0] = f0 < g0; out[1] = f1 < g1; out[2] = f2 < g2; out[3] = f3 < g3;
      out[4] = f4 < g4; out[5] = f5 < g5; out[6] = f6 < g6; out[7] = f7 < g7;
    }
  return (w+5);
}

static t_int *scalarlt_tilde_perform(t_int *w)
{
  t_float *in = (t_float *)(w[1]);
  t_float f = *(t_float *)(w[2]);
  t_float *out = (t_float *)(w[3]);
  int n = (int)(w[4]);
  while (n--) *out++ = *in++ < f; 
  return (w+5);
}

static t_int *scalarlt_tilde_perf8(t_int *w)
{
  t_float *in = (t_float *)(w[1]);
  t_float g = *(t_float *)(w[2]);
  t_float *out = (t_float *)(w[3]);
  int n = (int)(w[4]);
  for (; n; n -= 8, in += 8, out += 8)
    {
      float f0 = in[0], f1 = in[1], f2 = in[2], f3 = in[3];
      float f4 = in[4], f5 = in[5], f6 = in[6], f7 = in[7];
      
      out[0] = f0 < g; out[1] = f1 < g; out[2] = f2 < g; out[3] = f3 < g;
      out[4] = f4 < g; out[5] = f5 < g; out[6] = f6 < g; out[7] = f7 < g;
    }
  return (w+5);
}
#ifdef __SSE__
static t_int *lt_tilde_performSSE(t_int *w)
{
  __m128 *in1 = (__m128 *)(w[1]);
  __m128 *in2 = (__m128 *)(w[2]);
  __m128 *out = (__m128 *)(w[3]);
  int n = (int)(w[4])>>4;
  const __m128 one    = _mm_set1_ps(1.f);

  while (n--) {
    __m128 xmm0, xmm1;
    xmm0   = _mm_cmplt_ps(in1[0], in2[0]);
    out[0] = _mm_and_ps  (xmm0 , one);

    xmm1   = _mm_cmplt_ps(in1[1], in2[1]);
    out[1] = _mm_and_ps  (xmm1 , one);

    xmm0   = _mm_cmplt_ps(in1[2], in2[2]);
    out[2] = _mm_and_ps  (xmm0 , one);

    xmm1   = _mm_cmplt_ps(in1[3], in2[3]);
    out[3] = _mm_and_ps  (xmm1 , one);

    in1+=4;
    in2+=4;
    out+=4;
  }  

  return (w+5);
}
static t_int *scalarlt_tilde_performSSE(t_int *w)
{
  __m128 *in = (__m128 *)(w[1]);
  __m128 *out = (__m128 *)(w[3]);
  t_float f = *(t_float *)(w[2]);
  __m128 scalar = _mm_set1_ps(f);
  int n = (int)(w[4])>>4;
  const __m128 one    = _mm_set1_ps(1.f);

  while (n--) {
    __m128 xmm0, xmm1;
    xmm0   = _mm_cmplt_ps (in[0], scalar);
    out[0] = _mm_and_ps   (xmm0,  one);

    xmm1   = _mm_cmplt_ps (in[1], scalar);
    out[1] = _mm_and_ps   (xmm1,  one);

    xmm0   = _mm_cmplt_ps (in[2], scalar);
    out[2] = _mm_and_ps   (xmm0,  one);

    xmm1   = _mm_cmplt_ps (in[3], scalar);
    out[3] = _mm_and_ps   (xmm1,  one);

    in +=4;
    out+=4;
  }
  return (w+5);
}
#endif /* __SSE__ */

static void lt_tilde_dsp(t_lt_tilde *x, t_signal **sp)
{
  t_sample*in1=sp[0]->s_vec;
  t_sample*in2=sp[1]->s_vec;
  t_sample*out=sp[2]->s_vec;

  int n=sp[0]->s_n;

#ifdef __SSE__
  if(
     Z_SIMD_CHKBLOCKSIZE(n)&&
     Z_SIMD_CHKALIGN(in1)&&
     Z_SIMD_CHKALIGN(in2)&&
     Z_SIMD_CHKALIGN(out)
     )
    {
      post("SIMD");
      dsp_add(lt_tilde_performSSE, 4, in1, in2, out, n);
    } else
#endif
  if(n&7)
    dsp_add(lt_tilde_perform, 4, in1, in2, out, n);
  else	
    dsp_add(lt_tilde_perf8, 4, in1, in2, out, n);
}

static void scalarlt_tilde_dsp(t_scalarlt_tilde *x, t_signal **sp)
{
  t_sample*in =sp[0]->s_vec;
  t_sample*out=sp[1]->s_vec;
  int n       =sp[0]->s_n;

#ifdef __SSE__
  if(
     Z_SIMD_CHKBLOCKSIZE(n)&&
     Z_SIMD_CHKALIGN(in)&&
     Z_SIMD_CHKALIGN(out)
     )
    {
      post("SIMD");
      dsp_add(scalarlt_tilde_performSSE, 4, in, &x->x_g, out, n);
    } else
#endif
  if (n&7)
    dsp_add(scalarlt_tilde_perform, 4, in, &x->x_g, out, n);
  else	
    dsp_add(scalarlt_tilde_perf8, 4, in, &x->x_g, out, n);
}

static void lt_tilde_setup(void)
{
  lt_tilde_class = class_new(gensym("<~"), (t_newmethod)lt_tilde_new, 0,
			    sizeof(t_lt_tilde), 0, A_GIMME, 0);
  class_addmethod(lt_tilde_class, (t_method)lt_tilde_dsp, gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(lt_tilde_class, t_lt_tilde, x_f);
  class_sethelpsymbol(lt_tilde_class, gensym("zexy/sigbinops+"));
  scalarlt_tilde_class = class_new(gensym("<~"), 0, 0,
				  sizeof(t_scalarlt_tilde), 0, 0);
  CLASS_MAINSIGNALIN(scalarlt_tilde_class, t_scalarlt_tilde, x_f);
  class_addmethod(scalarlt_tilde_class, (t_method)scalarlt_tilde_dsp, gensym("dsp"),
		  0);
  class_sethelpsymbol(scalarlt_tilde_class, gensym("zexy/sigbinops+"));
  zexy_register("<~");
}


/* ---------------------- global setup ------------------------- */
void z_0x3c0x7e_setup(void)
{
	lt_tilde_setup();
}

void setup_0x3c0x7e(void)
{
	lt_tilde_setup();
}

