#include "Python.h"
#include <dedx.h>

static PyObject * get_simple_stp(PyObject *self, PyObject *args)
{
	int ion,target,err;
	float energy;
	float stp;
	PyArg_ParseTuple(args, "llf", &ion, &target, &energy);

	stp = dedx_get_simple_stp(ion,target,energy,&err);
	return PyFloat_FromDouble((double)stp);
}
static PyObject * get_stp(PyObject *self, PyObject *args)
{
	int prog,ion,target,err;
	float energy;
	float stp;
	dedx_config * config;
	dedx_workspace *ws;
	PyArg_ParseTuple(args, "lllf",&prog, &ion, &target, &energy);
	config = calloc(1,sizeof(dedx_config));
	ws = dedx_allocate_workspace(1,&err);
	config->program = prog;
	config->ion = ion;
	config->target = target;
	dedx_load_config(ws,config,&err);
	stp = dedx_get_stp(ws,config,energy,&err);
	
	dedx_free_workspace(ws,&err);
	dedx_free_config(config,&err);
	return PyFloat_FromDouble((double)stp);
}
static PyMethodDef LibdedxMethods[] = {
    {"get_simple_stp",  get_simple_stp, METH_VARARGS}, 
    {"get_stp",  get_stp, METH_VARARGS}, 
    {NULL, NULL, 0, NULL}       
};
PyMODINIT_FUNC
initlibdedx(void)
{
    (void) Py_InitModule("libdedx", LibdedxMethods);
}
