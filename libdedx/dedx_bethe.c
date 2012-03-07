/*
    libdedx is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libdedx is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libdedx.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dedx_bethe.h"
#include <math.h>


void _dedx_gold_section(_dedx_bethe_struct bet, _dedx_gold_struct * gold, int *err);
float _dedx_m(float PT,_dedx_bethe_struct bet, int * err);
float _dedx_mm(float PT, _dedx_bethe_struct bet, _dedx_gold_struct gold, int * err);

float _dedx_calculate_bethe_energy(_dedx_bethe_coll_struct * ws, float energy, float PZ, float PA, float TZ, float TA, float rho, float Io_Pot)
{
	
	float dedx;
	_dedx_gold_struct * gold = ws->gold;
	_dedx_bethe_struct * bet = ws->bet;
	int err = 0;
	if(gold != NULL && bet != NULL && (bet->PZ0 == PZ && bet->PA0 == PA && bet->TZ0 == TZ && bet->TA0 == TA && bet->rho == rho && bet->potentiale == Io_Pot))
	{
		dedx = _dedx_mm(energy*PA, *bet,*gold,&err);
		return dedx;
	}
	else
	{
		if(ws->gold != NULL)
			free(ws->gold);
		if(ws->bet != NULL)
			free(ws->bet);
		ws->gold = calloc(1,sizeof(_dedx_gold_struct));
		ws->bet = calloc(1,sizeof(_dedx_bethe_struct));
		gold = ws->gold;
		bet = ws->bet;
	}
	//float mass = 940*PA;

	gold->e_min = 1E-5;
	gold->e_max = 1E1;
	gold->epsilon = 1E-5;
	gold->h = 1E-5;
	gold->e_extr = 0;
	gold->f_extr = 0;


	bet->PZ0 = PZ;
	bet->TA0 = TA;
	bet->rho = rho;
	bet->potentiale = Io_Pot;
	bet->TZ0 = TZ;
	bet->PA0 = PA;

	_dedx_gold_section(*bet,gold,&err);

	dedx = _dedx_mm(energy*PA, *bet,*gold,&err);

	return dedx;
}
float _dedx_mm(float PT, _dedx_bethe_struct bet, _dedx_gold_struct gold, int * err)
{
	double T = PT;
	float dedx;
	double mass = 940*bet.PA0;
	//Lindhard-Scharff formula
	if(T  <= gold.e_sewn*bet.PA0)
	{
		dedx=(gold.f_sewn/sqrt(gold.e_sewn*bet.PA0))*sqrt(T);
		return dedx;
	}


	//Kinematic
	double momentum=sqrt(T*(T+2.*mass));
	double e0=T+mass;
	double gamma_2=pow(e0/mass,2);
	double beta_2=(gamma_2-1)/gamma_2;
	double beta_gamma=beta_2*gamma_2;
	//double w_max=pow(momentum,2)/(mass*(mass/1.022+0.511/(2.0*mass)+e0/mass));

	//Density effect

	double x0,x1;
	double x =log10(momentum/mass);

	double hnp=28.8*sqrt(bet.rho*bet.TZ0/bet.TA0);
	double c=-2.0*log(bet.potentiale/hnp)-1.0;
	double cav=-c;
	double xal=cav/4.606;
	double delta;

	if((bet.potentiale -100) >= 0)
	{
		x1=3.0;
		if((cav-5.215) >= 0)
		{
			x0=0.326*cav-1.5;
		}
		else
		{
			x0=0.2;
		}
	}
	else
	{
		x1 = 2.0;
		if((cav - 3.681) >= 0)
		{
			x0=0.326*cav-1.0;
		}
		else
		{
			x0=0.2;
		}
	}
	float alit=4.606*(xal-x0)/pow((x1-x0),3);


	if((x-x0) > 0)
	{
		if((x-x1) > 0)
		{
			delta=4.606*x+c;
		}
		else
		{
			delta=4.606*x+c+alit*pow((x1-x),3);
		}
	}
	else
	{
		delta = 0;
	}

	double DD=1.164+0.2319*exp(-0.004302*bet.TZ0);
	double U1=DD+1.658*exp(-0.05170*bet.PZ0);
	double U2=8.144 +0.09876*log(bet.TZ0);
	double U3=0.3140+0.01072*log(bet.TZ0);
	double U4=0.5218+0.02521*log(bet.TZ0);
	double GAMMA=1.0-U1*exp((-U2*pow((T/bet.PA0),U3))/pow(bet.PZ0,U4));
	double ZPART2 = pow(bet.PZ0,2);
	ZPART2=ZPART2*pow(GAMMA,2);
	dedx=0.307075*bet.TZ0/bet.TA0*(ZPART2/beta_2)*(log((1.022E+06/bet.potentiale)*beta_gamma)-beta_2-delta/2.0);
	return dedx;
}
void _dedx_gold_section(_dedx_bethe_struct bet, _dedx_gold_struct * gold, int * err)
{

	double e_min = gold->e_min;
	double e_max = gold->e_max;
	double e_zero = gold->e_zero;

	double rla= e_min+0.3819661*(e_max-e_min);
	double rmu =e_min+0.6180339*(e_max-e_min);
	double tla=_dedx_m(rla*bet.PA0,bet,err);
	double tmu=_dedx_m(rmu*bet.PA0,bet,err);
	while(1)
	{
		if((e_max-e_min)< gold->epsilon)
		{
			e_zero = (e_max+e_min)*0.5;
			break;
		}
		else
		{
			if(tla > 0.0)
			{
				e_max=rmu;
				rmu=rla;
				tmu=tla;
				rla=e_min+0.3819661*(e_max-e_min);

				tla=_dedx_m(rla*bet.PA0,bet,err);
			}
			else
			{

				e_min=rla;
				rla=rmu;
				tla=tmu;
				rmu=e_min+0.6180339*(e_max-e_min);
				tmu=_dedx_m(rmu*bet.PA0,bet,err);
			}
		}
	}

	e_min = e_zero;
	e_max = gold->e_max;

	rla=e_min+0.3819661*(e_max-e_min);
	rmu=e_min+0.6180339*(e_max-e_min);
	tla=_dedx_m(rla*bet.PA0,bet,err);
	tmu=_dedx_m(rmu*bet.PA0,bet,err);

	while(1)
	{
		if((e_max-e_min) < gold->epsilon)
		{
			gold->e_extr=(e_min+e_max)*0.5;
			gold->f_extr=_dedx_m(gold->e_extr*bet.PA0,bet,err);
			break;
		}
		else
		{
			if(tla > tmu)
			{
				e_max=rmu;
				rmu=rla;
				tmu=tla;
				rla=e_min+0.3819661*(e_max-e_min);
				tla=_dedx_m(rla*bet.PA0,bet,err);
			}
			else
			{
				e_min=rla;
				rla=rmu;
				tla=tmu;
				rmu=e_min+0.6180339*(e_max-e_min);
				tmu=_dedx_m(rmu*bet.PA0,bet,err);
			}
		}
	}
	e_min=gold->e_zero;
	e_max=gold->e_extr;

	rla=e_min+0.3819661*(e_max-e_min);
	rmu=e_min+0.6180339*(e_max-e_min);
	tla=(_dedx_m((rla+gold->h)*bet.PA0,bet,err)- _dedx_m((rla-gold->h)*bet.PA0,bet,err))/gold->h-_dedx_m(rla*bet.PA0,bet,err)/rla;
	tmu=(_dedx_m((rmu+gold->h)*bet.PA0,bet,err)- _dedx_m((rmu-gold->h)*bet.PA0,bet,err))/gold->h-_dedx_m(rmu*bet.PA0,bet,err)/rmu;
	while(1)
	{
		if((e_max-e_min) < gold->epsilon)
		{
			gold->e_sewn=(e_min+e_max)*0.5;
			gold->f_sewn=_dedx_m(gold->e_sewn*bet.PA0,bet,err);
			break;
		}
		else
		{
			if(tla <= 0)
			{
				e_max=rmu;
				rmu=rla;
				tmu=tla;
				rla=e_min+0.3819661*(e_max-e_min);
				tla=(_dedx_m((rla+gold->h)*bet.PA0,bet,err)- _dedx_m((rla-gold->h)*bet.PA0,bet,err))/gold->h-_dedx_m(rla*bet.PA0,bet,err)/rla;

			}
			else
			{
				e_min=rla;
				rla=rmu;
				tla=tmu;
				rmu=e_min+0.6180339*(e_max-e_min);
				tmu=(_dedx_m((rmu+gold->h)*bet.PA0,bet,err)- _dedx_m((rmu-gold->h)*bet.PA0,bet,err))/gold->h-_dedx_m(rmu*bet.PA0,bet,err)/rmu;
			}
		}
	}

}
float _dedx_m(float PT,_dedx_bethe_struct bet, int * err)
{
	double T = PT;
	double mass = 940*bet.PA0;

	//Kinematic
	double momentum = sqrt(T*(T+2*mass));
	double e_0 = T+mass;
	double gamma_2 = pow(e_0/mass,2);
	double beta_2 = (gamma_2-1)/gamma_2;
	double gamma_beta = gamma_2*beta_2;
	//double w_max = pow(momentum,2)/(mass*(mass/1.022+0.511/(2.0*mass)+e_0/mass));


	//Density effect

	double x0,x1;
	double x =log10(momentum/mass);

	double hnp=28.8*sqrt(bet.rho*bet.TZ0/bet.TA0);
	double c=-2.0*log(bet.potentiale/hnp)-1.0;
	double cav=-c;
	double xal=cav/4.606;
	double delta;

	if((bet.potentiale -100) >= 0)
	{
		x1=3.0;
		if((cav-5.215) >= 0)
		{
			x0=0.326*cav-1.5;
		}
		else
		{
			x0=0.2;
		}
	}
	else
	{
		x1 = 2.0;
		if((cav - 3.681) >= 0)
		{
			x0=0.326*cav-1.0;
		}
		else
		{
			x0=0.2;
		}
	}
	float alit=4.606*(xal-x0)/pow((x1-x0),3);


	if((x-x0) > 0)
	{
		if((x-x1) > 0)
		{
			delta=4.606*x+c;
		}
		else
		{
			delta=4.606*x+c+alit*pow((x1-x),3);
		}
	}
	else
	{
		delta = 0;
	}

	double DD=1.164+0.2319*exp(-0.004302*bet.TZ0);
	double U1=DD+1.658*exp(-0.05170*bet.PZ0);
	double U2=8.144 +0.09876*log(bet.TZ0);
	double U3=0.3140+0.01072*log(bet.TZ0);
	double U4=0.5218+0.02521*log(bet.TZ0);

	double GAMMA=1.0-U1*exp((-U2*pow((T/bet.PA0),U3))/pow(bet.PZ0,U4));



	double ZPART2 = pow(bet.PZ0,2);

	ZPART2=ZPART2*pow(GAMMA,2);


	float DEDXM=0.307075*bet.TZ0/bet.TA0*(ZPART2/beta_2)*(log((1.022E+06/bet.potentiale)*gamma_beta)-beta_2-delta/2.0);

	return DEDXM;
}

