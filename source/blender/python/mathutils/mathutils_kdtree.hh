/* SPDX-FileCopyrightText: 2023 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup mathutils
 */

#pragma once

#include <Python.h>

PyMODINIT_FUNC PyInit_mathutils_kdtree();

extern PyTypeObject PyKDTree_Type;
