# UTAustinX: UT.RTBN.12.01x Realtime Bluetooth Networks

This contains my answers to the lab exercises of the EdX course 
[UTAustinX: UT.RTBN.12.01x Realtime Bluetooth Networks](https://www.edx.org/course/real-time-bluetooth-networks-shape-world-utaustinx-ut-rtbn-12-01x-0).

This course teaches the design fundamentals of a real-time operating system (RTOS) and how to build a Bluetooth-connected (ARM) device.

I used the following hardware:
* MSP432 LaunchPad (MSP-EXP432P401R)
* MKII-Educational Booster Pack (BOOSTXL-EDUMKII)
* CC2650 LaunchPad (LAUNCHXL-CC2650)

Lab assignments:
* Lab 1 (Introduction to I/O): Modify a superloop application to run a certain task at a specific frequency. This application is a fitness device. It inputs from the microphone, accelerometer, light sensor, and buttons. It performs some simple measurements and calculations of steps, sound intensity, and light intensity. It outputs data to the LCD and generates simple beeping sounds.
* Lab 2 (Thread management): Implement a very simple RTOS that runs main threads and event threads. Main threads are run using round-robin scheduling. An event thread is run periodically with interrupts disabled. Also implement spinning semaphores.
* Lab 3 (Blocking, Sleeping and FIFO Queues): Extend the RTOS of Lab 2 by implementing thread sleeping, blocking semaphores, and FIFO queues.
* Lab 4 (Priority Scheduler): Modify Lab 3 to use a priority scheduler. The (ready) thread with the highest priority always runs. Threads with the same priority are run using round-robin scheduling. Also implement handling of button presses using interrupts instead of polling.
* Lab 5 (File System using a Solid-State Disk): Implement a write-once FAT filesystem that uses the flash memory for storage.
* Lab 6 (Bluetooth BLE): Interface the CC2650 Bluetooth module to the MSP43 LaunchPad, implement message packet builders for TI's Network Protocol Interface (NPI) to support Bluetooth communication, and connect the fitness device to a smartphone.
