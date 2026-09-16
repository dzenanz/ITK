itk_module_test()

set(StructuralSimilarityGTests itkStructuralSimilarityImageFilterGTest.cxx)

creategoogletestdriver(StructuralSimilarity "${StructuralSimilarity-Test_LIBRARIES}" "${StructuralSimilarityGTests}")

ExternalData_Expand_Arguments(ITKData _CTHEAD1_PNG_INPUT "DATA{${ITK_DATA_ROOT}/Input/cthead1.png}")
ExternalData_Expand_Arguments(ITKData _CTHEAD1_JPG_INPUT "DATA{${ITK_DATA_ROOT}/Input/cthead1.jpg}")
ExternalData_Expand_Arguments(ITKData _CTHEAD1_PNG_CC "DATA{${ITK_DATA_ROOT}/Baseline/BasicFilters/ConnectedComponentImageFilterTest.png}")
target_compile_definitions(
  StructuralSimilarityGTestDriver
  PRIVATE
    "CTHEAD1_PNG_INPUT=${_CTHEAD1_PNG_INPUT}"
    "CTHEAD1_JPG_INPUT=${_CTHEAD1_JPG_INPUT}"
    "CTHEAD1_PNG_CC=${_CTHEAD1_PNG_CC}"
)
