#include "dedx_tools.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct
{
  //int id;
  //  double A;
  dedx_workspace * ws;
  dedx_config *cfg;
} _dedx_tools_settings;
double _dedx_adapt24(double (*func)(double x,_dedx_tools_settings * set), _dedx_tools_settings * set,double a, double b, double f2, double f3, double acc, double eps, double *err)
{
  double h = b-a;
  double f1 = (*func)(a+h/6,set);
  double f4 = (*func)(a+h*5/6,set);
  double q2 = (f1+f2+f3+f4)*h/4;
  double q4 = ((f1+f4)/3+(f2+f3)/6)*h;
  double tol = acc+eps*fabs(q4);
  *err = fabs(q4-q2)/3;
  if(*err < tol)
    return q4;
  else
  {
    acc /= sqrt(2);
    double mid = (a+b)/2;
    double el = 0;
    double er = 0;
    double ql = _dedx_adapt24(func,set,a,mid,f1,f2,acc,eps,&el);
    double qr = _dedx_adapt24(func,set,mid,b,f3,f4,acc,eps,&er);
    *err = sqrt(el*el+er*er);
    return ql+qr;
  }
}
double _dedx_adapt(double (*func)(double x,_dedx_tools_settings * set),
		   _dedx_tools_settings * set,
		   double a, double b, double acc, double eps, 
		   double *err)
{
  double h = b-a;
  return _dedx_adapt24(func,set,a,b,(*func)(a+h/3,set),(*func)(a+2*h/3,set),acc,eps,err);
}

double _dedx_adapt_stp(double energy, _dedx_tools_settings * set)
{
	int err = 0;
	// TODO: i am sure sure if its cfg->ion_a or ion_A !!
	return 1/dedx_get_stp(set->ws,set->cfg,energy/(set->cfg->ion_a),&err);
}

double _dedx_find_min_stp_func(double x,_dedx_tools_settings * set)
{
	int err = 0;
	float stp = 1/dedx_get_stp(set->ws, set->cfg,x,&err);
	if(err != 0)
		return INFINITY;
	return stp;
}
double _dedx_find_min(double (*func)(double x, _dedx_tools_settings * set),_dedx_tools_settings * set, double acc)
{
	double x[] = {0.01,10};
	double f[] = {func(x[0],set),func(x[1],set)};
	int i = 0;
	double x_temp = 0;
	double f_temp = 0;
	double h = 0;
	while(fabs(x[1]-x[0])>acc)
	{
		i = 0;
		h = x[1]-x[0];
		if(f[1] > f[0])
			i = 1;
		//try flip

		x_temp = x[i]+2*h*pow(-1,i);
		f_temp = func(x_temp,set);
		if(f_temp > f[i])
		{
			//try shrink
			x_temp = x[i]+0.5*h*pow(-1,i);
			f_temp = func(x_temp,set);

			if(f_temp > f[i])
				return -1;
		}
		f[i] = f_temp;
		x[i] = x_temp;
	}	
	return (x[0]+x[1])/2;
}
double dedx_get_inverse_csda(dedx_workspace *ws,dedx_config * config,float range,int *err)
{
	if(config->ion_a <= 0)
	{
		*err = 209;
		return -1;
	}
	double acc = 1e-5;
	double min = dedx_get_min_energy(config->program,config->ion);
	double max = dedx_get_max_energy(config->program,config->ion);

	double x_temp;
	double f_temp;
	
	while((max-min)>acc)
	{
		x_temp = (max+min)/2;
		f_temp = dedx_get_csda(ws,config,x_temp,err);
		if(*err != 0)
			return -1;
		
		if(f_temp >= range)
		{
			max = x_temp;
		}
		else
		{
			min = x_temp;
		}
	}
	return (min+max)/2;
}
double dedx_get_inverse_stp(dedx_workspace * ws, dedx_config * config,float stp,int side,int *err)
{
	if(config->ion_a <= 0)
	{
		*err = 209;
		return -1;
	}
	double acc = 1e-5;
	_dedx_tools_settings set;
	set.ws = ws;
	if(*err != 0)
		return -1;
	dedx_load_config(ws,config,err);
	//	set.id = config->cfg_id;
	set.cfg = config;
	
	if(*err != 0)
		return -1;
	double max = _dedx_find_min(_dedx_find_min_stp_func,&set,acc*100);
	double x1; 
	double x2;
	double x_temp;
	double f_temp;
	if(side < 0)
	{
		x1 = dedx_get_min_energy(config->program,config->ion);
		x2 = max;
	}
	else
	{
		x2 = max;
		x1 = dedx_get_max_energy(config->program,config->ion);
	}
	while(fabs(x1-x2)>acc)
	{
		x_temp = (x1+x2)/2;
		f_temp = dedx_get_stp(set.ws, set.cfg, x_temp,err);
		
		if(f_temp >= stp)
		{
			x2=x_temp;
		}
		else
		{
			x1=x_temp;
		}
	}
	//free(cfg); might be used later again
	return (x1+x2)/2;
}
double dedx_get_csda(dedx_workspace *ws, dedx_config * config,float energy,int *err)
{
	if(config->ion_a <= 0)
	{
		*err = 209;
		return -1;
	}
	double calculation_error = 0;
	double acc = 1e-6;
	double eps = 1e-6;
	_dedx_tools_settings set;
	double range = 0.0;
	double A = config->ion_a;  // TODO: or nucleon number?

	if(*err != 0)
		return -1;
	if(config->loaded == 0)
		dedx_load_config(ws,config,err);
	if(*err != 0)
		return -1;
	set.cfg = config;
	//set.A = config->nucleon_number;	
	set.ws = ws;
	range = _dedx_adapt(_dedx_adapt_stp,&set,dedx_get_min_energy(config->program,config->ion)*A,energy*A,
			    acc,eps,&calculation_error);
	return range;
}
