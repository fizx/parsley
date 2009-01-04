#ifndef DEXPYMODULE_H_INCLUDED
#define DEXPYMODULE_H_INCLUDED

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

#include <Python.h>

typedef struct {
	PyObject_HEAD
	dexPtr dex;
} CDexter;

static PyMethodDef dexpy_methods[] = {
    {NULL}  /* Sentinel */
};

static PyMemberDef CDexter_members[] = {
    {NULL}  /* Sentinel */
};

static PyMethodDef CDexter_methods[] = {
    {"parse_string", (PyCFunction)CDexter_parse_string, METH_VARARGS,
     "Parses an in-memory string with the current dex"
    },
    {"parse_file", (PyCFunction)CDexter_parse_file, METH_VARARGS,
     "Parses file or url with the current dex"
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject dexpy_CDexterType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "dexpy.CDexter",           /*tp_name*/
    sizeof(CDexter), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
		0, 												 /*tp_dealloc*/
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
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "CDexter objects",           /* tp_doc */
		0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
		CDexter_methods,             /* tp_methods */
    CDexter_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,							      /* tp_init */
    0,                         /* tp_alloc */
		0,                 /* tp_new */
};




#endif