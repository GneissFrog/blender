/* SPDX-FileCopyrightText: 2024 Your Name
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup spcustom
 */

#pragma once

#include "BLI_compiler_attrs.h"
#include "DNA_listbase.h"  // For ListBase
#include "DNA_space_types.h"

struct ARegion;
struct bContext;
struct wmNotifier;
struct wmMsgBus;
struct wmWindowManager;
struct wmSpaceTypeListenerParams;
struct wmRegionListenerParams;
struct SpaceLink;  // Added this forward declaration

/* Space Custom */
typedef struct Custom {
  SpaceLink *next, *prev;
  ListBase regionbase; /* Storage of regions for inactive spaces */
  int spacetype;
  /* End 'SpaceLink' header */
} Custom;

/* Functions */
void ED_spacetype_custom(void);
