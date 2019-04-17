 # Data Logging Examples for the MKR series Arduino Microcontrollers


 ## Formatting an SD Card

Creating a FAT16/FAT32 SD card to work with the library on MacOS:
On the mac terminal, list your disks:

````
$ diskutil list
````

You should get a response like this:

````
/dev/disk2 (external, physical):
````

etc.

Then format your disk:

````
$ sudo diskutil eraseDisk FAT32 DISKNAME MBRFormat /dev/disk2
````

DISKNAME needs to be all caps, 8 characters or less.
Set the disk name based on the list of disks. For example, `/dev/disk2` above comes from the list you got from diskutil list.

## Timing: `millis()` vs. Real-time Clock


## Putting the Processor to Sleep
It's a good practice when you put the processor to sleep to use the built-in LED to indicate when it's awake. When the processor is asleep, however, it won't show up as a serial port, which can make reprogramming tricky. Two simple tricks make programming workflow with sleep functions easier:

* Put a `delay()` in the setup, 1-3 seconds, so that the processor stays awake a bit longer and acknowledges upload from the IDE. This is also useful if you're printing any diagnostic info in the setup.
* When the processor is asleep, double-tap the reset button to put it in bootloader mode. The bootloader will start and the processor will show up as a serial device, but the sketch won't start, so it won't go to sleep. This makes it easy to reprogram.