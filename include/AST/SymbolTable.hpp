#ifndef Fern_Ast_SymbolTable_hpp
#define Fern_Ast_SymbolTable_hpp

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace fern {

template<typename T>
class SymbolTable {
public:
  SymbolTable() = default;
  ~SymbolTable() = default;

  auto incScope() -> void { _scopedMaps.emplace_back(); }

  auto decScope() -> void { _scopedMaps.pop_back(); }

  auto insert(const std::string &name, std::shared_ptr<T> value) -> void {
    _scopedMaps.back()[name] = value;
  }

  auto lookup(const std::string &name) -> std::shared_ptr<T> {
    for (auto it = _scopedMaps.rbegin(); it != _scopedMaps.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second;
      }
    }
    return nullptr;
  }

  auto localLookup(const std::string &name) -> std::shared_ptr<T> {
    auto found = _scopedMaps.back().find(name);
    if (found != _scopedMaps.back().end()) {
      return found->second;
    }
    return nullptr;
  }

private:
  using typemap_t = std::unordered_map<std::string, std::shared_ptr<T>>;
  std::vector<typemap_t> _scopedMaps;
};

template<typename T>
class RawPtrSymbolTable {
public:
  RawPtrSymbolTable() = default;
  ~RawPtrSymbolTable() = default;

  auto incScope() -> void { _scopedMaps.emplace_back(); }

  auto decScope() -> void { _scopedMaps.pop_back(); }

  auto insert(const std::string &name, T *value) -> void {
    _scopedMaps.back()[name] = value;
  }

  auto lookup(const std::string &name) -> T * {
    for (auto it = _scopedMaps.rbegin(); it != _scopedMaps.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second;
      }
    }
    return nullptr;
  }

  auto localLookup(const std::string &name) -> std::shared_ptr<T> {
    auto found = _scopedMaps.back().find(name);
    if (found != _scopedMaps.back().end()) {
      return found->second;
    }
    return nullptr;
  }

private:
  using typemap_t = std::unordered_map<std::string, T *>;
  std::vector<typemap_t> _scopedMaps;
};
} // namespace fern

#endif
