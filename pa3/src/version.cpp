#include "version.h"

bool PkgVersion::operator==(const PkgVersion& other) const {
  // TODO
  return false;
}

bool PkgVersion::operator<(const PkgVersion& other) const {
  // TODO
  return false;
}

PkgVersion::operator std::string() const {
  // TODO
  return {};
}

std::ostream& operator<<(std::ostream& os, const PkgVersion& version) {
  // TODO
  return os;
}
