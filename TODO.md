# TODO



# BUGS

- virtual motor position still not updating (fix: ???) 
    This is now working, I'm not exactly sure what changed.
- virtual motor moves cause continuous movement (fix: ???)
    THis is now no longer happening, Im' not exactly sure what changed.
- virtuals are reported as unsupported (fix: 0x095c8bf)
- boom and track are swapped. (fix: dragonframe motor assignment)
    This is still happening, It was fixed temporarily, and is back again. (regression)

- Track reports a large value when starting a jog, (580mm) which is strange, the other 
    virtual axis report resonable values (i.e. incrementing up or down from 0.0 as we jog the motor)

    We need to see when this is happening in the device. 
    This is happening on the first iteration of the solve FK function.
    My guess is that we have narrowed down the possibilities to an issue in the FK setup or the 
    FK math itself, which is why it is producing this specific 580.9 value each time.

    it is particularly suspiscious that it is always 580 and it is the same when we touch any other 
    motor.

    it clears after we go back to normal movement, though that is because we clear the entire virtuals 
    struct when we receive the virt_config=none packet.

    fix: We compute a baseline crane position when the crane is configured for virtuals, then
    when we are computing the Forward Kinematics to track the cranes virtual position as we jog
    we subtract off the baseline pose from the current absolute pose, which retrieves for us the 
    relative pose. which is what is expected for dragonframe.

- swing position reported as 5000 degrees when it actually moved something closer to 15 degrees
    - swing position is no longer reporting as incorrect, yet now track is reporting as incorrect?
      are these related? and maybe it changed because track and boom are now swapped again,


how can we observe what is happening to fix the issue with the track position?

- we can use the new rewrite, which is a lot of work to even get off the ground
- we can use more debug pins. which is tough because we can't get any data out of them.
    - what is the sequence in the code from which we could deduce the incorrect behavior from?

- I added a serial print of the incoming and outgoing bytes, these are then decoded by hand.




