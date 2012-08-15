/*
  This file is part of libdedx.

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
#include "dedx_mpaul.h"

float _dedx_calculate_mspaul_coef(char  mode, int ion, int target, float energy)
{
  if(ion == 2)
    return 1;
  float output;
  int err = 0;
  float f;
  f = _dedx_read_effective_charge(target, &err);

  double a = 5.0;
  double b = -1;
  double c = -1;
  //double d;
  double FACTOR;
  float FOUT = 0.0;

  int z2 = (int)(f + 0.5);
  if(mode == 'd' && z2 <= 3)
  {
  	mode = 'c';
  }
  if(mode == 'h' && z2 < 3)
  {
	  mode = 'g';
  }
  if(mode == 'c' || mode == 'd')
  {
	  a =  152.3281067 -509.175005*pow(ion,0.5)
		  + 764.0597161 * ion -666.456225 * pow(ion,1.5)
		  + 369.6135044 * pow(ion,2)-134.506826* pow(ion,2.5)
		  + 32.02328376 * pow(ion,3.0) -4.80247840 * pow(ion,3.5)
		  + 0.411498619 * pow(ion,4.0) -0.015353889 * pow(ion,4.5);

	  b =  (-1.60299361 +2.594689067 * pow(ion,0.5)
			  - 1.73972608 * ion +0.601973528 * pow(ion,1.5)
			  - 0.10683182 * pow(ion,2.0) +0.0077231543*pow(ion,2.5))
		  / (1.0 -1.44605189 * pow(ion,0.5) +0.871321669*ion
				  - 0.24591206 * pow(ion,1.5) +0.027726119 * pow(ion,2.0)
				  - 0.00042121782 * pow(ion,2.5));

	  c =  1.0  / (0.15607719 -0.0023161329 * pow(ion,2) * log(ion)
			  + 0.0016853943 * pow(ion,2.5));
  }
  else if(mode == 'g' && z2 > 2)
  {
	  a =  38.150347 - 5.0998443 * ion - 151.25405 / ion
		  + 0.3682134 * pow(ion,2) + 301.77474 / pow(ion,2)
		  - 1.3542411E-2 * pow(ion,3) - 225.08009 / pow(ion,3)
		  + 1.9888596E-4 * pow(ion,4);

	  b =  -  0.1983923
		  +  3.2653672E-2 * ion
		  -  12.281832 * exp(-ion);

	  c =  - 1.3058678
		  -  0.5460869* pow(log(ion),2)
		  +  2.3894315*log(ion);
	  c =  exp(c);
  }
  else if(mode == 'g' && z2 <= 2)
  {

	  a = 82.333704 - 1.7353839E2 * pow(ion,0.5)
		  + 1.4994474E2 * ion    - 67.608335* pow(ion,1.5)
		  + 1.6771605E1 * pow(ion,2) - 2.1732999*pow(ion,2.5)
		  + 1.1514042E-1 * pow(ion,3);

	  b = 0.74789173 - 1.6701292 / log(ion);

	  c = 3.2370042 - 0.049755185 * ion;
  }
  else if(mode == 'h' && ion == 3)
  {
	  a = 0.59251;
	  b =-0.71211;
	  c = 1.91956;
  }

  else if(mode == 'h' && ion == 4)
  {
	  a = 0.35030;
	  b =-0.28291;
	  c = 2.92675;
  }
  else if(mode == 'h' && ion == 5)
  {
	  a = 0.32192;
	  b =-0.13151;
	  c = 2.97854;
  }
  else if(mode == 'h' && ion == 6)
  {
	  a = 0.27061;
	  b =-0.01952;
	  c = 2.80761;
  }

  else if(mode == 'h' && ion == 7)
  {
	  a = 0.21054;
	  b = 0.01528;
	  c = 3.99705;
  }


  else if(mode == 'h' && ion == 8)
  {
	  a = 0.16969;
	  b = 0.1075;
	  c = 3.75755;
  }

  else if(mode == 'h' && ion == 9)
  {
	  a = 0.12192;
	  b = 0.08955;
	  c = 3.59459;
  }

  else if(mode == 'h' && ion == 10)
  {
	  a = 0.09358;
	  b = 0.08692;
	  c = 3.39734;
  }
  else if(mode == 'h' && ion == 11)
  {
	  a = 0.06353;
	  b = 0.21303;
	  c = 4.29082;
  }

  else if(mode == 'h' && ion == 16)
  {
	  a = 0.05698;
	  b = 0.34491;
	  c = 3.01333;
  }
  else if(mode == 'h' && ion == 17)
  {
	  a = 0.05163;
	  b = 0.37948;
	  c = 2.81286;
  }
  else if(mode == 'h' && ion == 18) {

	  a = 0.048;
	  b = 0.38887;
	  c = 2.84076;
  }
  else{

	  //illegal mode

  }

  if(mode == 'c' || mode == 'd')
  {
	  double X   =log10(energy);

	  double D1  =17.18567-.65734*ion;

	  FOUT=1.01-a;

	  double TP  =(X+c*pow(log(2.0),1/D1)-b)/c;
	  if(TP<0) TP=0;
	  FOUT=FOUT*(1.0-exp(-pow(TP,D1)));
	  FOUT=a+FOUT;
	  FOUT=(1.0-0.0041466419-0.0010098198*exp(-X))*FOUT;

	  FACTOR = 1.0;

	  if(mode == 'd' && z2 > 4 && z2 < 10)
	  {


		  FACTOR = 1.0 + 0.0012171149 - 0.04021975/(1.0 +
				  pow(((X + 2.0388341)/0.48193661),2.0));
	  }
	  else if(mode == 'd' && z2 > 10 && z2 < 19)
	  {
		  FACTOR = 1.0 + 0.003491232 - 0.11423452/(1.0 +
				  pow(((X + 1.5906109)/0.41741941),2.0));
	  }


	  else if(mode == 'd' && z2 > 19 && z2 < 36)
	  {
		  FACTOR = 1.0;
	  }
	  else if(mode == 'd' && z2 > 36 && z2 < 62)
	  {
		  FACTOR = 1.0 - 0.0030928609 + 0.11488954/(1.0 +
				  pow(((X + 1.4359953)/0.37177939),2.0));
	  }



	  else if(mode == 'd' && z2 > 62 && z2 < 93)
	  {
		  /*a0 = -0.069155044;
		    b0 = -0.65644678;
		    c0 =  0.18379076;
		    d0 = -0.0010104478;
		    n0 =  pow(d0,2.0) + pow(c0,2.0);

		    FACTOR =1.0 + a0 * c0 * exp(-0.5*pow((X - b0),2.0)/n0)*
		    (1.0D0 + MSERF1 (d0*(x - b0)/((2.0D0*n0)**0.5D0*c0)))/
		    n0**0.5D0*/
	  }

	  else if(mode == 'd')
	  {
		  //illegal mode
	  }

	  FOUT=FOUT * FACTOR;

  }
  else if(mode == 'g' || mode == 'h')
  {

	  double X   = log10(energy);
	  double D1  =9.1963-0.29643*ion;

	  FOUT=1.01-a;

	  double TP  =(X+c*pow((log(2.0)),(1/D1))-b)/c;
	  if(TP<0)
		  TP=0;
	  FOUT=FOUT*(1.0-exp(-pow(TP,D1)));

	  FOUT=a+FOUT;


  }
  output=FOUT*pow(ion,2)/4.0;
  return output;
}
