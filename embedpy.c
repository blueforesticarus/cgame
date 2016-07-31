#include <python2.7/Python.h>
#include <stdint.h>
#include "parser.c"

static PyObject* cgame_move_entity(PyObject* self, PyObject* args)
{
    uintptr_t pos_loc;
    int dir;

	PyArg_ParseTuple(args, "Ki", &pos_loc, &dir);
    move_vec(pos_loc, dir); 
    return Py_BuildValue("");
}

static PyObject* cgame_get_mob_vec(PyObject* self, PyObject* args)
{
    extern vec2 mobpos;
    return Py_BuildValue("K", &mobpos);
}

static PyMethodDef cgame_methods[] = {
	{"move_entity", cgame_move_entity, METH_VARARGS, NULL},
	{"get_mob_vec", cgame_get_mob_vec, METH_VARARGS, NULL},
	{NULL, NULL, 0, NULL}
};

void init_pycgame(void)
{
  Py_InitModule3("_cgame", cgame_methods, "cgame interface");
}

void* embedpy(void* vp)
{
  Py_Initialize();
  PyImport_AppendInittab("_cgame",init_pycgame);
  PyEval_InitThreads();
  PyRun_SimpleString("execfile('mob.py')\n");
  Py_Finalize();
}

void embedit2()
{
  PyRun_SimpleString("import code; code.interact()\n");
}
