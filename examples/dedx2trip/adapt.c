/*
 * adapt.c
 *
 *  Created on: Jun 4, 2011
 *      Author: ricky
 */

#include "adapt.h"

real adapt24(real (*func)(real),real a,real b,real f2,real f3,real acc,real eps,real *error) {
	real h=b-a,f1,f4;
#pragma omp parallel
	{
#pragma omp sections
		{
#pragma omp section
			f1=(*func)(a+h/6);
#pragma omp section
			f4=(*func)(a+5*h/6);
		}
	}
	real q4=(f1+f4)*h/3+(f2+f3)*h/6;
	real q2=(f1+f2+f3+f4)*h/4;
	real tol=acc+eps*fabs(q4); *error=fabs(q4-q2)/3;
	if(*error < tol) return q4;
	else	{
		real i1,i2,error1,error2;
		{
			i1=adapt24(func,a,(a+b)/2,f1,f2,acc/sqrt(2.),eps,&error1);
			i2=adapt24(func,(a+b)/2,b,f3,f4,acc/sqrt(2.),eps,&error2);
		}
		*error=sqrt(error1*error1+error2*error2);
		return i1+i2; }
}

real adapt(real (*func)(real),real a,real b,real acc,real eps,real *error){
	real f2=(*func)(a+(b-a)/3); real f3=(*func)(a+2*(b-a)/3);
	real integ=adapt24(func,a,b,f2,f3,acc,eps,error);
	return integ; }
