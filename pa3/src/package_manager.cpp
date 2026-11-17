#include "package_manager.h"

#include <algorithm>
#include <functional>
#include <iterator>
#include <optional>
#include <set>
#include <string>
#include <utility>

bool PackageManager::conflicts(const Package& package, const LocalRepository& repository) {
  // TODO
  return false;
}

std::optional<std::vector<Package>> PackageManager::collect_missing(const Package& package,
                                                                    const LocalRepository& repository) const {
  // TODO
  return std::nullopt;
}

Status PackageManager::install(const std::string& name, bool explicit_install) {
  // TODO
  return Status::SUCCESS;
}

void PackageManager::remove_with_unused_dependencies(const std::string& name) {
  // TODO
}

Status PackageManager::uninstall(const std::string& name) {
  // TODO
  return Status::SUCCESS;
}

Status PackageManager::upgrade() {
  // TODO
  return Status::SUCCESS;
}

std::size_t PackageManager::prune() {
  // TODO
  return 0;
}

std::vector<std::string> PackageManager::orphans() const {
  // TODO
  return {};
}
