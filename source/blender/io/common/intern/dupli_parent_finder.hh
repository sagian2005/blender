/* SPDX-FileCopyrightText: 2020 Blender Authors
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once

#include "IO_dupli_persistent_id.hh"

#include "BKE_duplilist.hh"

#include "BLI_map.hh"
#include "BLI_set.hh"

namespace blender::io {

/* Find relations between duplicated objects. This class should be instanced for a single real
 * object, and fed its dupli-objects. */
class DupliParentFinder final {
 private:
  /* To check whether an Object * is instanced by this duplicator. */
  blender::Set<const Object *> dupli_set_;

  /* To find the DupliObject given its Persistent ID. */
  using PIDToDupliMap = blender::Map<const PersistentID, const DupliObject *>;
  PIDToDupliMap pid_to_dupli_;

  /* Mapping from instancer PID to duplis instanced by it. */
  using InstancerPIDToDuplisMap =
      blender::Map<const PersistentID, blender::Set<const DupliObject *>>;
  InstancerPIDToDuplisMap instancer_pid_to_duplis_;

 public:
  void insert(const DupliObject *dupli_ob);

  bool is_duplicated(const Object *object) const;
  const DupliObject *find_suitable_export_parent(const DupliObject *dupli_ob) const;

 private:
  const DupliObject *find_duplicated_parent(const DupliObject *dupli_ob) const;
  const DupliObject *find_instancer(const DupliObject *dupli_ob) const;
};

}  // namespace blender::io
