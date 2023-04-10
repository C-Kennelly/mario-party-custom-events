VSCode, by default, includes anything in the workspace directory as part of its IncludePath for Intellisense.

So!  This directory is a copy of the ultra64.h and dependent files so that the ultratypes don't trigger syntax error highlighting with the default

All files were copied from from http://n64devkit.square7.ch/header/
// Header file: http://n64devkit.square7.ch/header/ultra64.htm
// Ultratypes: http://n64devkit.square7.ch/header/ultratypes.htm

One note - the ultra64/PR/ultratypes.h file was edited from the original files to remove an ifdef statement at the top so that the ultratypes symbols are always defined even if the system doesn't have the variable set.