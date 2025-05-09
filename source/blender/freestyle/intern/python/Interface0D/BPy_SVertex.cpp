/* SPDX-FileCopyrightText: 2004-2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup freestyle
 */

#include "BPy_SVertex.h"

#include "../BPy_Convert.h"
#include "../BPy_Id.h"
#include "../Interface1D/BPy_FEdge.h"

#include "BLI_sys_types.h"

using namespace Freestyle;

///////////////////////////////////////////////////////////////////////////////////////////

/*----------------------SVertex methods ----------------------------*/

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_doc,
    "Class hierarchy: :class:`Interface0D` > :class:`SVertex`\n"
    "\n"
    "Class to define a vertex of the embedding.\n"
    "\n"
    ".. method:: __init__()\n"
    "            __init__(brother)\n"
    "            __init__(point_3d, id)\n"
    "\n"
    "   Builds a :class:`SVertex` using the default constructor,\n"
    "   copy constructor or the overloaded constructor which builds"
    "   a :class:`SVertex` from 3D coordinates and an Id.\n"
    "\n"
    "   :arg brother: A SVertex object.\n"
    "   :type brother: :class:`SVertex`\n"
    "   :arg point_3d: A three-dimensional vector.\n"
    "   :type point_3d: :class:`mathutils.Vector`\n"
    "   :arg id: An Id object.\n"
    "   :type id: :class:`Id`");

static int SVertex_init(BPy_SVertex *self, PyObject *args, PyObject *kwds)
{
  static const char *kwlist_1[] = {"brother", nullptr};
  static const char *kwlist_2[] = {"point_3d", "id", nullptr};
  PyObject *obj = nullptr;
  float v[3];

  if (PyArg_ParseTupleAndKeywords(args, kwds, "|O!", (char **)kwlist_1, &SVertex_Type, &obj)) {
    if (!obj) {
      self->sv = new SVertex();
    }
    else {
      self->sv = new SVertex(*(((BPy_SVertex *)obj)->sv));
    }
  }
  else if ((void)PyErr_Clear(),
           PyArg_ParseTupleAndKeywords(
               args, kwds, "O&O!", (char **)kwlist_2, convert_v3, v, &Id_Type, &obj))
  {
    Vec3r point_3d(v[0], v[1], v[2]);
    self->sv = new SVertex(point_3d, *(((BPy_Id *)obj)->id));
  }
  else {
    PyErr_SetString(PyExc_TypeError, "invalid argument(s)");
    return -1;
  }
  self->py_if0D.if0D = self->sv;
  self->py_if0D.borrowed = false;
  return 0;
}

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_add_normal_doc,
    ".. method:: add_normal(normal)\n"
    "\n"
    "   Adds a normal to the SVertex's set of normals. If the same normal\n"
    "   is already in the set, nothing changes.\n"
    "\n"
    "   :arg normal: A three-dimensional vector.\n"
    "   :type normal: :class:`mathutils.Vector` | tuple[float, float, float] | list[float]");

static PyObject *SVertex_add_normal(BPy_SVertex *self, PyObject *args, PyObject *kwds)
{
  static const char *kwlist[] = {"normal", nullptr};
  PyObject *py_normal;
  Vec3r n;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", (char **)kwlist, &py_normal)) {
    return nullptr;
  }
  if (!Vec3r_ptr_from_PyObject(py_normal, n)) {
    PyErr_SetString(PyExc_TypeError,
                    "argument 1 must be a 3D vector (either a list of 3 elements or Vector)");
    return nullptr;
  }
  self->sv->AddNormal(n);
  Py_RETURN_NONE;
}

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_add_fedge_doc,
    ".. method:: add_fedge(fedge)\n"
    "\n"
    "   Add an FEdge to the list of edges emanating from this SVertex.\n"
    "\n"
    "   :arg fedge: An FEdge.\n"
    "   :type fedge: :class:`FEdge`");

static PyObject *SVertex_add_fedge(BPy_SVertex *self, PyObject *args, PyObject *kwds)
{
  static const char *kwlist[] = {"fedge", nullptr};
  PyObject *py_fe;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", (char **)kwlist, &FEdge_Type, &py_fe)) {
    return nullptr;
  }
  self->sv->AddFEdge(((BPy_FEdge *)py_fe)->fe);
  Py_RETURN_NONE;
}

// virtual bool     operator== (const SVertex &brother)

#ifdef __GNUC__
#  ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wcast-function-type"
#  else
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wcast-function-type"
#  endif
#endif

static PyMethodDef BPy_SVertex_methods[] = {
    {"add_normal",
     (PyCFunction)SVertex_add_normal,
     METH_VARARGS | METH_KEYWORDS,
     SVertex_add_normal_doc},
    {"add_fedge",
     (PyCFunction)SVertex_add_fedge,
     METH_VARARGS | METH_KEYWORDS,
     SVertex_add_fedge_doc},
    {nullptr, nullptr, 0, nullptr},
};

#ifdef __GNUC__
#  ifdef __clang__
#    pragma clang diagnostic pop
#  else
#    pragma GCC diagnostic pop
#  endif
#endif

/*----------------------mathutils callbacks ----------------------------*/

/* subtype */
#define MATHUTILS_SUBTYPE_POINT3D 1
#define MATHUTILS_SUBTYPE_POINT2D 2

static int SVertex_mathutils_check(BaseMathObject *bmo)
{
  if (!BPy_SVertex_Check(bmo->cb_user)) {
    return -1;
  }
  return 0;
}

static int SVertex_mathutils_get(BaseMathObject *bmo, int subtype)
{
  BPy_SVertex *self = (BPy_SVertex *)bmo->cb_user;
  switch (subtype) {
    case MATHUTILS_SUBTYPE_POINT3D:
      bmo->data[0] = self->sv->getX();
      bmo->data[1] = self->sv->getY();
      bmo->data[2] = self->sv->getZ();
      break;
    case MATHUTILS_SUBTYPE_POINT2D:
      bmo->data[0] = self->sv->getProjectedX();
      bmo->data[1] = self->sv->getProjectedY();
      bmo->data[2] = self->sv->getProjectedZ();
      break;
    default:
      return -1;
  }
  return 0;
}

static int SVertex_mathutils_set(BaseMathObject *bmo, int subtype)
{
  BPy_SVertex *self = (BPy_SVertex *)bmo->cb_user;
  switch (subtype) {
    case MATHUTILS_SUBTYPE_POINT3D: {
      Vec3r p(bmo->data[0], bmo->data[1], bmo->data[2]);
      self->sv->setPoint3D(p);
      break;
    }
    case MATHUTILS_SUBTYPE_POINT2D: {
      Vec3r p(bmo->data[0], bmo->data[1], bmo->data[2]);
      self->sv->setPoint2D(p);
      break;
    }
    default:
      return -1;
  }
  return 0;
}

static int SVertex_mathutils_get_index(BaseMathObject *bmo, int subtype, int index)
{
  BPy_SVertex *self = (BPy_SVertex *)bmo->cb_user;
  switch (subtype) {
    case MATHUTILS_SUBTYPE_POINT3D:
      switch (index) {
        case 0:
          bmo->data[0] = self->sv->getX();
          break;
        case 1:
          bmo->data[1] = self->sv->getY();
          break;
        case 2:
          bmo->data[2] = self->sv->getZ();
          break;
        default:
          return -1;
      }
      break;
    case MATHUTILS_SUBTYPE_POINT2D:
      switch (index) {
        case 0:
          bmo->data[0] = self->sv->getProjectedX();
          break;
        case 1:
          bmo->data[1] = self->sv->getProjectedY();
          break;
        case 2:
          bmo->data[2] = self->sv->getProjectedZ();
          break;
        default:
          return -1;
      }
      break;
    default:
      return -1;
  }
  return 0;
}

static int SVertex_mathutils_set_index(BaseMathObject *bmo, int subtype, int index)
{
  BPy_SVertex *self = (BPy_SVertex *)bmo->cb_user;
  switch (subtype) {
    case MATHUTILS_SUBTYPE_POINT3D: {
      Vec3r p(self->sv->point3D());
      p[index] = bmo->data[index];
      self->sv->setPoint3D(p);
      break;
    }
    case MATHUTILS_SUBTYPE_POINT2D: {
      Vec3r p(self->sv->point2D());
      p[index] = bmo->data[index];
      self->sv->setPoint2D(p);
      break;
    }
    default:
      return -1;
  }
  return 0;
}

static Mathutils_Callback SVertex_mathutils_cb = {
    SVertex_mathutils_check,
    SVertex_mathutils_get,
    SVertex_mathutils_set,
    SVertex_mathutils_get_index,
    SVertex_mathutils_set_index,
};

static uchar SVertex_mathutils_cb_index = -1;

void SVertex_mathutils_register_callback()
{
  SVertex_mathutils_cb_index = Mathutils_RegisterCallback(&SVertex_mathutils_cb);
}

/*----------------------SVertex get/setters ----------------------------*/

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_point_3d_doc,
    "The 3D coordinates of the SVertex.\n"
    "\n"
    ":type: :class:`mathutils.Vector`");

static PyObject *SVertex_point_3d_get(BPy_SVertex *self, void * /*closure*/)
{
  return Vector_CreatePyObject_cb(
      (PyObject *)self, 3, SVertex_mathutils_cb_index, MATHUTILS_SUBTYPE_POINT3D);
}

static int SVertex_point_3d_set(BPy_SVertex *self, PyObject *value, void * /*closure*/)
{
  float v[3];
  if (mathutils_array_parse(v, 3, 3, value, "value must be a 3-dimensional vector") == -1) {
    return -1;
  }
  Vec3r p(v[0], v[1], v[2]);
  self->sv->setPoint3D(p);
  return 0;
}

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_point_2d_doc,
    "The projected 3D coordinates of the SVertex.\n"
    "\n"
    ":type: :class:`mathutils.Vector`");

static PyObject *SVertex_point_2d_get(BPy_SVertex *self, void * /*closure*/)
{
  return Vector_CreatePyObject_cb(
      (PyObject *)self, 3, SVertex_mathutils_cb_index, MATHUTILS_SUBTYPE_POINT2D);
}

static int SVertex_point_2d_set(BPy_SVertex *self, PyObject *value, void * /*closure*/)
{
  float v[3];
  if (mathutils_array_parse(v, 3, 3, value, "value must be a 3-dimensional vector") == -1) {
    return -1;
  }
  Vec3r p(v[0], v[1], v[2]);
  self->sv->setPoint2D(p);
  return 0;
}

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_id_doc,
    "The Id of this SVertex.\n"
    "\n"
    ":type: :class:`Id`");

static PyObject *SVertex_id_get(BPy_SVertex *self, void * /*closure*/)
{
  Id id(self->sv->getId());
  return BPy_Id_from_Id(id);  // return a copy
}

static int SVertex_id_set(BPy_SVertex *self, PyObject *value, void * /*closure*/)
{
  if (!BPy_Id_Check(value)) {
    PyErr_SetString(PyExc_TypeError, "value must be an Id");
    return -1;
  }
  self->sv->setId(*(((BPy_Id *)value)->id));
  return 0;
}

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_normals_doc,
    "The normals for this Vertex as a list. In a sharp surface, an SVertex\n"
    "has exactly one normal. In a smooth surface, an SVertex can have any\n"
    "number of normals.\n"
    "\n"
    ":type: list of :class:`mathutils.Vector`");

static PyObject *SVertex_normals_get(BPy_SVertex *self, void * /*closure*/)
{
  PyObject *py_normals;
  set<Vec3r> normals = self->sv->normals();
  set<Vec3r>::iterator it;
  py_normals = PyList_New(normals.size());
  uint i = 0;

  for (it = normals.begin(); it != normals.end(); it++) {
    Vec3r v(*it);
    PyList_SET_ITEM(py_normals, i++, Vector_from_Vec3r(v));
  }
  return py_normals;
}

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_normals_size_doc,
    "The number of different normals for this SVertex.\n"
    "\n"
    ":type: int");

static PyObject *SVertex_normals_size_get(BPy_SVertex *self, void * /*closure*/)
{
  return PyLong_FromLong(self->sv->normalsSize());
}

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_viewvertex_doc,
    "If this SVertex is also a ViewVertex, this property refers to the\n"
    "ViewVertex, and None otherwise.\n"
    "\n"
    ":type: :class:`ViewVertex`");

static PyObject *SVertex_viewvertex_get(BPy_SVertex *self, void * /*closure*/)
{
  ViewVertex *vv = self->sv->viewvertex();
  if (vv) {
    return Any_BPy_ViewVertex_from_ViewVertex(*vv);
  }
  Py_RETURN_NONE;
}

PyDoc_STRVAR(
    /* Wrap. */
    SVertex_curvatures_doc,
    "Curvature information expressed in the form of a seven-element tuple\n"
    "(K1, e1, K2, e2, Kr, er, dKr), where K1 and K2 are scalar values\n"
    "representing the first (maximum) and second (minimum) principal\n"
    "curvatures at this SVertex, respectively; e1 and e2 are\n"
    "three-dimensional vectors representing the first and second principal\n"
    "directions, i.e. the directions of the normal plane where the\n"
    "curvature takes its maximum and minimum values, respectively; and Kr,\n"
    "er and dKr are the radial curvature, radial direction, and the\n"
    "derivative of the radial curvature at this SVertex, respectively.\n"
    "\n"
    ":type: tuple");

static PyObject *SVertex_curvatures_get(BPy_SVertex *self, void * /*closure*/)
{
  const CurvatureInfo *info = self->sv->getCurvatureInfo();
  if (!info) {
    Py_RETURN_NONE;
  }
  Vec3r e1(info->e1.x(), info->e1.y(), info->e1.z());
  Vec3r e2(info->e2.x(), info->e2.y(), info->e2.z());
  Vec3r er(info->er.x(), info->er.y(), info->er.z());
  PyObject *retval = PyTuple_New(7);
  PyTuple_SET_ITEMS(retval,
                    PyFloat_FromDouble(info->K1),
                    PyFloat_FromDouble(info->K2),
                    Vector_from_Vec3r(e1),
                    Vector_from_Vec3r(e2),
                    PyFloat_FromDouble(info->Kr),
                    Vector_from_Vec3r(er),
                    PyFloat_FromDouble(info->dKr));
  return retval;
}

static PyGetSetDef BPy_SVertex_getseters[] = {
    {"point_3d",
     (getter)SVertex_point_3d_get,
     (setter)SVertex_point_3d_set,
     SVertex_point_3d_doc,
     nullptr},
    {"point_2d",
     (getter)SVertex_point_2d_get,
     (setter)SVertex_point_2d_set,
     SVertex_point_2d_doc,
     nullptr},
    {"id", (getter)SVertex_id_get, (setter)SVertex_id_set, SVertex_id_doc, nullptr},
    {"normals", (getter)SVertex_normals_get, (setter) nullptr, SVertex_normals_doc, nullptr},
    {"normals_size",
     (getter)SVertex_normals_size_get,
     (setter) nullptr,
     SVertex_normals_size_doc,
     nullptr},
    {"viewvertex",
     (getter)SVertex_viewvertex_get,
     (setter) nullptr,
     SVertex_viewvertex_doc,
     nullptr},
    {"curvatures",
     (getter)SVertex_curvatures_get,
     (setter) nullptr,
     SVertex_curvatures_doc,
     nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr} /* Sentinel */
};

/*-----------------------BPy_SVertex type definition ------------------------------*/

PyTypeObject SVertex_Type = {
    /*ob_base*/ PyVarObject_HEAD_INIT(nullptr, 0)
    /*tp_name*/ "SVertex",
    /*tp_basicsize*/ sizeof(BPy_SVertex),
    /*tp_itemsize*/ 0,
    /*tp_dealloc*/ nullptr,
    /*tp_vectorcall_offset*/ 0,
    /*tp_getattr*/ nullptr,
    /*tp_setattr*/ nullptr,
    /*tp_as_async*/ nullptr,
    /*tp_repr*/ nullptr,
    /*tp_as_number*/ nullptr,
    /*tp_as_sequence*/ nullptr,
    /*tp_as_mapping*/ nullptr,
    /*tp_hash*/ nullptr,
    /*tp_call*/ nullptr,
    /*tp_str*/ nullptr,
    /*tp_getattro*/ nullptr,
    /*tp_setattro*/ nullptr,
    /*tp_as_buffer*/ nullptr,
    /*tp_flags*/ Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    /*tp_doc*/ SVertex_doc,
    /*tp_traverse*/ nullptr,
    /*tp_clear*/ nullptr,
    /*tp_richcompare*/ nullptr,
    /*tp_weaklistoffset*/ 0,
    /*tp_iter*/ nullptr,
    /*tp_iternext*/ nullptr,
    /*tp_methods*/ BPy_SVertex_methods,
    /*tp_members*/ nullptr,
    /*tp_getset*/ BPy_SVertex_getseters,
    /*tp_base*/ &Interface0D_Type,
    /*tp_dict*/ nullptr,
    /*tp_descr_get*/ nullptr,
    /*tp_descr_set*/ nullptr,
    /*tp_dictoffset*/ 0,
    /*tp_init*/ (initproc)SVertex_init,
    /*tp_alloc*/ nullptr,
    /*tp_new*/ nullptr,
};

///////////////////////////////////////////////////////////////////////////////////////////
