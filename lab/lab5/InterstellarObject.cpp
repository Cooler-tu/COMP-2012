#include "InterstellarObject.h"

Vector2 InterstellarObject::get_position() const {
    return m_position;
}

void InterstellarObject::simulate(double years) {
    m_position = Vector2::add(m_position, Vector2::scale(m_velocity, years));
    CelestialObject::simulate(years);
}