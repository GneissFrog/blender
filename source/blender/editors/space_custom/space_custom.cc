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

/* ******************** default callbacks for custom space ***************** */
static SpaceLink *custom_create(const ScrArea * /*area*/, const Scene * /*scene*/)
{
  SpaceCustom *scustom = static_cast<SpaceCustom *>(
      MEM_callocN(sizeof(SpaceCustom), "initcustom"));
  scustom->spacetype = SPACE_CUSTOM;

  /* Header */
  ARegion *region = BKE_area_region_new();
  BLI_addtail(&scustom->regionbase, region);
  region->regiontype = RGN_TYPE_HEADER;
  region->alignment = (U.uiflag & USER_HEADER_BOTTOM) ? RGN_ALIGN_BOTTOM : RGN_ALIGN_TOP;

  /* Tools region */
  region = BKE_area_region_new();
  BLI_addtail(&scustom->regionbase, region);
  region->regiontype = RGN_TYPE_TOOLS;
  region->alignment = RGN_ALIGN_LEFT;
  region->flag = RGN_FLAG_HIDDEN;

  /* UI region */
  region = BKE_area_region_new();
  BLI_addtail(&scustom->regionbase, region);
  region->regiontype = RGN_TYPE_UI;
  region->alignment = RGN_ALIGN_RIGHT;
  region->flag = RGN_FLAG_HIDDEN;

  /* Main region */
  region = BKE_area_region_new();
  BLI_addtail(&scustom->regionbase, region);
  region->regiontype = RGN_TYPE_WINDOW;

  return (SpaceLink *)scustom;
}

static void custom_free(SpaceLink * /*sl*/) {}

static SpaceLink *custom_duplicate(SpaceLink *sl)
{
  SpaceCustom *scustom_new = static_cast<SpaceCustom *>(MEM_dupallocN(sl));
  return (SpaceLink *)scustom_new;
}

static void custom_init(wmWindowManager * /*wm*/, ScrArea * /*area*/) {}

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

/* ************************ keymap definitions ************************* */
static void custom_keymap(wmKeyConfig *keyconf)
{
  printf("custom_keymap: Starting keymap registration\n");

  /* Custom Generic Keymap */
  wmKeyMap *generic_keymap = WM_keymap_ensure(
      keyconf, "Custom Generic", SPACE_CUSTOM, RGN_TYPE_WINDOW);
  printf("custom_keymap: Created generic keymap: %p\n", (void *)generic_keymap);
  generic_keymap->poll = custom_poll;

  /* Tools region */
  {
    KeyMapItem_Params params{};
    params.type = EVT_TKEY;
    params.value = 1;  // Press
    params.modifier = 0;
    params.keymodifier = 0;
    params.direction = 0;

    WM_keymap_add_panel(generic_keymap, "CUSTOM_PT_tools", &params);
  }

  /* Properties region */
  {
    KeyMapItem_Params params{};
    params.type = EVT_NKEY;
    params.value = 1;  // Press
    params.modifier = 0;
    params.keymodifier = 0;
    params.direction = 0;

    WM_keymap_add_panel(generic_keymap, "CUSTOM_PT_sidebar", &params);
  }

  /* Custom Keymap */
  wmKeyMap *custom_keymap = WM_keymap_ensure(keyconf, "Custom", SPACE_CUSTOM, RGN_TYPE_WINDOW);
  printf("custom_keymap: Created custom keymap: %p\n", (void *)custom_keymap);
  custom_keymap->poll = custom_poll;

  /* Add item */
  {
    KeyMapItem_Params params{};
    params.type = EVT_CKEY;
    params.value = 1;  // Press
    params.modifier = 0;
    params.keymodifier = 0;
    params.direction = 0;

    WM_keymap_add_menu(custom_keymap, "CUSTOM_MT_editor_menus", &params);
  }

  printf("custom_keymap: Finished keymap registration\n");
}


/* ******************** main region ****************** */
static void custom_main_region_init(wmWindowManager *wm, ARegion *region)
{
  UI_view2d_region_reinit(&region->v2d, V2D_COMMONVIEW_STANDARD, region->winx, region->winy);

  wmKeyMap *keymap = WM_keymap_ensure(
      wm->defaultconf, "Custom Generic", SPACE_CUSTOM, RGN_TYPE_WINDOW);
  WM_event_add_keymap_handler_v2d_mask(&region->runtime->handlers, keymap);

  keymap = WM_keymap_ensure(wm->defaultconf, "Custom", SPACE_CUSTOM, RGN_TYPE_WINDOW);
  WM_event_add_keymap_handler_v2d_mask(&region->runtime->handlers, keymap);
}


/* ******************** registration ****************** */

static void custom_region_listener(const wmRegionListenerParams *params)
{
  ARegion *region = params->region;
  const wmNotifier *wmn = params->notifier;

  switch (wmn->category) {
    case NC_SCREEN:
      printf("Region listener - screen changed. Region type: %d, hidden: %d\n",
             region->regiontype,
             (region->flag & RGN_FLAG_HIDDEN) != 0);
      break;
  }
}


static bool custom_context_poll(const bContext *C)
{
  return CTX_wm_space_custom(C) != nullptr;
}

static bool custom_poll(bContext *C)
{
  // Print raw space data first
  SpaceLink *sl = CTX_wm_space_data(C);
  /*printf("Poll - Raw space type: %d (CUSTOM=%d)\n", sl ? sl->spacetype : -1, SPACE_CUSTOM);*/

  // Try to get our custom space specifically
  SpaceCustom *scustom = CTX_wm_space_custom(C);
  /*printf("Poll - Custom space: %p\n", (void *)custom);*/

  return scustom != nullptr;
}

static int /*eContextResult*/ custom_context(const bContext *C,
                                             const char *member,
                                             bContextDataResult *result)
{
  SpaceLink *sl = CTX_wm_space_data(C);
  printf("Custom context check - member: %s, space type: %d\n", member, sl ? sl->spacetype : -1);

  if (CTX_data_dir(member)) {
    CTX_data_dir_set(result, nullptr);
    return CTX_RESULT_OK;
  }
  return CTX_RESULT_MEMBER_NOT_FOUND;
}


/* ******************** main region ****************** */


//static void custom_main_region_init(wmWindowManager *wm, ARegion *region)
//{
//  UI_view2d_region_reinit(&region->v2d, V2D_COMMONVIEW_STANDARD, region->winx, region->winy);
//
//  /* Add keymap handlers */
//  wmKeyMap *keymap = WM_keymap_ensure(
//      wm->defaultconf, "Custom Generic", SPACE_CUSTOM, RGN_TYPE_WINDOW);
//  WM_event_add_keymap_handler_v2d_mask(&region->runtime->handlers, keymap);
//
//  keymap = WM_keymap_ensure(wm->defaultconf, "Custom", SPACE_CUSTOM, RGN_TYPE_WINDOW);
//  WM_event_add_keymap_handler_v2d_mask(&region->runtime->handlers, keymap);
//
//  ED_region_panels_init(wm, region);
//}

static void custom_main_region_draw(const bContext *C, ARegion *region)
{
  SpaceLink *sl = CTX_wm_space_data(C);
 /*printf("Main region draw - space type: %d\n", sl ? sl->spacetype : -1);*/

  SpaceCustom *scustom = CTX_wm_space_custom(C);
  /*printf("Main region draw - custom space: %p\n", (void *)custom);*/

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
  wmKeyMap *keymap = WM_keymap_ensure(wm->defaultconf, "Custom", SPACE_CUSTOM, 0);
  WM_event_add_keymap_handler(&region->runtime->handlers, keymap);
  printf("Added keymap handler to tools region\n");

  ED_region_panels_init(wm, region);
}


static void custom_tools_draw(const bContext *C, ARegion *region)
{

  ED_region_panels(C, region);
}

/* ******************** ui region ****************** */

static void custom_ui_init(wmWindowManager *wm, ARegion *region)
{
  wmKeyMap *keymap = WM_keymap_ensure(wm->defaultconf, "Custom", SPACE_CUSTOM, 0);
  WM_event_add_keymap_handler(&region->runtime->handlers, keymap);
  printf("Added keymap handler to UI region\n");

  ED_region_panels_init(wm, region);
}

static void custom_ui_draw(const bContext *C, ARegion *region)
{
  ED_region_panels(C, region);
}

/* ******************** registration ****************** */
void rna_Space_region_update(struct Main *bmain, struct Scene *scene, struct PointerRNA *ptr)
{
  // Your implementation here
  // For example, you might want to redraw the region or update some data
  // based on the property change.

  // Example:
  if (bmain && scene && ptr) {
    // Trigger a redraw or update logic here
    WM_main_add_notifier(NC_SPACE | ND_SPACE_CUSTOM, NULL);
  }
}
void ED_spacetype_custom()
{
  printf("\n### CUSTOM EDITOR: Starting space type registration ###\n");
  std::unique_ptr<SpaceType> st = std::make_unique<SpaceType>();

  st->spaceid = SPACE_CUSTOM;
  STRNCPY(st->name, "Custom");

  st->create = custom_create;
  st->free = custom_free;
  st->init = custom_init;
  st->duplicate = custom_duplicate;
  st->operatortypes = nullptr;
  st->context = custom_context;
  st->dropboxes = nullptr;
  st->gizmos = nullptr;
  st->listener = custom_listener;


  printf("Registering Custom space type with ID: %d\n", st->spaceid);

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
  art->keymapflag = ED_KEYMAP_UI;
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
  
  //// Register RNA
  //RNA_struct_blender_type_set(&RNA_SpaceCustom, st.get());
  st->keymap = custom_keymap;
  BKE_spacetype_register(std::move(st));
}
