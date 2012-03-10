from libdedx import Libdedx
a = Libdedx(program=2,ion=1,target=276)
for i in range(10000000):
	a.get_stp(10)
