#! /usr/bin/python

import os


# package name, package location
Packages = ( ( "LinderdaumCore", "Src"                                          ), 
#            ( "AcesHigh",       os.path.join( "Apps", "AcesHigh",      "Src" ) ),
             ( "FightingChess",  os.path.join( "Apps", "FightingChess", "Src" ) ),
             ( "Runner",         os.path.join( "Apps", "Runner",        "Src" ) ),
             ( "PuzzL",          os.path.join( "Apps", "PuzzL",         "Src" ) ),
             ( "Paratrooper",    os.path.join( "Apps", "Paratrooper",   "Src" ) )
           )


GlobalOutDir = "Out"

ExcludeDirs  = [ ".svn" ]
ExcludeFiles = [ ".gitignore" ]

######################
######################
######################
######################
######################
######################

print("")
print( "Rebuilding LinderScript Database" )
print("")

# remove all files from folder
def RemoveSources(OutputPath):
   for Root, Dirs, Files in os.walk( OutputPath ):
      for Dir in ExcludeDirs: 
         if Dir in Dirs: Dirs.remove( Dir )
      for File in Files:
         if File in ExcludeFiles: continue
         FullName = os.path.join( Root, File )
         os.remove( FullName )

def MoveAll(InPath, OutPath):
   for Root, Dirs, Files in os.walk( InPath ):
      for Dir in ExcludeDirs: 
         if Dir in Dirs: Dirs.remove( Dir )
      for File in Files:
         if File in ExcludeFiles: continue
         FullName = os.path.join( Root, File )
         os.rename( FullName, os.path.join( OutPath, File ) )

def mkdir(Path):
   if not os.path.exists( Path ): os.mkdir( Path )

def rename(In,Out):
   if not os.path.exists( In ): return
   if os.path.exists( Out ): os.remove( Out )
   os.rename(In, Out)

def PreProcessPackage( Package ):
   # clean old LSDC output
   RemoveSources( os.path.join( GlobalOutDir, "LSDC_"+Package[0] ) )

def PostProcessPackage( Package ):
   ExportsDir       = os.path.join( Package[1], "Generated", "Exports")
   SerializationDir = os.path.join( Package[1], "Generated", "Serialization")
   # clear output directory for tunnellers
   RemoveSources( ExportsDir )
   # clear output directory for serialization
   RemoveSources( SerializationDir )
   # serialization code for the FC
   MoveAll( os.path.join( GlobalOutDir, "LSDC_"+Package[0], "Serialization" ), SerializationDir )
   MoveAll( os.path.join( GlobalOutDir, "LSDC_"+Package[0], "Export" ), ExportsDir )
   MoveAll( os.path.join( GlobalOutDir, "LSDC_"+Package[0], "Script" ), ExportsDir )
   # copy enums
   SrcEnumsH    = os.path.join( GlobalOutDir, "LSDC_"+Package[0], Package[0] + "_EnumConverters.h")
   SrcEnumsCPP  = os.path.join( GlobalOutDir, "LSDC_"+Package[0], Package[0] + "_EnumConverters.cpp")
   DestEnumsH   = os.path.join( Package[1], "Generated", Package[0] + "_EnumConverters.h")
   DestEnumsCPP = os.path.join( Package[1], "Generated", Package[0] + "_EnumConverters.cpp")
   if os.path.exists( DestEnumsH ): os.remove( DestEnumsH )
   if os.path.exists( DestEnumsCPP ): os.remove( DestEnumsCPP )
#   rename(SrcEnumsH,   DestEnumsH)
#   rename(SrcEnumsCPP, DestEnumsCPP)

#
# Build all packages
#

PackageList = ""

for Package in Packages: 
   PreProcessPackage( Package )
   PackageList += " --package " + Package[1]

os.system( os.path.join("Tools","LSDC","LSDC") + PackageList + " --statistics --exclude pugixml.hpp" )

for Package in Packages: PostProcessPackage( Package )

# .NET code
rename( os.path.join( "Out", "LSDC_LinderdaumCore", "NET", "LinderdaumNET.cpp" ), os.path.join("Apps.NET","ManagedAPI","LinderdaumNET.cpp") )
rename( os.path.join( "Out", "LSDC_LinderdaumCore", "NET", "LinderdaumNET.h"   ), os.path.join("Apps.NET","ManagedAPI","LinderdaumNET.h") )

os.remove( "Debug_NETTypes.list" )
os.remove( "Debug_StringCvt.list" )

print( "Done." )
