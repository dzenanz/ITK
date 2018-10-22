set(DOCUMENTATION "This is a collection of classes that are intended to be
removed from the toolkit.")

itk_module(ITKDeprecated
  PRIVATE_DEPENDS
    ITKCommon
  COMPILE_DEPENDS
    # ITKMesh
  TEST_DEPENDS
    ITKTestKernel
  # EXCLUDE_FROM_DEFAULT # If ITKV4_COMPATIBILITY is set to On, we most likely want this module.
  DESCRIPTION
    "${DOCUMENTATION}"
)
