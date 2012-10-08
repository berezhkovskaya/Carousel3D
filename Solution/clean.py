#! /usr/bin/python

import os

print( "Cleaning solution" )

ExcludeDirs  = [ ".svn" ]
ExcludeFiles = [ ".gitignore" ]
OutputPath  = "Out"

# remove all files from output folder
for Root, Dirs, Files in os.walk( OutputPath ):
   for Dir in ExcludeDirs: 
      if Dir in Dirs: Dirs.remove( Dir )
   for File in Files:
      if File in ExcludeFiles: continue
      FullName = os.path.join( Root, File )
      os.remove( FullName )

# remove all vcproj in current dir
Path = "."

def IsProject(Name):
   return Name.endswith( ".vcproj" ) or Name.endswith( ".vcxproj" )

for Item in os.listdir( Path ):
   ItemPath = os.path.join(Path, Item)
   if os.path.isfile(ItemPath) and IsProject(ItemPath):
      os.remove( ItemPath )