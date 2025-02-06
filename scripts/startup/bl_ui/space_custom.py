# SPDX-FileCopyrightText: 2024 Your Name
# SPDX-License-Identifier: GPL-2.0-or-later

import bpy
from bpy.types import Header, Menu, Panel

class CUSTOM_HT_header(Header):
    bl_space_type = 'CUSTOM'

    def draw(self, context):
        layout = self.layout
        st = context.space_data
        layout.template_header()

        # Draw the editor menus
        CUSTOM_MT_editor_menus.draw_collapsible(context, layout)

        layout.separator_spacer()
        layout.label(text="Custom Editor")
        layout.separator_spacer()

class CUSTOM_MT_editor_menus(Menu):
    bl_idname = "CUSTOM_MT_editor_menus"
    bl_label = ""

    @classmethod
    def draw_collapsible(cls, context, layout):
        layout.menu(cls.bl_idname)

    def draw(self, context):
        layout = self.layout
        layout.menu("CUSTOM_MT_view")
        layout.menu("CUSTOM_MT_select")

class CUSTOM_MT_view(Menu):
    bl_label = "View"

    def draw(self, context):
        layout = self.layout
        layout.prop(context.space_data, "show_region_ui", text="Sidebar")
        layout.prop(context.space_data, "show_region_tools", text="Tools")
        layout.separator()
        layout.operator("screen.area_dupli", text="New Window")
        layout.separator()
        layout.operator("screen.header_toggle_menus", text="Show Menus", icon='HIDE_OFF')

class CUSTOM_MT_select(Menu):
    bl_label = "Select"

    def draw(self, context):
        layout = self.layout
        layout.label(text="Select Menu")

# Main window panels
class CUSTOM_PT_main(Panel):
    bl_space_type = 'CUSTOM'
    bl_region_type = 'WINDOW'
    bl_label = "Main Panel"
    bl_category = "Main"

    def draw(self, context):
        layout = self.layout
        layout.label(text="Custom Editor Panel")
        layout.operator("screen.space_context_cycle")

# Tools region panels
class CUSTOM_PT_tools(Panel):
    bl_space_type = 'CUSTOM'
    bl_region_type = 'TOOLS'
    bl_label = "Tools"
    bl_category = "Tools"

    def draw(self, context):
        layout = self.layout
        layout.label(text="Tools Panel")

# UI/Sidebar region panels
class CUSTOM_PT_sidebar(Panel):
    bl_space_type = 'CUSTOM'
    bl_region_type = 'UI'
    bl_label = "Sidebar"
    bl_category = "Sidebar"

    def draw(self, context):
        layout = self.layout
        layout.label(text="Sidebar Panel")

classes = (
    CUSTOM_HT_header,
    CUSTOM_MT_editor_menus,
    CUSTOM_MT_view,
    CUSTOM_MT_select,
    CUSTOM_PT_main,
    CUSTOM_PT_tools,
    CUSTOM_PT_sidebar,
)

def register():
    for cls in classes:
        bpy.utils.register_class(cls)

def unregister():
    for cls in classes:
        bpy.utils.unregister_class(cls)

if __name__ == "__main__":
    register()
