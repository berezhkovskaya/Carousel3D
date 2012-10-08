#!/usr/bin/python

import os
import sys
import platform

# Crazy hack: copy a Configuration.LinderdaumX file to Configuration.Linderdaum.X (this _is_ a problem due to another hack in VCauto.py)
if not(platform.system() == "Windows"):
	if not os.path.exists("Solution/Configuration.Linderdaum.X"):
		open("Solution/Configuration.Linderdaum.X","w").writelines(open("Solution/Configuration.LinderdaumX","r").readlines())

# Engine core
os.system( os.path.join("Tools","VCauto.py") + " --core CORE -s Src -pr LinderdaumEngine -o Linderdaum -ver 2008 -ver 2010 -c Solution/Configuration.Linderdaum -qt Solution/Linderdaum.qt -andr1 Solution/Linderdaum.Android1 -andr2 Solution/Linderdaum.Android2 -androut BuildAndroid/jni/Android.mk -t makefile   -m Solution/Targets.linderdaum -i Src -plex Solution/PlatformsExcludes.list" )

# Now process the Apps folder

Path = "Apps"

for App in os.listdir( Path ):
   AppPath = os.path.join(Path, App)
   if os.path.isdir(AppPath):
      if os.path.exists( os.path.join(AppPath, "makeconfig.py" ) ):
         print("")
         print( "----------------------" )
         print("")
         print( "Generation config for:", AppPath )

         if platform.system() == "Windows":
            os.chdir( AppPath )
            os.system( "makeconfig.py" )
         else:
            os.chdir( "./"+AppPath )
            os.system( "chmod +x makeconfig.py" )
            os.system( os.path.join(".", "makeconfig.py") )

         os.chdir( os.path.join("..", ".." ) )
