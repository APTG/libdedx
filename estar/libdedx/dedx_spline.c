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
#include "dedx_spline.h"
int _dedx_linear_search(_dedx_spline_base * coef, float value)
{
    int i;
    for(i = 0; i < _DEDX_MAXELEMENTS; i++)
    {
        if(coef[i].x >= value)
        {
            break;
        }
    }
    return i;
}
int _dedx_binary_search(_dedx_spline_base * coef, float value, int n)
{
    int head = n-1;
    int tail = 0;
    int guess = n/2;
    while(head != tail+1)
    {
        if(coef[guess].x <= value)
        {
            tail = guess;
        }
        else
        {
            head = guess;
        }
        guess = (head - tail)/2 + tail;
    }
    return guess;
}



void _dedx_calculate_coefficients(_dedx_spline_base * coef, float * energy, float * stopping, int n)
{
    int i;
    float h[_DEDX_MAXELEMENTS];
    float alpha[_DEDX_MAXELEMENTS];
    float l[_DEDX_MAXELEMENTS];
    float my[_DEDX_MAXELEMENTS];
    float z[_DEDX_MAXELEMENTS];

    l[0]=1;
    my[0]=0;
    z[0]=0;

    l[n-1] = 1;
    z[n-1]=0;
    coef[n-1].c = 0;

    for(i = 0; i < n; i++)
    {
        coef[i].a = stopping[i];
        coef[i].x = energy[i];
    }
    for(i = 0; i < n-1; i++)
    {
        h[i] = energy[i+1]-energy[i];
    }
    for(i = 1; i < n -1; i++)
    {
        alpha[i] = 3/h[i]*(stopping[i+1]-stopping[i])-3/h[i-1]*(stopping[i]-stopping[i-1]);
    }
    for(i=1; i < n-1; i++)
    {
        l[i]=2*(energy[i+1]-energy[i-1])-h[i-1]*my[i-1];
        my[i]=h[i]/l[i];
        z[i]=(alpha[i]-h[i-1]*z[i-1])/l[i];

    }
    for(i = n-2; i >= 0; i--)
    {
        coef[i].c = z[i] -my[i]*coef[i+1].c;
        coef[i].b = (coef[i+1].a-coef[i].a)/h[i]-h[i]*(coef[i+1].c+2*coef[i].c)/3;
        coef[i].d = (coef[i+1].c-coef[i].c)/(3*h[i]);
    }
}

float _dedx_evaluate_spline(_dedx_spline_base * coef, float x, _dedx_lookup_accelerator * acc, int n)
{
    int i;
    int lookup = 1;
    if(acc != NULL)
    {
      // Next line looks weird.
      // if((coef[acc->cache].x <= x) & x < coef[acc->cache+1].x) 
      // TBC: Just a guess what Jakob meant?
      if((coef[acc->cache].x <= x) && (x < coef[acc->cache+1].x))
        {
            lookup = 0;
            i = acc->cache;
        }
    }
    if(lookup)
    {
        i = _dedx_binary_search(coef,x,n);
        if(acc != NULL)
            acc->cache = i;
    }
    float energy = coef[i].a+coef[i].b*(x-coef[i].x)+coef[i].c*pow(x-coef[i].x,2)+coef[i].d*pow(x-coef[i].x,3);
    return energy;
}
