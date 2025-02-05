set(INC
  # Current paths were relative, but need full paths from source root
  # ../include -> ../../editors/include
  ../../editors/include
  ../../blenkernel
  ../../blenlib
  ../../makesdna
  ../../makesrna
  ../../windowmanager
  ../../gpu
  ../../imbuf
  ../../nodes
  # Ensure guardedalloc path is correct
  ../../../../intern/guardedalloc
)

set(SRC
  space_custom.cc
  space_custom.hh
)

set(LIB
)

blender_add_lib(bf_editor_custom "${SRC}" "${INC}" "${INC_SYS}" "${LIB}")
