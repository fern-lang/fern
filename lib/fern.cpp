#include <iostream>
#include "FernConfig.hpp"

auto main(int argc, char** argv) -> int {
  std::cout << "Hello, Fern!" << std::endl;

  std::cout << "Fern version: " << FernVersion << std::endl;
  std::cout << "Fern build date: " << FernBuildDate << std::endl;
  std::cout << "Fern git revision: " << FernGitRev << std::endl;

  return 0;
}
