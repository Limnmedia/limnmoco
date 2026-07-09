// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LIMNMOCO_CM7_MOTORS_HPP
#define LIMNMOCO_CM7_MOTORS_HPP

#include <cstdint>

namespace motors {

// #NOTE: a motor, when moving from it's current position to a new position,
//    in order to move safely, must accelerate to a target velocity, 
//    and then decelerate to zero velocity.
//    
//    in my mind I see a bell curve, placed from point A to point B.
//    this is obviously in the integer domain rather than floating point
//    though, how do we smoothly add velocity? with acceleration of course.
//    we can compute how much acceleration is needed to reach the maximum 
//    velocity over a given period of time, or a given period of steps.
//    
//    ideally there is an acceleration phase, a steady phase, then a 
//    deceleration phase. if the distance is such that we cannot reach 
//    the maximum velocity given the maximum acceleration, then we should 
//    not ever enter the steady phase. ideally this falls out of the 
//    implementation.
//
// 100%v |                           --------------
//       |                         --              --
//       |                       --                  --
//       |                     --                      --
//       |                   --                          --
//       |                 --                              --
//       |               --                                  --
// 50%v  |             --                                      --
//       |           --                                          --
//       |         --                                              --
//       |       --                                                  --
//       |     --                                                      --
//       |   --                                                          --
//       | --                                                              --
//   0%v |-                                                                  -
//       |_____________________________________________________________________
//       0s                         Xs              Ys                       Zs
//
//    so we can acheive this linear increase in velocity by linearly accelerating till
//    we reach the maximum velocity, then holding at zero acceleration, then linearly
//    decellerating.
//
//    we could also do non-linear acceleration. through the use of polynomial interpolation,
//    bilinear interpolation, spline interpolation, and more.
//
//    The smoothstep function also seems like the right tool for this purpose, it would reduce 
//    the acceleration at the points in the graph when we change from zero to acceleration and 
//    acceleration to zero. (I hope that makes sense...)
//
//    okay, so if we have N steps to take. and we have a maximum velocity of V, and a maximum acceleration A.
//    we add acceleration at some frequency, for instance, how many steps do we have the motor take before 
//    adding in acceleration? or, how long do we wait before we add acceleration to the motors velocity?
//
//    to reach the target position, we have to take a certain number of steps, how fast we take those steps 
//    is important, for the case of virtual steps. where we want the motors to reach their target positions 
//    at the same time, yet the distances traveled may be the same or very different, and the maximum velocity 
//    of each motor may be different. it may not be possible in all cases with all possible 
//    velocity/acceleration/position permutations.
//
//    if we have N steps to take, and we are travelling at a constant velocity V (in steps per second) then 
//    we will reach the target position at N/V seconds. if we add acceleration at each time step then the formula 
//    is not so simple. The acceleration is added at some Hz, and the steps are generated at 200kHz 
//    if we add acceleration at 100kHz then each motor is capable of stepping twice before it's velocity changes.
//    if we add acceleration at 50kHz then each motor is capable of stepping four times before it's velocity changes
//    does this matter? 
//    well, yes. while the target speeds are technically always reached, and the target location is always reached.
//    the smoothness of the curve is dependent entirely on the rate at which acceleration is added.
//    the more we update acceleration the smaller an acceleration value has to be to mean a fast or slow acceleration.
//    at ten updates a second, 10 acceleration only adds 100 velocity. at 100 updates a second 10 acceleration adds 1000 velocity
//    this looks like it has the potential to overwhelm the motor, and cause us to lose position. which we absolutely 
//    never want to happen. additionally, if we update the velocity constantly we will be interrupting the rest of the work 
//    that needs doing within the m7 processor, such as processing messages and running the virtual position solver,
//    (inverse kinematics)
//    this is a choice that needs making, and we have to live with the fact that it's results are baking in an implicit rule 
//    into the codebase built atop it. we could go with something standard like 60Hz. That is fast enough that humans are barely 
//    able to tell, and it leaves 16ms for the m7 to go and do other work. which is a long time.
//    I don't know for sure as I have yet to measure the solver in it's current form, but the previous IK solver took around 1ms
//    and the dragonframe packet response code takes < 3ms to write out 32 bytes of data out the serial line.
//
//    okay, so if we update velocity at 60Hz. then what? well, we can now know that the amount of time it takes to reach a target 
//    number of steps. it's the sum of the number of time slices it takes to reach the target position. we take n = V/60 steps in 
//    a time slice, then in the next time step we are moving at an updated velocity which means we move an updated number of steps.
//    well how many time slices does it take to reach a target number of steps? it's N/n except that n isn't a fixed value, so that 
//    doesn't work.
//
//      velocity += acceleration
//
//    okay, the TI document I found describes the creation of an acceleration profile and a decelleration profile, and then allocating 
//    steps to each segment. so then we have N/A + N/V + N/D = N where A is the acceleration steps V is the steps at the target velocity 
//    and D is the steps of the deceleration profile.
//
//    if we have 100 steps to take, and we allocate 25 of them to acceleration, 25 of them to decelleration, and 50 of them at the target 
//    velocity, then we have to accelerate in 25 steps, doesn't that open up the possibility of accelerating to fast? wherein we add too 
//    much velocity to the given motor at single time step. for instance if we have a target velocity equal to the maximum velocity, 200kHz
//    and we need to take 100 steps, then we are now tasked with accelerating from 0 to 200KHz in 25 steps. however, the time it takes to 
//    step 25 times is dependent on the rate of stepping. so if we start at 0, we step 0 times, then we update velocity, how much do we add?
//    if we naively add 200kHz/25 = 8,000 acceleration. then, how many steps will we take in that first time step? well that would be 
//    that is how many times would 8,000 roll over 200,000. 8,000 takes 25 iterations before it generates a step. so how many 25 iterations 
//    fit into 16ms? at 200kHz, isn't it 200'000 / 8'000 / (1 / 60) = 1'500. which is way more than 25. so in order to generate exactly 25 steps
//    before reaching the target velocity what do we do?
//  
//    we need to have step one accur at a given velocity, then somehow update the velocity multiple times before the next step is generated
//    effectively 'skipping' multiple points in the acceleration curve.
//
//    if we have a generic acceleration curve, and we know how long it takes and how many steps it takes
//    call the number of steps A, plus the deceleration curve and it's time + steps call the number of steps D.
//    (we want to parametrize this curve based on acceleration).
//    then we have three cases, where T is the target number of steps
//    1. A + D < T
//    2. A + D = T
//    3. A + D > T
//
//    1. if A + D < T then we know that we can run at the maximum velocity for a given number of steps. 
//       the number will be equal to the difference T - (A + D). 
//    2. if A + D = T then we just run the acceleration curve and the deceleration curve back to back
//    3. if A + D > T then we know we need to remove steps for both curves combined to reach the target 
//       position. the number of steps to remove will be equal to the difference (A + D) - T
//
//    We then have to figure out how many steps run in a given time slice. I think this doesn't work with our current setup
//    without continuous updating of the motors velocity. and since we need to handle other work while steps are being 
//    generated, we have to give more responsibility to the m4 co-processor. 
//    can we give the m4 co-processor the acceleration and target position and have it figure out the rest?
//    I think we have to. 
//
//    the m4 at a minimum has to be able to stop itself in the middle of the time step when it reaches the target position
//    which means it must have access to the acceleration and target position data.
//
//    if the m4 updates the acceleration itself, then it can update at 200kHz just as it updates the velocity into the accumulator 
//    to generate a pulse or not. which is "continuous" updating of the acceleration and velocity.
//
//    if the m4 is doing the updating, then the m7 just has to write in the target position to the shared memory and
//    it is free to go off to do other work. so that bit of code is easy at least.
//
//
//
//
//

struct Motor {
    int32_t position;
};


} // namespace motors

#endif // !LIMNMOCO_CM7_MOTORS_HPP

