/* SPDX-FileCopyrightText: 2024 Your Name
 *
 * SPDX-License-Identifier: GPL-2.0-or-later */

/** \file
 * \ingroup spcustom
 */

#include "MEM_guardedalloc.h"
#include <cstring>
#include <stdio.h>

#include "BLI_listbase.h"
#include "BLI_string.h"
#include "BLI_string_utils.hh"
#include "BLI_utildefines.h"

#include "BKE_context.hh"
#include "BKE_screen.hh"

#include "DNA_scene_types.h"
#include "DNA_space_types.h"
#include "DNA_userdef_types.h"

#include "RNA_access.hh"
#include "RNA_define.hh"

#include "ED_screen.hh"
#include "ED_space_api.hh"

#include "WM_api.hh"
#include "WM_types.hh"

#include "UI_interface.hh"
#include "UI_resources.hh"
#include "UI_view2d.hh"
#include "interface_templates_intern.hh"

#include "space_custom.hh"

enum {
  ND_SPACE_CUSTOM = (1 << 0),
};

/* ******************** default callbacks for custom space ***************** */

static SpaceLink *custom_create(const ScrArea * /*area*/, const Scene * /*scene*/)
{
  Custom *custom = static_cast<Custom *>(MEM_callocN(sizeof(Custom), "initcustom"));

  custom->spacetype = SPACE_CUSTOM;

  /* Header */
  ARegion *region = BKE_area_region_new();
  BLI_addtail(&custom->regionbase, region);
  region->regiontype = RGN_TYPE_HEADER;
  region->alignment = (U.uiflag & USER_HEADER_BOTTOM) ? RGN_ALIGN_BOTTOM : RGN_ALIGN_TOP;

  /* Tools region (optional, for toolbar) */
  region = BKE_area_region_new();
  BLI_addtail(&custom->regionbase, region);
  region->regiontype = RGN_TYPE_TOOLS;
  region->alignment = RGN_ALIGN_LEFT;
  region->flag = RGN_FLAG_HIDDEN;

  /* UI region (right panel) */
  region = BKE_area_region_new();
  BLI_addtail(&custom->regionbase, region);
  region->regiontype = RGN_TYPE_UI;
  region->alignment = RGN_ALIGN_RIGHT;
  region->flag = RGN_FLAG_HIDDEN;

  /* Main region */
  region = BKE_area_region_new();
  BLI_addtail(&custom->regionbase, region);
  region->regiontype = RGN_TYPE_WINDOW;

  return (SpaceLink *)custom;
}

static void custom_init(wmWindowManager * /*wm*/, ScrArea * /*area*/) {}

static SpaceLink *custom_duplicate(SpaceLink *sl)
{
  Custom *custom_new = static_cast<Custom *>(MEM_dupallocN(sl));
  return (SpaceLink *)custom_new;
}

static void custom_free(SpaceLink * /*sl*/) {}

static void custom_region_listener(const wmRegionListenerParams *params)
{
  ARegion *region = params->region;
  const wmNotifier *wmn = params->notifier;

  /* Context changes */
  switch (wmn->category) {
    case NC_SPACE:
      if (wmn->data == ND_SPACE_CUSTOM) {
        ED_region_tag_redraw(region);
      }
      break;
  }
}

static void custom_listener(const wmSpaceTypeListenerParams *params)
{
  ScrArea *area = params->area;
  const wmNotifier *wmn = params->notifier;

  switch (wmn->category) {
    case NC_SPACE:
      if (wmn->data == ND_SPACE_CUSTOM) {
        ED_area_tag_redraw(area);
      }
      break;
  }
}

/* ******************** main region ****************** */

static void custom_main_region_init(wmWindowManager *wm, ARegion *region)
{
  UI_view2d_region_reinit(&region->v2d, V2D_COMMONVIEW_STANDARD, region->winx, region->winy);
}

static void custom_main_region_draw(const bContext *C, ARegion *region)
{
  /* Draw UI elements defined in Python */
  const char *contexts[] = {"CUSTOM_PT_main_panel", nullptr};
  ED_region_panels_ex(C, region, WM_OP_INVOKE_REGION_WIN, contexts);
}

/* ******************** header region ****************** */

static void custom_header_init(wmWindowManager * /*wm*/, ARegion *region)
{
  ED_region_header_init(region);
}

static void custom_header_draw(const bContext *C, ARegion *region)
{
  /* Important: This draws both the header and menus */
  ED_region_header_layout(C, region);
  ED_region_header_draw(C, region);
}


/* ******************** tools region ****************** */

static void custom_tools_init(wmWindowManager *wm, ARegion *region)
{
  ED_region_panels_init(wm, region);
}

static void custom_tools_draw(const bContext *C, ARegion *region)
{

  ED_region_panels(C, region);
}

/* ******************** ui region ****************** */

static void custom_ui_init(wmWindowManager *wm, ARegion *region)
{
  ED_region_panels_init(wm, region);
}

static void custom_ui_draw(const bContext *C, ARegion *region)
{
  ED_region_panels(C, region);
}

/* ******************** registration ****************** */

void ED_spacetype_custom()
{
  std::unique_ptr<SpaceType> st = std::make_unique<SpaceType>();

  st->spaceid = SPACE_CUSTOM;
  STRNCPY(st->name, "Custom");

  st->create = custom_create;
  st->free = custom_free;
  st->init = custom_init;
  st->duplicate = custom_duplicate;
  st->listener = custom_listener;

  /* Main region */
  ARegionType *art = static_cast<ARegionType *>(
      MEM_callocN(sizeof(ARegionType), "custom main region"));
  art->regionid = RGN_TYPE_WINDOW;
  art->init = custom_main_region_init;
  art->draw = custom_main_region_draw;
  art->keymapflag = ED_KEYMAP_UI | ED_KEYMAP_VIEW2D;
  BLI_addhead(&st->regiontypes, art);

  /* Header */
  art = static_cast<ARegionType *>(MEM_callocN(sizeof(ARegionType), "custom header region"));
  art->regionid = RGN_TYPE_HEADER;
  art->prefsizey = HEADERY;
  art->keymapflag = ED_KEYMAP_UI | ED_KEYMAP_VIEW2D | ED_KEYMAP_HEADER;
  art->init = custom_header_init;
  art->draw = custom_header_draw;
  BLI_addhead(&st->regiontypes, art);

  /* Tools */
  art = static_cast<ARegionType *>(MEM_callocN(sizeof(ARegionType), "custom tools region"));
  art->regionid = RGN_TYPE_TOOLS;
  art->prefsizex = UI_COMPACT_PANEL_WIDTH;
  art->keymapflag = ED_KEYMAP_UI | ED_KEYMAP_FRAMES;
  art->init = custom_tools_init;
  art->draw = custom_tools_draw;
  BLI_addhead(&st->regiontypes, art);

  /* UI */
  art = static_cast<ARegionType *>(MEM_callocN(sizeof(ARegionType), "custom ui region"));
  art->regionid = RGN_TYPE_UI;
  art->prefsizex = UI_COMPACT_PANEL_WIDTH;
  art->keymapflag = ED_KEYMAP_UI;
  art->init = custom_ui_init;
  art->draw = custom_ui_draw;
  art->listener = custom_region_listener;
  BLI_addhead(&st->regiontypes, art);

  BKE_spacetype_register(std::move(st));
}
