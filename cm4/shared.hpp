// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM4_SHARED_HPP
#define LIMNMOCO_CM4_SHARED_HPP

#include <array>
#include <cstdint>

#include "config.hpp"
#include "common.hpp"
#include "status.hpp"

// #NOTE: Keep in sync with cm7/shared.hpp

class Shared {
public:
  using Positions     = std::array<int32_t, LIMNMOCO_MOTOR_COUNT>;
  using Velocities    = std::array<uint32_t, LIMNMOCO_MOTOR_COUNT>;
  using Accelerations = std::array<uint32_t, LIMNMOCO_MOTOR_COUNT>;
  using Statuses      = std::array<Status, LIMNMOCO_MOTOR_COUNT>;

  Shared();

  static void begin();
  static Shared *ptr();

  Directions get_directions();
  Status get_direction(uint8_t motor, bool &d) volatile;
  Status set_direction(uint8_t motor, bool d) volatile;

  Status get_motor_target(uint8_t motor, int32_t &target) volatile;
  Status set_motor_target(uint8_t motor, int32_t target) volatile;

  Status get_motor_position(uint8_t motor, int32_t &position) volatile;
  Status set_motor_position(uint8_t motor, int32_t position) volatile;

  Status get_motor_velocity(uint8_t motor, uint32_t &velocity) volatile;
  Status set_motor_velocity(uint8_t motor, uint32_t velocity) volatile;

  Status get_motor_acceleration(uint8_t motor, uint32_t &acceleration) volatile;
  Status set_motor_acceleration(uint8_t motor, uint32_t acceleration) volatile;

  Status get_motor_status(uint8_t motor, Status &status) volatile;
  Status set_motor_status(uint8_t motor, Status status) volatile;

  Status is_motor_moving(uint8_t motor, bool &moving) volatile;

private:
  static Shared *m_ptr;

  Directions    m_directions;
  Positions     m_motor_targets;
  Positions     m_motor_positions;
  Velocities    m_motor_velocities;
  Accelerations m_motor_accelerations;
  Statuses      m_motor_statuses;
};

#endif // !LIMNMOCO_CM4_SHARED_HPP


