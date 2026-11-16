#include "Moon.h"

Moon::Moon(const string &name, double age, const Planet &orbitee, double orbital_radius, double angular_speed):
    Orbiter(name, age, orbitee, orbital_radius, angular_speed) {

}

const Star &Moon::get_star() const {
    return static_cast<const Planet &>(get_orbitee()).get_star();
}
