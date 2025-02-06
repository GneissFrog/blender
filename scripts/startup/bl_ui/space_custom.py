# SPDX-FileCopyrightText: 2024 Your Name
# SPDX-License-Identifier: GPL-2.0-or-later

import bpy
from bpy.types import Header, Menu, Panel

class CUSTOM_HT_header(Header):
    bl_space_type = 'CUSTOM'

    def draw(self, context):
        layout = self.layout

        # Add the standard header elements (editor type selector, etc)
        row = layout.row(align=True)
        row.template_header()

        # Add some spacing
        layout.separator_spacer()

        # Editor label
        layout.label(text="Custom Editor")

        # Flexible space
        layout.separator_spacer()

        # Add standard header buttons (like maximize, etc)
        if context.region.alignment != 'RIGHT':
            # Only add these buttons if we're not in the right-aligned region
            layout.popover(
                panel="SCREEN_PT_header_tools",
                text="",
                icon='DOWNARROW_HLT',
            )

class CUSTOM_MT_editor_menus(Menu):
    bl_idname = "CUSTOM_MT_editor_menus"
    bl_label = ""

    def draw(self, context):
        layout = self.layout
        layout.menu("CUSTOM_MT_view")
        layout.menu("CUSTOM_MT_select")

class CUSTOM_MT_view(Menu):
    bl_label = "View"

    def draw(self, context):
        layout = self.layout
        layout.operator("screen.area_dupli", text="New Window")
        layout.separator()
        layout.operator("screen.header_toggle_menus", text="Show Menus", icon='HIDE_OFF')

class CUSTOM_MT_select(Menu):
    bl_label = "Select"

    def draw(self, context):
        layout = self.layout
        layout.label(text="Select Menu")

# This is the main panel that will appear in the UI
class CUSTOM_PT_main_panel(Panel):
    bl_space_type = 'CUSTOM'
    bl_region_type = 'WINDOW'
    bl_label = "Custom Panel"

    def draw(self, context):
        layout = self.layout
        layout.label(text="Custom Editor Panel")
        layout.operator("screen.space_context_cycle")

classes = (
    CUSTOM_HT_header,
    CUSTOM_MT_editor_menus,
    CUSTOM_MT_view,
    CUSTOM_MT_select,
    CUSTOM_PT_main_panel,
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)

def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()
