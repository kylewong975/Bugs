# Bugs
Bugs! A game simulation where contestants make their own ants and compete for resources. 

### Instructions
1. Download the repository. 
2. To run the executable, run the following command in the command shell: 
`Bugs.exe field.txt ____.bug ____.bug _____.bug _____.bug`
* Bugs.exe is the executable. 
* The next argument is the field text file, which contains the necessary information to 
generate a field containing poison, pool of water, grasshoppers, anthills, food, and 
pebbles in their respective locations. 
* The next 0-4 arguments are the .bug files, which are the contestant's ant program 
to be run in the simulation. The 1st argument will place contestant 0's ants in anthill 0, 
2nd argument will place contestant 1's ants in anthill 1, etc. If fewer than 4 arguments 
are supplied, then the respective anthills will simply do nothing. 

When running the above command, a window should pop up, prompting you to press Enter to begin. 

### About the Repository
You should see, in the root of the repository, some dll files, Bugs.ilk, Bugs.pdb, 
and the Bugs.exe executable. Do not change any of those contents. You also see the 
field.txt and the various .bug files. The field is for the map of the simulation and the 
.bug files are the ant programs which run under the simulation. 

The Ants folder contains some sample ant files. You may also put your own ant files here 
as well, to keep the root of the repository less cluttered. 

The Assets folder contains the images rendered in the game simulation. 

The Fields folder contains the sample field files. You may also put your own field files here 
as well, to keep the root of the repository less cluterred. 

The Instructions folder contains the competition rules and instructions on how to program your 
own ant! 

The src folder contains the source code of this game simulation. It contains code I wrote, along 
with the open source libraries such as the freeglut and irrKlang. 

### Customizations
##### Fields
You don't have to use the field being provided! You can add or change the field to 
whatever you want. It can have lots of poison, if you are up for the challenge; lots 
of pebbles, if you want only the brightest of the ants to be able to navigate through 
the field; lots of food, to see which anthill produces the most ants; the choice is 
yours! 

To make your own field, here are just a few restrictions and instructions: 
* The field must be of size 64x64 square. 
* Surround the field with pebbles (that is, the boundaries)
* To not have anything in a given location, simply leave an empty space ' '
* To place a pool of water, put 'w'
* To place food, put 'f'
* To put a pebble, put '\*'
* To put poison, put 'p'
* To place the anthills, designate '0' for anthill 0, '1' for anthill 1, 
'2' for anthill 2, and '3' for anthill 3 (0-based indexing)
* To place a grasshopper, put 'g' 

##### Ants
Feel free to program your own ant and see if you can beat the sample ant files provided! 
Instructions to build your own ant and competition rules are given in the file under the 
repository folder Instructions. Your Ant files should have a .bug extension. 
