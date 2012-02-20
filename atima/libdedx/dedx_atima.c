#include "dedx_atima.h"
float _dedx_bethek(float energy,int PZ, int PA, int TZ, float TA, float pot,float rho);
float _dedx_dedxela(float energy,int PZ, int PA, int TZ, float TA);
float _dedx_sezi(float energy,int PZ, int PA, int TZ, float TA);
float _dedx_rpstop(int TZ, float energy);
float _dedx_bf4(int TZ,float eta, float zpeff);

float _dedx_calculate_atima_energy(float energy,dedx_config * config)
{
	int * err = 0;
	float PZ, PA, TZ, TA, rho, pot;	
	float dedx = 0;
		float factor;

	PZ = _dedx_get_atom_charge(config->ion,err);
	PA = _dedx_get_atom_mass(config->ion,err);
	TZ = _dedx_get_atom_charge(config->target,err);
	TA = _dedx_get_atom_mass(config->target,err);
	float e_tot = PA*energy*1000;

	if(energy <= 10)
	{
		dedx = _dedx_sezi(e_tot,PZ,PA,TZ,TA) + _dedx_dedxela(e_tot,PZ,PA,TZ,TA);
	}
	else if(energy <= 30)
	{
		factor = 0.05*(energy-10);
		dedx = (1-factor)*_dedx_sezi(e_tot,PZ,PA,TZ,TA)+factor*_dedx_bethek(e_tot,PZ,PA,TZ,TA,pot,rho)+_dedx_dedxela(e_tot,PZ,PA,TZ,TA);
	}
	else
	{
		dedx = _dedx_bethek(e_tot,PZ,PA,TZ,TA,pot,rho)+_dedx_dedxela(e_tot,PZ,PA,TZ,TA);
	}
	return dedx;
}
float _dedx_bethek(float energy,int PZ, int PA, int TZ, float TA, float pot, float rho)
{
	float e = energy/PA;
	float e1 = energy/(1000*PA);
	float gamma,bsq,zeta,eta,zpeff,f1,f2,f4,f6;
	float c,beta;
	
	gamma = 1+e1/DEDX_AMU;
	bsq = 1-1/pow(gamma,2);
	beta = sqrt(bsq);
	zeta = 1- exp(-130 * beta/(pow(PZ,2/3)));
	zpeff = zeta*PZ;
	eta = beta*gamma;

	f1 = 4e-5*DEDX_PI*DEDX_ECHARGE2*DEDX_ECHARGE2*DEDX_N_AVO/DEDX_EMASS*pow(zpeff,2)*TZ/(bsq*TA);
	f2 = log(2*DEDX_EMASS*1e6*bsq/pot);
	if(eta >= 0.13)
	{       
	  c = (.422377*pow(eta,-2)+.0304043*pow(eta,-4)-.00038106*pow(eta,-6))*1e-6*pow(pot,2)
	+(3.858019*pow(eta,-2) - 0.1667989*(pow(eta,-4))+ 0.00157955*(pow(eta,-6)))*1.0e-9*pow(pot,3);
		f2 = f2 -c/TZ;
	}
	f4 = _dedx_bf4(TZ,eta,zpeff);
	f6 = 2*log(gamma)-bsq;
	return 0;	
}
float _dedx_bf4(int TZ,float eta, float zpeff)
{
	return 0;
}
float _dedx_dedxela(float energy,int PZ, int PA, int TZ, float TA)
{
	float sn,a;	
	float epsil = 32.53*TA*energy/(PZ*TZ*(PA+TA)*(pow(PZ,0.23)+pow(TZ,.23)));
	if(epsil >= 30)
	{
		sn = log(epsil)/(2*epsil);
	}
	else
	{
		a = .01321*pow(epsil,0.21226)+.19593*pow(epsil,0.5);
		sn = 0.5*log(1+1.1383*epsil)/(epsil+a);
	}	
	sn = sn*PZ*TZ*PA*8.462/((PA+TA)*pow(PZ,0.23)+pow(TZ,0.23));
      	sn = sn*.1*DEDX_N_AVO/TA;
   	return sn;
}
float _dedx_sezi(float energy,int PZ, int PA, int TZ, float TA)
{	
	float dedx = 0;;
	int err = 0;
	float * ion_info;
	ion_info = _dedx_get_atima_data(PZ,&err);
	float * target_info;
	target_info = _dedx_get_atima_data(TZ,&err);
	float v_fermi = target_info[1];
	float lfctr = ion_info[2];
	float e = energy/PA;
	if(PZ == 1)
	{
		dedx = _dedx_rpstop(TZ,e) * 0.60222/TA;
	}
	else if(PZ == 2)
	{	
		float E = e;
		float he0 = 1;
		float B,A,heh;
		if(e < he0)
			E = he0;
		B=log(E);
		A = 0.2865 + 0.1266*B - .001429*B*B+0.02402*B*B*B-0.01135*B*B*B*B + 0.001475*B*B*B*B*B;
		if(A<30)
			heh = 1-exp(-1*A);
		else
			heh = 1-exp(-30);
		if(log(E)< 0)
			A = (1.0+(0.007 + 0.00005 *TZ)*exp(-pow(7.6,2)));
		else
			A = (1.0+(0.007 + 0.00005 *TZ)*exp(-pow(7.6-log(E),2)));
		heh = heh*A*A;
		dedx = _dedx_rpstop(TZ,e)*heh*4;
		if(e <= he0)
			dedx = dedx*sqrt(e/he0);
		dedx = dedx * 0.602222/TA;
	}
	else if(PZ > 2)
	{
		float vr,yr,help,power,sp;
		float a,q,b,l,l0,l1,q1,eee;
		float zeta;
		float yrmin = 0.130;
		float vrmin = 1.0;
		float vmin,help1,help2,help3,help4,help5;
		float v = sqrt(e/25.0)/v_fermi;
		float v_2 = v*v;
		if (v > 1.0) 
		{
			vr = v*v_fermi*(1+1/(5*v_2));
		}
		else
		{
			vr = 3*v_fermi/4*(1+(2/3*v_2)-(v_2*v_2/15));
		}
		yr = _dedx_max(vr/pow(PZ,0.67),_dedx_max(vrmin/pow(PZ,0.67),yrmin));
		help = vr / pow(PZ,0.6667);
		yr = _dedx_max(yrmin,help); 
		help = vrmin/(pow(PZ,0.6667));
		yr = _dedx_max(yr,help);
		a = -0.803*pow(yr,0.3) + 1.3167*pow(yr,0.6)+00.38157*yr+0.008983*yr*yr;
		q = _dedx_min(1,_dedx_max(0,(1-exp(-1*_dedx_min(a,50)))));
		b= (_dedx_min(0.43,_dedx_max(0.32,0.12+0.25*PZ)))/pow(PZ,0.3333);
		l0 = (0.8-q*_dedx_min(1.2,0.6+PZ/30))/pow(PZ,0.3333);
		if(q < 0.2)
		{
			l1 = 0;
		}
		else if(q < _dedx_max(0,0.9-0.025*PZ))
		{
			q1 = 0.2;
			l1 = b*(q-0.2)/fabs(_dedx_max(0,0.9-0.025*PZ)-0.2000001);
		}
		else if(q < _dedx_max(0,1-0.025*_dedx_min(16,PZ)))
		{
			l1 = b;
		}
		else
		{
			l1 = b*(1-q)/(0.025*_dedx_min(16,PZ));
		}
		l = _dedx_max(l1,l0*lfctr);
		zeta = q + (1/(2*v_fermi*v_fermi))*(1-q)*log(1+pow((4*l*v_fermi/1.919),2));
		a = -pow(7.6-_dedx_max(0,log(e)),2);
		zeta = zeta*(1+1/pow(PZ,2))*(0.18+0.0015*TZ)*exp(a);
		if(yr <= _dedx_max(yrmin,vrmin/pow(PZ,0.6667)))
		{
			vmin=0.5*(vrmin+sqrt(_dedx_max(0,vrmin*vrmin-0.8*v_fermi*v_fermi)));
			eee = 25*vmin*vmin;
			power = 0.5;
			if(TZ == 6 || ((TZ == 14 || TZ == 32) && PZ <= 19))
			{
				power = 0.35;
			}
			sp = _dedx_rpstop(TZ,e);
			help1 = zeta*PZ;
			help2 = help1*help1;
			help3 = e/eee;
			help4 = pow(help3,power);
			help5 = help2*help4;
			dedx = sp*help5;
			dedx = dedx*0.60222/TA;
		}
		else
		{
			dedx = _dedx_rpstop(TZ,e)*(pow(zeta*PZ,2)*0.60222)/TA;
		}
	}

	return dedx;
}

float _dedx_rpstop(int TZ, float energy)
{
	float sl,sh,sp;
	float pe0 = 25;
	float velpwr;
	float E = energy;
	const float * coef;
	coef = dedx_pcoef[TZ];
	if(E < pe0)
		E = pe0;
	sl = coef[0]*pow(E,coef[1])+coef[2]*pow(E,coef[3]);
	sh = coef[4]/(pow(E,coef[5]))*log(coef[6]/E+coef[7]*E);
	sp = sl*sh/(sl+sh);
	if (E <= pe0) 
	{ 
		if(TZ > 6)
		{
			velpwr = 0.45;
		}
		else
		{
			velpwr = 0.25;
		}        
		sp = sp*pow((E/pe0),velpwr); 
	}
	return sp;
}
