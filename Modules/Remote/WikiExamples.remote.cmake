#
# ITK WikiExamples
#

# If the environment var WikiExamplesTag exists, use it
if (NOT DEFINED ENV{WikiExamplesTag})
  set(GIT_TAG 8909cf03e7b115d46735701d6c6a6edc6b2c59e2)
else()
  set(GIT_TAG $ENV{WikiExamplesTag})
endif()

itk_fetch_module(WikiExamples
  "A collection of examples that illustrate how to use ITK."
  GIT_REPOSITORY ${git_protocol}://github.com/dzenanz/ITKWikiExamples.git
  GIT_TAG ${GIT_TAG}
  )
