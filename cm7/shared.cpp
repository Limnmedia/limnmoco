// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>

#include "shared.hpp"

Shared::Shared()
  : m_directions{0} {
  m_motor_targets.fill(0);
  m_motor_positions.fill(0);
  m_motor_velocities.fill(0);
  m_motor_accelerations.fill(0);
}

// static
void Shared::begin() {
  m_ptr = new ((Shared *)LIMNMOCO_SHARED_DATA_ADDRESS) Shared();
}

// static
Direction Shared::get_direction() {
  return m_ptr->m_direction;
}
