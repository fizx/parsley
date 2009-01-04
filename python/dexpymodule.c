#include <Python.h>
#include <dexter.h>
#include <libxslt/xslt.h>
#include <libexslt/exslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>
#include <dexter.h>
#include <string.h>
#include <json/json.h>
#include <xml2json.h>

typedef struct {
	PyObject_HEAD
	dexPtr dex;
} DexPy;

static PyTypeObject dexpy_DexPyType;

static PyMethodDef dexpy_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

PyMODINIT_FUNC
initdexpy(void) 
{
    PyObject* m;

    dexpy_DexPyType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&dexpy_DexPyType) < 0)
        return;

    m = Py_InitModule3("dexpy", dexpy_methods,
                       "Python binding for dexter");

    Py_INCREF(&dexpy_DexPyType);
    PyModule_AddObject(m, "DexPy", (PyObject *)&dexpy_DexPyType);
}

static void
DexPy_dealloc(DexPy* self)
{
		if(self->dex != NULL) dex_free(self->dex);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
DexPy_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	DexPy *self;
	self = (DexPy *)type->tp_alloc(type, 0);
	return (PyObject *)self;	
}

static int
DexPy_init(DexPy *self, PyObject *args, PyObject *kwds)
{
	char *script = "";
	char *incl = "";
		
	if (!PyArg_ParseTuple(args, "S|S", &script, &incl)) {
		Py_DECREF(self);
		return NULL;
	}
	
	self->dex = dex_compile(script, incl);
	
	if(self->dex->error != NULL) {
		PyErr_SetString(PyExc_RuntimeError, self->dex->error);
		Py_DECREF(self);
		return NULL;
	}
	
  return 0;
}


static PyObject *
pythonize_recurse(xmlNodePtr xml) {
  if(xml == NULL) return NULL;
  xmlNodePtr child;
	static PyObject * obj = NULL;

  switch(xml->type) {
    case XML_ELEMENT_NODE:
      child = xml->children;
      if(xml->ns == NULL) {
        child = xml;
        obj = PyDict_New();
        while(child != NULL) {
          PyDict_SetItem(obj, Py_BuildValue("S", child->name), pythonize_recurse(child->children));
          child = child->next;
        }
      } else if(!strcmp(xml->ns->prefix, "dexter")) {
        if(!strcmp(xml->name, "groups")) {
          obj = PyList_New(0);
          while(child != NULL) {
            PyList_Append(obj, pythonize_recurse(child->children));
            child = child->next;
          }          
        } else if(!strcmp(xml->name, "group")) {
          // Implicitly handled by dexter:groups handler
        }
      }
      break;
    case XML_TEXT_NODE:
      obj = Py_BuildValue("S", xml->content);
      break;
  }
	if(obj == NULL) {		
		Py_INCREF(Py_None);
		return Py_None;
	}
  return obj;
}

static PyObject *
DexPy_parse_doc(dexPtr dex, xmlDocPtr xml, char *type) {
	if(xml == NULL) {
		if(dex->error == NULL) {
			dex->error = strdup("Unknown dex error");
		}
		PyErr_SetString(PyExc_RuntimeError, dex->error);
		free(dex->error);
		dex->error = NULL;
		return NULL;
	}
	
 	PyObject *output;
	if(!strcmp(type, "json")) {
		struct json_object *json = xml2json(xml->children->children);
		char* str = json_object_to_json_string(json);
		output = Py_BuildValue("S", str);
		json_object_put(json);
	} else if(!strcmp(type, "xml")) {
		char* str;
		int size;
		xmlDocDumpMemory(xml, &str, &size);
		output = Py_BuildValue("S", str);
	} else {
 		output = pythonize_recurse(xml->children->children);
		if(output == NULL){
			Py_INCREF(Py_None);
			return Py_None;
		}
	}
	return output;
}

static PyObject *
DexPy_parse(DexPy *self, PyObject *args, PyObject *keywords)
{
	char *file = NULL;
	char *string = NULL;
	char *input = "python";
	char *output = "python";
	int len;
	
	static char * list[] = { "file", "string", "input", "output", NULL };
	
	if (!PyArg_ParseTupleAndKeywords(args, keywords, 
		"|ss#ss", list, &file, &string, &len, &input, &output)) {
		return NULL;
	}
	
	if(file != NULL) {
		return DexPy_parse_doc(self->dex, dex_parse_file(self->dex, file, !strcmp(input, "html")), output);
	} else {
		return DexPy_parse_doc(self->dex, dex_parse_string(self->dex, string, len, !strcmp(input, "html")), output);
	}
}


static PyMethodDef DexPy_methods[] = {	
	  {"parse", (PyCFunction)DexPy_parse, METH_VARARGS | METH_KEYWORDS,
	   "Parses with a variety of options"
	  },
    // {"parse_string", (PyCFunction)DexPy_parse_string, METH_VARARGS,
    //  "Parses an in-memory string with the current dex"
    // },
    // {"parse_file", (PyCFunction)DexPy_parse_file, METH_VARARGS,
    //  "Parses file or url with the current dex"
    // },
    {NULL}  /* Sentinel */
};

static PyTypeObject dexpy_DexPyType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "dexpy.DexPy",           /*tp_name*/
    sizeof(DexPy), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor) DexPy_dealloc,                         /*tp_dealloc*/
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
    "DexPy objects",           /* tp_doc */
		0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
		DexPy_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)DexPy_init,      /* tp_init */
    0,                         /* tp_alloc */
    DexPy_new,                 /* tp_new */
};