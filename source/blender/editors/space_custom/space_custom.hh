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
struct SpaceLink;
//struct StructRNA RNA_SpaceCustom;

/* Functions */

void ED_spacetype_custom(void);
bool custom_poll(bContext *C);
void rna_def_space_custom(BlenderRNA *brna);

#define CTX_wm_space_custom(C) ((SpaceCustom *)CTX_wm_space_data(C))
