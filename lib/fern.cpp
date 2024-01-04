#include <Roots/Filesystem.hpp>
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <iostream>
#include "Errors/FancyPrinter.hpp"
#include "FernConfig.hpp"
#include "Parse/Lex/Lexer.hpp"
#include "Errors/Context.hpp"
#include "Parse/Parser.hpp"

auto main(int argc, char **argv) -> int {
  cxxopts::Options opts("fern", fmt::format("Fern Compiler v{}", FernVersion));
  opts.allow_unrecognised_options();
  opts.add_options()("v,version", "Print version information")(
      "h,help", "Print this help text")("ifile", "File to compile",
                                        cxxopts::value<std::string>());

  opts.parse_positional({"ifile"});
  auto optRes = opts.parse(argc, argv);

  if (optRes.count("help") || optRes.arguments().empty()) {
    std::cout << opts.help() << std::endl;
    return 0;
  } else if (optRes.count("version")) {
    std::cout << fmt::format("Fern v{} ({} @ {})", FernVersion, FernBuildDate, FernGitRev)
              << std::endl;

    return 0;
  }

  auto ifile = optRes["ifile"].as<std::string>();

  auto res = roots::fs::readFile(ifile);
  if (!res) {
    std::cerr << fmt::format("Failed to read file: {}", res.error().toString())
              << std::endl;
    return 1;
  }

  if (res.value().empty()) {
    std::cerr << "File is empty" << std::endl;
    return 1;
  }

  auto source = res.value();
  fern::Context ctx(source, ifile);
  fern::Lexer lexer(ctx);
  fern::FancyErrorPrinter errPrinter(source, ifile);

  auto lexRes = lexer.lex();
  if (!lexRes) {
    ctx.printErrors(errPrinter);

    return 1;
  }
  ctx.flushWarnings(errPrinter);

  fern::Parser parser(lexer.getTokens(), ctx);
  auto parseRes = parser.parse();

  if (!parseRes) {
    ctx.printErrors(errPrinter);
    return 1;
  }
  ctx.flushWarnings(errPrinter);

  std::cout << "Successfully parsed file" << std::endl;

  return 0;
}
