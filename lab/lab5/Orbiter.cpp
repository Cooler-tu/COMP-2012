#include "Orbiter.h"

using std::cos;
using std::sin;

Orbiter::Orbiter(const string &name, double age, const CelestialObject &orbitee, double orbital_radius, double angular_speed)
    :CelestialObject{name, age}, m_orbitee(orbitee), m_orbital_radius(orbital_radius), m_angular_position(0), m_angular_speed(angular_speed)
{}

Vector2 Orbiter::get_position() const {
    Vector2 position(Vector2::add(m_orbitee.get_position(), 
    Vector2::scale(Vector2(cos(m_angular_position), sin(m_angular_position)), m_orbital_radius)));
    return position;
}

void Orbiter::simulate(double years) {
    m_angular_position += years * m_angular_speed;
    CelestialObject::simulate(years);
}

double Orbiter::get_distance_from_star() const {
    // Description - Computes the distance from the solar system's Star to the Orbiter.
    // distance of 2 vectors = magnitude of (vector a - vector b)
    return Vector2::subtract(get_position(), get_star().get_position()).get_magnitude();
}