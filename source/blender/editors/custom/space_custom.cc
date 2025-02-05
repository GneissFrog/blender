/* SPDX-FileCopyrightText: 2024 Your Name
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup spcustom
 */

#include <cstring>

#include "MEM_guardedalloc.h"

#include "BLI_listbase.h"
#include "BLI_utildefines.h"

#include "BKE_context.hh"
#include "BKE_screen.hh"

#include "ED_screen.hh"
#include "ED_space_api.hh"

#include "WM_api.hh"
#include "WM_types.hh"

#include "UI_resources.hh"
#include "UI_view2d.hh"

#include "space_custom.hh"

/* ******************** default callbacks for custom space ***************** */

static SpaceLink *custom_create(const ScrArea * /*area*/, const Scene * /*scene*/)
{
  SpaceCustom *custom = static_cast<SpaceCustom *>(MEM_callocN(sizeof(SpaceCustom), "initcustom"));
  custom->spacetype = SPACE_CUSTOM;  // You'll need to define this in DNA_space_types.h

  /* Header */
  ARegion *region = BKE_area_region_new();
  BLI_addtail(&custom->regionbase, region);
  region->regiontype = RGN_TYPE_HEADER;
  region->alignment = (U.uiflag & USER_HEADER_BOTTOM) ? RGN_ALIGN_BOTTOM : RGN_ALIGN_TOP;

  /* Main region */
  region = BKE_area_region_new();
  BLI_addtail(&custom->regionbase, region);
  region->regiontype = RGN_TYPE_WINDOW;

  return (SpaceLink *)custom;
}

static void custom_free(SpaceLink * /*sl*/)
{
  // Add cleanup code if needed
}

static void custom_init(wmWindowManager * /*wm*/, ScrArea * /*area*/)
{
  // Add initialization code if needed
}

static SpaceLink *custom_duplicate(SpaceLink *sl)
{
  SpaceCustom *custom_new = static_cast<SpaceCustom *>(MEM_dupallocN(sl));
  return (SpaceLink *)custom_new;
}

/* ******************** registration ********************/

void ED_spacetype_custom()
{
  std::unique_ptr<SpaceType> st = std::make_unique<SpaceType>();
  ARegionType *art;

  st->spaceid = SPACE_CUSTOM;  // You'll need to define this in DNA_space_types.h
  STRNCPY(st->name, "Custom");

  st->create = custom_create;
  st->free = custom_free;
  st->init = custom_init;
  st->duplicate = custom_duplicate;

  /* Main region */
  art = static_cast<ARegionType *>(MEM_callocN(sizeof(ARegionType), "custom region"));
  art->regionid = RGN_TYPE_WINDOW;
  art->init = nullptr;  // Add initialization if needed
  art->draw = nullptr;  // Add draw function if needed
  BLI_addhead(&st->regiontypes, art);

  /* Header */
  art = static_cast<ARegionType *>(MEM_callocN(sizeof(ARegionType), "custom header region"));
  art->regionid = RGN_TYPE_HEADER;
  art->prefsizey = HEADERY;
  art->init = nullptr;  // Add initialization if needed
  art->draw = nullptr;  // Add draw function if needed
  BLI_addhead(&st->regiontypes, art);

  BKE_spacetype_register(std::move(st));
}
