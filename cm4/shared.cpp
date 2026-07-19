// SPDX-License-Identifier: GPL-3.0-or-later

#include <cstring>

#include "shared.hpp"

Shared *Shared::m_ptr;

Shared::Shared() 
    : m_directions{0} {
      m_motor_targets.fill(0);
      m_motor_positions.fill(0);
      m_motor_velocities.fill(0);
      m_motor_accelerations.fill(0);
}

// static
void Shared::begin() {
    m_ptr = new ((Shared *)0x3800FD00) Shared();
}

// static
Shared *Shared::ptr() {
    return m_ptr;
}

Directions Shared::get_directions() {
    return m_directions;
}

Status Shared::get_direction(uint8_t motor, bool &d) volatile {
    if (motor >= m_directions.size()) {
        return STATUS_ERR_RANGE;
    }

    d = m_directions[motor];
    return STATUS_OK;
}

Status Shared::set_direction(uint8_t motor, bool d) volatile {
    if (motor >= m_directions.size()) {
        return STATUS_ERR_RANGE;
    }

    if (m_motor_velocities[motor] > 0) {
        return STATUS_ERR_MOVING;
    }

    m_directions[motor] = d;
    return STATUS_OK;
}

Status Shared::get_motor_target(uint8_t motor, int32_t &target) volatile {
    if (motor >= m_motor_targets.size()) {
        return STATUS_ERR_RANGE;
    }

    target = m_motor_targets[motor];
    return STATUS_OK;
}

Status Shared::set_motor_target(uint8_t motor, int32_t target) volatile {
    if (motor >= m_motor_targets.size()) {
        return STATUS_ERR_RANGE;
    }

    m_motor_targets[motor] = target;
    return STATUS_OK;
}

Status Shared::get_motor_position(uint8_t motor, int32_t &position) volatile {
    if (motor >= m_motor_positions.size()) {
        return STATUS_ERR_RANGE;
    }

    position = m_motor_positions[motor];
    return STATUS_OK;
}

Status Shared::set_motor_position(uint8_t motor, int32_t position) volatile {
    if (motor >= m_motor_positions.size()) {
        return STATUS_ERR_RANGE;
    } 

    m_motor_positions[motor] = position;
    return STATUS_OK;
}

Status Shared::get_motor_velocity(uint8_t motor, uint32_t &velocity) volatile {
    if (motor >= m_motor_velocities.size()) {
        return STATUS_ERR_RANGE;
    }

    velocity = m_motor_velocities[motor];
    return STATUS_OK;
}

Status Shared::set_motor_velocity(uint8_t motor, uint32_t velocity) volatile {
    if (motor >= m_motor_velocities.size()) {
        return STATUS_ERR_RANGE;
    }

    m_motor_velocities[motor] = velocity;
    return STATUS_OK;
}

Status Shared::get_motor_acceleration(uint8_t motor, uint32_t &acceleration) volatile {
    if (motor >= m_motor_accelerations.size()) {
        return STATUS_ERR_RANGE;
    }

    acceleration = m_motor_accelerations[motor];
    return STATUS_OK;
}

Status Shared::set_motor_acceleration(uint8_t motor, uint32_t acceleration) volatile {
    if (motor >= m_motor_accelerations.size()) {
        return STATUS_ERR_RANGE;
    }

    m_motor_accelerations[motor] = acceleration;
    return STATUS_OK;
}

Status Shared::is_motor_moving(uint8_t motor, bool &moving) volatile {
    if (motor >= m_motor_velocities.size()) {
        return STATUS_ERR_RANGE;
    }

    moving = (m_motor_velocities[motor] == 0);
    return STATUS_OK;
}


