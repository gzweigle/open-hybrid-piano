# IPS 2.X Damper

Damper processing code running on a Teensy 4.1 processor and the IPS 2.X mainboard.

Place this code in the same directory location as the main Arduino project for the damper board.

May need to change the extension of *src_damper.cpp* to *src_damper.ino* before compiling.

In damper_settings.cpp is the following code:

  ////////

  // Ethernet data.

  //

  MUST EDIT TO ADD VALUES BEFORE RUNNING.

  snprintf(teensy_ip, 16, "X.X.X.X");   // Arbitrary assigned Teensy IP

  snprintf(computer_ip,16, "X.X.X.X");  // Get from ipconfig command on local computer

  upd_port = X;  // Must match UDP port in receiver code

  //

Before compiling, replace the X's with Ethernet and port addresses and then comment out the statement "MUST EDIT TO ADD VALUES BEFORE RUNNING."

If not using Ethernet, then put any numbers in place of the X placeholders.