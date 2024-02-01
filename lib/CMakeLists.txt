add_subdirectory(Core)

newFernTarget(
  fern

  BINARY
  fern.cpp

  LINK_LIBS
  FernCore
)
