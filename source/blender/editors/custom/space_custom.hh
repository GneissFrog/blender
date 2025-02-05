/* SPDX-FileCopyrightText: 2024 Your Name
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup spcustom
 */

#pragma once

#include "BLI_listbase.h"

struct SpaceCustom {
  struct SpaceLink *next, *prev;
  /** Storage of regions for inactive spaces. */
  ListBase regionbase;
  /** The specific space-type the editor belongs to. */
  char spacetype;
  char link_flag;
  char _pad0[6];
};
