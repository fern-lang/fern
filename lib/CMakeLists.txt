add_subdirectory(Parse)

newFernTarget(
  fern

  BINARY
  fern.cpp

  LINK_LIBS
  FernParse
)
