#include "Python.h"
#include <dedx.h>

static PyObject * get_simple_dedx(PyObject *self, PyObject *args)
{
	int ion,target,err;
	float energy;
	int ok;
	float stp;
	ok = PyArg_ParseTuple(args, "llf", &ion, &target, &energy);

	stp = dedx_get_simple_stp(ion,target,energy,&err);
	return PyFloat_FromDouble((double)stp);
}
static PyMethodDef LibdedxMethods[] = {
    {"get_dedx",  get_simple_dedx, METH_VARARGS,
     "Execute a shell command."},    
    {NULL, NULL, 0, NULL}       
};
PyMODINIT_FUNC
initlibdedx(void)
{
    (void) Py_InitModule("libdedx", LibdedxMethods);
}
