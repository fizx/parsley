#include <Python.h>
#include <parsley.h>
#include <libxslt/xslt.h>
#include <libexslt/exslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxml/parser.h>
#include <libxml/HTMLparser.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlwriter.h>
#include <string.h>
#include <stdio.h>
#include <json/json.h>
#include <xml2json.h>

typedef struct {
	PyObject_HEAD
	parsleyPtr parsley;
} PyParsley;

static PyTypeObject pyparsley_PyParsleyType;

static PyMethodDef pyparsley_methods[] = {
    {NULL}  /* Sentinel */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

static PyObject *jsonmodule;

PyMODINIT_FUNC
initpyparsley(void) 
{
		jsonmodule = PyImport_ImportModule("json");
		if(jsonmodule == NULL)
			return NULL;
			
    PyObject* m;

    pyparsley_PyParsleyType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&pyparsley_PyParsleyType) < 0)
        return;

    m = Py_InitModule3("pyparsley", pyparsley_methods,
                       "Python binding for parsley");

    Py_INCREF(&pyparsley_PyParsleyType);
    PyModule_AddObject(m, "PyParsley", (PyObject *)&pyparsley_PyParsleyType);
}

static void
PyParsley_dealloc(PyParsley* self)
{
		if(self->parsley != NULL) parsley_free(self->parsley);
    self->ob_type->tp_free((PyObject*)self);
}

static PyObject *
PyParsley_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	PyParsley *self;
	self = (PyParsley *)type->tp_alloc(type, 0);
	return (PyObject *)self;	
}

static int
PyParsley_init(PyParsley *self, PyObject *args, PyObject *kwds)
{
 	PyObject *script;
	char *string = "";
	char *incl = "";
		
	if (!PyArg_ParseTuple(args, "O|S", &script, &incl)) {
		Py_DECREF(self);
		return -1;
	}
	
	PyObject *dumps = PyObject_GetAttrString(jsonmodule, "dumps");
	if(dumps == NULL) return -1;
	
	if(PyDict_Check(script)){
		script = PyObject_CallFunctionObjArgs(dumps, script, NULL);
		if(script == NULL) return -1;
	} 
	
	string = PyString_AsString(script);
	if(string == NULL) return -1;

	self->parsley = parsley_compile(string, incl);
	
	if(self->parsley->error != NULL) {
		PyErr_SetString(PyExc_RuntimeError, self->parsley->error);
		Py_DECREF(self);
		return -1;
	}
	
  return 0;
}


static PyObject *
pythonize_recurse(xmlNodePtr xml) {
  if(xml == NULL) return NULL;
  xmlNodePtr child;
	PyObject * obj = NULL;
	
  switch(xml->type) {
    case XML_ELEMENT_NODE:
      child = xml->children;
      if(xml->ns == NULL) {
       	child = xml;
        obj = PyDict_New();
        while(child != NULL) {
          PyDict_SetItemString(obj, child->name, pythonize_recurse(child->children));
          child = child->next;
        }
      } else if(!strcmp(xml->ns->prefix, "parsley")) {
        if(!strcmp(xml->name, "groups")) {
          obj = PyList_New(0);
          while(child != NULL) {
            PyList_Append(obj, pythonize_recurse(child->children));
            child = child->next;
          }          
        } else if(!strcmp(xml->name, "group")) {
          // Implicitly handled by parsley:groups handler
        }
      }
      break;
    case XML_TEXT_NODE:
      obj = Py_BuildValue("s", xml->content);
      break;
  }
	if(obj == NULL) {		
		Py_INCREF(Py_None);
		return Py_None;
	}
  return obj;
}

static PyObject *
PyParsley_parse_doc(parsedParsleyPtr ptr, char *type) {
	if(ptr->error != NULL || ptr->xml == NULL) {
		if(ptr->error == NULL) ptr->error = strdup("Unknown parsley error");
		PyErr_SetString(PyExc_RuntimeError, ptr->error);
    parsed_parsley_free(ptr);
		return NULL;
	}
	
 	PyObject *output;
	if(!strcmp(type, "json")) {
		struct json_object *json = xml2json(ptr->xml->children->children);
		char* str = json_object_to_json_string(json);
		output = Py_BuildValue("s", str);
		json_object_put(json);
	} else if(!strcmp(type, "xml")) {
		char* str;
		int size;
		xmlDocDumpMemory(ptr->xml, &str, &size);
		output = Py_BuildValue("s", str);
	} else {
 		output = pythonize_recurse(ptr->xml->children->children);
		if(output == NULL){
			Py_INCREF(Py_None);
			return Py_None;
		}
	}
	parsed_parsley_free(ptr);
	return output;
}

static PyObject *
PyParsley_parse(PyParsley *self, PyObject *args, PyObject *keywords)
{
	char *file = NULL;
	char *string = NULL;
	char *input = "html";
	char *output = "python";
	int len;
	parsedParsleyPtr ptr;
	
	static char * list[] = { "file", "string", "input", "output", NULL };
	
	if (!PyArg_ParseTupleAndKeywords(args, keywords, 
		"|ss#ss", list, &file, &string, &len, &input, &output)) {
		return NULL;
	}
	
	if(self->parsley == NULL) {
		PyErr_SetString(PyExc_RuntimeError, "parsley data is NULL");
		return NULL;
	}
	
	if(file != NULL) {
		ptr = parsley_parse_file(self->parsley, file, !strcmp(input, "html"));
	} else {
		ptr = parsley_parse_string(self->parsley, string, len, !strcmp(input, "html"));
	}	
	
	return PyParsley_parse_doc(ptr, output);
}


static PyMethodDef PyParsley_methods[] = {	
	  {"parse", (PyCFunction)PyParsley_parse, METH_VARARGS | METH_KEYWORDS,
	   "Parses with a variety of options"
	  },
    // {"parse_string", (PyCFunction)PyParsley_parse_string, METH_VARARGS,
    //  "Parses an in-memory string with the current parslet"
    // },
    // {"parse_file", (PyCFunction)PyParsley_parse_file, METH_VARARGS,
    //  "Parses file or url with the current parslet"
    // },
    {NULL}  /* Sentinel */
};

static PyTypeObject pyparsley_PyParsleyType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "pyparsley.PyParsley",           /*tp_name*/
    sizeof(PyParsley), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor) PyParsley_dealloc,                         /*tp_dealloc*/
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
    "PyParsley objects",           /* tp_doc */
		0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
		PyParsley_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)PyParsley_init,      /* tp_init */
    0,                         /* tp_alloc */
    PyParsley_new,                 /* tp_new */
};