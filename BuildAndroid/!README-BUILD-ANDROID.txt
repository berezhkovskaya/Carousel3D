
  Linderdaum Engine building tutorial for Android
===================================================

Please read "!README-BUILD.txt" carefully before reading this tutorial!

This tutorial will explain to you how to build Linderdaum Engine for
Android operating system.


1. Prerequisites
------------------

To build Linderdaum Engine for Android you will require:
 - Cygwin  (Windows only)
 - Android SDK r8
 - Android NDK r5b (refer to http://developer.android.com)
 - Apache Ant
 - Java SE Development Kit 6
 - GNU TAR (optional, only to build CommonMedia.rar for Android examples)
 - a device with Android 2.2 to run the activity

In Debian/Ubuntu only the Android SDK/NDK must be downloaded and installed
separately. Follow the instructions from the official Android site.
Everything else is easily installed using the Apt package manager.

2. Configuring environment
----------------------------

 - set JAVA_HOME environment variable to your JDK folder
   (on Ubuntu with OpenJDK it is usually /usr/lib/kvm/default-java)
 - set NDK_ROOT environment variable to your Android NDK folder
 - add path to your Android SDK to 'trunk/Apps/*/local.properties' as 'sdk.dir='

3. Building
-------------

Under Windows:
   - Start Cygwin Bash Shell and change directory to 'trunk/BuildAndroid'
   - Create libLinderdaumEngineCore.a using the following command:
        ndk-build -j4

Under Linux: 
  - Simply create libLinderdaumEngineCore.a using the following
    command in the 'trunk/BuildAndroid' directory:
        ndk-build -j4

After this a static library will be created that can be used in your applications.
See Apps/PuzzL for usage example. To build use "ant copy-common-media debug" command.

4. Feedback
-------------

Please, report all bugs and your feedback 
to Linderdaum Engine forums at: http://www.linderdaum.com
