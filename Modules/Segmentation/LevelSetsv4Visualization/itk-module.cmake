set(DOCUMENTATION "This module constains classes related to the
visualization of the level-sets.")

itk_module( ITKLevelSetsv4Visualization
  DEPENDS
    ITKLevelSetsv4
    ITKVTK
    ITKVtkGlue # Not necessary; `find_package(VTK)` needs done however
  TEST_DEPENDS
    ITKVTK
    ITKTestKernel
  EXCLUDE_FROM_DEFAULT
  DESCRIPTION
   "${DOCUMENTATION}"
  )
