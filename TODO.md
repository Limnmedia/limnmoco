# TODO



# BUGS

- virtual motor position still not updating (fix: ???) 
    This is now working, I'm not exactly sure what changed.
- virtual motor moves cause continuous movement (fix: ???)
    THis is now no longer happening, Im' not exactly sure what changed.
- virtuals are reported as unsupported (fix: 0x095c8bf)
- boom and track are swapped. 
    This is still happening, It was fixed temporarily, and is back again. (regression)
- Track reports a large value when starting a jog, (580mm) which is strange, the other 
    virtual axis report resonable values. We need to see when this is happening in the device. 
- swing position reported as 5000 degrees when it actually moved something closer to 15 degrees
    - swing position is no longer reporting as incorrect, yet now track is reporting as incorrect?
      are these related? and maybe it changed because track and boom are now swapped again,


how can we observe what is happening to fix the issue with the track position?

- we can use the new rewrite, which is a lot of work to even get off the ground
- we can use more debug pins. which is tough because we can't get any data out of them.
    - what is the sequence in the code from which we could deduce the incorrect behavior from?
    




