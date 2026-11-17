#include "local_repository.h"

#include <algorithm>
#include <utility>

std::optional<const Package*> LocalRepository::search(const std::string& name) const {
  // TODO
  return std::nullopt;
}

std::vector<const Package*> LocalRepository::list() const {
  // TODO
  return {};
}

void LocalRepository::put(const Package& package, bool explicit_install) {
  // TODO
}

void LocalRepository::drop(const std::string& name) {
  // TODO
}

void LocalRepository::unlink(const std::string& dependency, const std::string& dependent) {
  // TODO
}

unsigned long long LocalRepository::fnv1a(const std::string& text) {
  constexpr unsigned long long FNV_OFFSET_BASIS{14695981039346656037ULL};
  constexpr unsigned long long FNV_PRIME{1099511628211ULL};

  // TODO
  return 0;
}
