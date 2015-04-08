# AutoInclude
Automatically generate header files containing the proper file locations of relevant headers and assets in a project, integrates well with any IDE-autocompletion


#Goal
The goal of this project is to create a program that will auto-generate a header file that contains the proper file-locations
files in a project's directory. I'm making this because I'm also making a game engine and I got tired of refactoring and
scouring through mountains of files in order to fix minor include changes. I love to organize files but I hate organizing
them when it comes to programming. Thus I've created this project to mitigate the time spent organizing things.

I also think it would be nice to be able to have the same functionality for asset files so that I won't have to wade through
a sea of broken script files. Maybe in the future this program will be able to automatically generate headers based on
implementation files that may be using outdated header files. For now though, the project will do just enough of what I need.

Of course, you COULD just use stdafx.h, but it's nice to automatically track the changes and additions to a project folder
