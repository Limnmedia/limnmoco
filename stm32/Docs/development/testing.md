# Testing Limnmoco


## Modules

The happy path flow of the firmware is roughly as follows

Dragonframe -USB> Transport

Transport -DMCv2> DMC

DMC -> Control-m7

Control-m7 -> Camera
            | Playback
            | Virtuals
            | Motion

Camera -> Intercore

Playback -> Motion

Virtuals -> Kineamatics

Kinematics -> Motion

Motion -> Intercore-m7

Intercore-m7 -> Intercore-m4

Intercore-m4 -> Control-m4

Control-m4 -> Camera
            | Step Generation

Camera -> GPIO

Step Generation -> GPIO

For the case of errors, if they arise in the m7 they are reported back to
control, which handles responding to dragonframe. If they arise in the m4, they
are passed to the m7 and then through control-m7 and back to dragonframe.

So, unmentioned modules are 

Errors

Calibration

Device

Linear Algrebra

## Testing Methods

For now we have one testing method: 
Compile the libraries and unit tests for the host system, and run the unit tests
on the host system.

I would like to define some hardware in the loop tests as well. Ideally, we 
could programmatically interface with the STLINK debugger and gather diagnostic
information about the device under debug at runtime conditions. We could then
define a fake Dragonframe host, connect to the device with it, and exercise the
devices functionality. With full visibility of the devices memory and internal 
state we can test that the steps are being generated properly along with camera
signals, that all code paths are exercised, That the Interlink module is 
tranferring data between the cores correctly, we may also be able to rephrase 
the unit testing in terms of the actual device and rerun the entire unit test
suite against the actual device.

lets call the first method host testing. or unit testing. and the second method
device testing, or hardware in the loop testing. 

Any library that may be tested on the host, may have unit tests run on the 
device if we get that running. 

it bears mentioning that host side unit testing being used to verify the 
validity of device side algorithms is reliant on a property of the compiler.
namely that the code generated from the same souce code is equal in semantics.


### Linear Algebra 

The Linear Algebra library is responsible for providing Linear Algebra
primitives for use by the rest of the firmware. Primarily the Kinematics engine.
As such, we only need a limited subset of the full discipline of Linear Algebra,
Just Vec3 and Mat3x3.

Everything that we do implement with respect to Vec3 and Mat3x3, must be
consistent with the axioms from Linear Algebra. Most of this is basics, such as
the associativity of addition between Vec3s. The correctness of these algorithms
will improve the correctness of the kinematics algorithms.

These functions are easily able to be unit tested in the usual way.
They may also be unit tested on the device if that is something we can get 
working with the tools provided by STMicroelectronics.

### Calibration

The calibration library is responsible for holding the calibration data for the
crane geometry, Conversion between units and steps, boom compensation data,
virtual to physical motor mapping. And other supporting algorithms for 
translating virtual motion into physical motion. 

Why is this separate from Virtual? DO we have some requirement to make each 
library as small as possible? There isn't much difference between many small
libraries and a few larger ones if their content is the same. This libraries
function is so closely tied to just the virtuals that it feels like we could 
just fold it's functionality into virtuals. (And for that matter LinearAlgebra
if only used by Kinematics may be folded into kinematics. However, I believe 
we may use it for path computation within the motion library, as a path through
3d space can be represented as a series of Vec3's. Though, it may also be that 
we only need a single number, representing distance. because these are motors 
we are talking about, and they are only parametrized on one dimension. as they
only allow motion in one dimension. Ref: Modern Robotics)

Regardless, the functions here are a set of utilities which convert between 
units, and read from and update some tables. Their functionality may be unit 
tested in the usual way. There is no special consideration from a testing 
perspective about this library.

### Device

Device owns device wide configuration values. The number of motors able to be 
driven is a constant that lives here, The number of frames that are able to be 
driven lives here. etc. It's mostly the point of contact between the build 
system and the firmware. allowing the internals to be written somewhat 
generically, or at least, parametrized on some axes, allowing for expansion of 
some capabilities without a full rewrite. There are no functions planned, so 
very little testing is necessary, if any. and any that is needed, (if any 
functions exist) can be unit tested in the usual way.

### Errors

Errors is built to pass data between the libraries as errors occur. it may not
be an actual library in practice. It may be the general shape of fallable 
functions within the firmware as a whole, and as such it might not make sense to
consider unit testing the "errors" module. If however, it is designed such that 
there is some, say, global state which is used to write current errors to, and 
then the shape of functions is such that a signal is returned which means an
error occurred, go check the global error state and read the current error.
Then that mechanism of writing and reading errors is worth testing. and 
depending on how it is written, may be unit tested on the host. I do like this
design, especially with it's ability to handle multiple errors simultaneously,
allowing the control algorithm to report each of them. It also keeps the return
values of functions more generally smaller, which should reduce stack space 
usage at runtime. (we dont allocate space for the error structure on the stack)
and if the errors are held in global memory we can avoid heap allocation 
algorithms at runtime. This should make analysis of the time and space bounds,
both upper and lower, easier.

### DMC

DMC is responsible for conversion between raw bytes and typed packets. 
Theoretically this could be placed into the transport layer, Though I find that
the set of functions is so large and specialized that it deserves it's own 
library. That way all the functionality is consolidated to one area, and we can 
focus testing and implementation on just those routines. 

Since these algorithms are just regular functions we are able to test them in 
the usual way. With host side unit testing. It will be nice to verify 
functionality with device side testing.

### Kinematics

Kinematics is responsible for the forward and inverse kinematics algorithms.
These are also regular functions and so can be tested in the usual way. 

### Motion

The motion planner is responsible for single and coordinated motion of the 
stepper motors. Though this is done at the level of unit distances, and not 
steps. 
These routines are also regular functions and so can be tested in the usual way.
with host side unit testing. Though it will be nice to verify functionality with
device side unit testing.

### Transport

Transport is responsible for the incoming and outgoing queues of DMCv2 packets.
It is conceptually tied to the USB HAL/LL functions, however, it may be possible
to introduce small shim functions which allow us to simulate a USB HAL/LL 
function producing bytes "over-the-wire" and by that method exercise the 
Transport queues behavior in host side unit testing. We could use either a 
function pointer held in a structure, or an extern function declaration whose
implementation is provided by either the testing library or a small shim library
when built for either the host side or device side. In this way we can test 
the libraries in the usual way. And again, it will be nice to verify 
functionality with device side unit testing.

### Playback

Playback is responsible for the buffer of frame data, (motor positions and DMX
data) and routines which allow for playing through that buffer. This is regular
functions which can be tested host side without modification. And again, it will
be nice to verify functionality with device side unit testing.

### Camera

Camera is responsible for creating the packet to send over the Intercore connect
to trigger either a read or a write of the Camera GPIO pins. This functionality
is possible to test with host side unit testing. though the Intercore connect
will need to be replaced with a test shim.

### virtual

virtual is responsible for taking the virtual packets, and coordinating the 
kinematics and motion modules to produce the desired movents of the crane. 

The more I think about it, the more it seems like that means that it is 
responsible for what we said calibration is responsible for.
what functions are we talking about when we say calibration and what functions
are we talking about when we say virtual. Virtual can also mean kinematics,
because it needs to use the kinematics algorithms, and it does so exclusively.

This is something that needs to be though through more thouroughly. 

### intercore

Intercore is responsible for the synchronous queue between the m7 and m4 
processors. The queue itself may be able to be extracted and shimed on both 
sides for host side unit testing. However, the Actual critical functionality of
the queue cannot be verified without device side unit testing. This is because
the implementation is highly dependent on the processor and how interrupts are 
handled. For instance. If we use an interrupt to signal between the cores. 
"There is data available to read" we must test to ensure that this interrupt 
does not cause the m4 to lose a step, or mistime a step. This case would be 
considered a failure of the algorithm. This and the synchronization between the 
cores cannot be properly tested without a device in the loop. I have been 
thinking about this, and we might be able to simulate this by running two test
cores as threads, and placing the queue between the threads. However, the 
synchronization primitives and read/write semantics are so different between 
x86 and these particular arm cores that I have less confidence in the 
correctness of such a test to actually prove validity of the underlying queue.

essentially, I trust host side unit testing to establish that the read/write 
semantics are correct, and not the synchronization semantics.
Testing this module is the inspiration for the hardware in the loop testing.

### control

The control module is responsible for the main loop of the program. as well 
as the dispatch code over the incoming packets, and posting outgoing packets.
I belive since these functions are delegated to other libraries this core 
routines are sufficiently isolated from the hardware that they may be tested 
with host side unit testing. This remains to be determined. and I am 
particularly interested in how we can write tests for the main loop of the 
program. Perhaps we write a function that encapsulates a single iteration, and 
this may then be tested? it bears some more thought.



