# Progetto_RT
This collection of file is the result of my project, developed in order to pass the 
Real Time Embedded System exam.

Application goal:

Goal of this application is to simulate, in industrial scope, the behavior of a tank
that want to maintain your own liquid at a certain level according with a proximity sensor
located on top of tank.

The tank has a button, controlled by graphical interface, that allow to open a tap 
so to empty the tank.

When tank's level goes beyond the minimum desiderated a filler task is activate automatically.

Application structure:

In the "Progetto_RT" folder there are three file related to code, one file, called "makefile"
that manage dipendences, libraries and so on in the project and a readme.md file like this.

main.c file include the main function of application. It serves to load the graphical interface
and run threads.

mylib.h file is a my properties library generated to group all the function prototype and the 
data structures.

mylib.c file implement function and threads behaviour managing all the situation.
