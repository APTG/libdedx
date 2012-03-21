#include <Python.h>
#include <stdio.h>
#include <dedx.h>
#include "structmember.h"

typedef struct{ 
	PyObject_HEAD
	dedx_config * config;
	dedx_workspace *ws;
	int err;

} Libdedx;

static void
Libdedx_dealloc(Libdedx* self)
{
	int err = 0;
	dedx_free_workspace(self->ws,&err);
	dedx_free_config(self->config,&err);
	self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
Libdedx_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{

	Libdedx *self;
	int program, ion, target;
	static char *kwlist[] = {"program","ion","target",};
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|lll", kwlist, &program, &ion, &target))
	        return NULL;
	self = (Libdedx *)type->tp_alloc(type, 0);
	self->ws = dedx_allocate_workspace(1,&(self->err));
	if(self->err != 0)
		return NULL;
	self->config = (dedx_config *)calloc(1,sizeof(dedx_config));
	dedx_config * config = self->config;
	config->program = program;
	config->ion = ion;
	config->target = target;
	dedx_load_config(self->ws,config,&(self->err));
	if(self->err != 0)
		return NULL;
	return (PyObject *)self;
}

static int
Libdedx_init(Libdedx *self, PyObject *args, PyObject *kwds)
{
	return 0;
}
static PyObject *
Libdedx_get_stp(Libdedx * self,PyObject * args)
{
	float energy,stp;
	PyArg_ParseTuple(args, "f",&energy);
	stp = dedx_get_stp(self->ws,self->config,energy,&(self->err));
	if(self->err != 0)
		return NULL;
	return PyFloat_FromDouble((double)stp);
}
static PyObject * Libdedx_get_error(Libdedx * self)
{
	char * err = calloc(100,sizeof(char));
	PyObject * out;
	if(self->err == 0)
		err = "No error";
	else
	{
		dedx_get_error_code(err,self->err);
	}
	out = PyString_FromString(err);
	free(err);
	return out;
}
static PyMemberDef Libdedx_members[] = {
	{NULL}  /* Sentinel */
};



static PyMethodDef Libdedx_methods[] = {
     {"get_stp", (PyCFunction)Libdedx_get_stp, METH_VARARGS,
     "Return stp"
    },{"get_error", (PyCFunction)Libdedx_get_error, METH_NOARGS,
     "Return error string"
    },
	{NULL}  /* Sentinel */
};

static PyTypeObject LibdedxType = {
	PyObject_HEAD_INIT(NULL)
		0,                         /*ob_size*/
	"libdedx.Libdedx",             /*tp_name*/
	sizeof(Libdedx),             /*tp_basicsize*/
	0,                         /*tp_itemsize*/
	(destructor)Libdedx_dealloc, /*tp_dealloc*/
	0,                         /*tp_print*/
	0,                         /*tp_getattr*/
	0,                         /*tp_setattr*/
	0,                         /*tp_compare*/
	0,                         /*tp_repr*/
	0,                         /*tp_as_number*/
	0,                         /*tp_as_sequence*/
	0,                         /*tp_as_mapping*/
	0,                         /*tp_hash */
	0,                         /*tp_call*/
	0,                         /*tp_str*/
	0,                         /*tp_getattro*/
	0,                         /*tp_setattro*/
	0,                         /*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"Libdedx objects",           /* tp_doc */
	0,		               /* tp_traverse */
	0,		               /* tp_clear */
	0,		               /* tp_richcompare */
	0,		               /* tp_weaklistoffset */
	0,		               /* tp_iter */
	0,		               /* tp_iternext */
	Libdedx_methods,             /* tp_methods */
	Libdedx_members,             /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)Libdedx_init,      /* tp_init */
	0,                         /* tp_alloc */
	Libdedx_new,                 /* tp_new */
};

static PyMethodDef module_methods[] = {
	{NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initlibdedx(void) 
{
	PyObject* m;

	if (PyType_Ready(&LibdedxType) < 0)
		return;

	m = Py_InitModule3("libdedx", module_methods,
			"Example module that creates an extension type.");

	if (m == NULL)
		return;

	Py_INCREF(&LibdedxType);
	PyModule_AddObject(m, "Libdedx", (PyObject *)&LibdedxType);
}
