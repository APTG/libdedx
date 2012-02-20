#include <dedx_tools.h>
#include <stdio.h>

int main()
{
	int err = 0;
	int program = 2; //PSTAR
	int ion = 1; //Proton
	int target = 276; //Water
	double A = 1; //Nucleid number of ion
	float energy = 100; //Energy per nucleos in MeV
	float csda = dedx_get_csda(program,ion,A,target,energy,&err);
	printf("Calculated csda %6.3Ecm^2/g\n",csda);
	return 0;
}
