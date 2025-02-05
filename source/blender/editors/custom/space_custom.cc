/* SPDX-FileCopyrightText: 2024 Your Name
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup spcustom
 */

#include <cstring>
#include <stdio.h>
#include "MEM_guardedalloc.h"

#include "BLI_listbase.h"
#include "BLI_string.h"
#include "BLI_string_utils.hh"
#include "BLI_utildefines.h"

#include "BKE_context.hh"
#include "BKE_screen.hh"

/* Add missing includes */
#include "DNA_scene_types.h"
#include "DNA_space_types.h" /* For SPACE_CUSTOM definition */
#include "DNA_userdef_types.h"

#include "ED_screen.hh"
#include "ED_space_api.hh"

#include "WM_api.hh"
#include "WM_types.hh"

#include "UI_interface.hh"
#include "UI_resources.hh"
#include "UI_view2d.hh"

#include "space_custom.hh"

/* Note: Some header files in Blender use .h extension while others use .hh */

/* ******************** default callbacks for custom space ***************** */

static SpaceLink *custom_create(const ScrArea * /*area*/, const Scene * /*scene*/)
{
  SpaceCustom *custom = static_cast<SpaceCustom *>(MEM_callocN(sizeof(SpaceCustom), "initcustom"));
  custom->spacetype = SPACE_CUSTOM;

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

static void custom_main_region_draw(const bContext *C, ARegion *region)
{
  // Draw background
  UI_ThemeClearColor(TH_BACK);
  UI_view2d_view_ortho(&region->v2d);

  // Get font style for drawing
  const uiFontStyle *fstyle = UI_FSTYLE_WIDGET;

  // Position text in the upper left of the region
  int x = 10;
  int y = region->winy - 20;

  // Define text color (white)
  const uchar text_col[4] = {255, 255, 255, 255};

  // Draw the text with color parameter
  UI_fontstyle_draw_simple(fstyle, x, y, "Custom Editor", text_col);
}

static void custom_header_draw(const bContext *C, ARegion *region)
{
  const uiStyle *style = UI_style_get();

  uiBlock *block = UI_block_begin(C, region, __func__, UI_EMBOSS);

  uiLayout *layout = UI_block_layout(
      block, UI_LAYOUT_HORIZONTAL, UI_LAYOUT_HEADER, 0, 0, region->winx, HEADERY, 0, style);

  uiItemL(layout, "Custom Editor", ICON_NONE);

  UI_block_layout_resolve(block, nullptr, nullptr);
  UI_block_end(C, block);
}

static void custom_main_region_init(wmWindowManager *wm, ARegion *region)
{
  UI_view2d_region_reinit(&region->v2d, V2D_COMMONVIEW_HEADER, region->winx, region->winy);
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

static void custom_keymap(wmKeyConfig *keyconf)
{
  wmKeyMap *keymap = WM_keymap_ensure(keyconf, "Custom Editor", SPACE_CUSTOM, 0);

  KeyMapItem_Params params{};
  params.type = EVT_CKEY;
  params.value = KM_PRESS;
  params.modifier = 0;
  params.keymodifier = 0;

  WM_keymap_add_item(keymap, "SCREEN_OT_space_context_cycle", &params);
}

/* ******************** registration ********************/

void ED_spacetype_custom()
{
  printf("Starting custom editor registration...\n");

  std::unique_ptr<SpaceType> st = std::make_unique<SpaceType>();
  ARegionType *art;

  st->spaceid = SPACE_CUSTOM;
  STRNCPY(st->name, "Custom");

  st->create = custom_create;
  st->free = custom_free;
  st->init = custom_init;
  st->duplicate = custom_duplicate;

  printf("Registered basic space type functions for custom editor\n");

  /* regions: main window */
  art = MEM_cnew<ARegionType>("spacetype custom region");
  art->regionid = RGN_TYPE_WINDOW;
  art->keymapflag = ED_KEYMAP_UI | ED_KEYMAP_VIEW2D;
  art->init = custom_main_region_init;
  art->draw = custom_main_region_draw;
  BLI_addhead(&st->regiontypes, art);

  printf("Added main region to custom editor\n");

  /* regions: header */
  art = MEM_cnew<ARegionType>("spacetype custom header");
  art->regionid = RGN_TYPE_HEADER;
  art->prefsizey = HEADERY;
  art->draw = custom_header_draw;
  BLI_addhead(&st->regiontypes, art);

  printf("Added header region to custom editor\n");

  BKE_spacetype_register(std::move(st));
  printf("Completed custom editor registration\n");
}
