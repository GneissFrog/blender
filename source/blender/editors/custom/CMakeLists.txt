set(INC
  ../../blenkernel
  ../../blenlib
  ../../gpu
  ../../makesdna
  ../../makesrna
  ../../windowmanager
  ../../../../intern/guardedalloc
)

set(SRC
  space_custom.cc
)

set(INC_SYS
)

set(LIB
)

blender_add_lib(bf_editor_custom "${SRC}" "${INC}" "${INC_SYS}" "${LIB}")
