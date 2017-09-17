# COSC302-Lab8
Use Dijkstra's algorithm to find paths through a city map

# city_map_base.cpp / city_map_base.o
This file was given and is used in generating the executable and various handler functions.

# city_map.h
This file specifies the structures used throughout the program. An intersection, Road segment, 
and City Map class are declared.

# city_map.cpp / city_map.o
This file does the bulk of the work in the program. The graph structure is build using the 
information listed in a given text file. The nodes are intersections, and the edges are road
segments. The entire graph is the city map. This program allows the user to find the best, worst,
and average path from the first intersection to the last intersection. This is done by using 
variants of Dijkstra's algorthim.

# makefile
Use this to compile the program.

# Usage
Parameter 1: Executable name
Parameter 2: Type of path
Parameter 3: Either display time, the routes picked, or the graph of the map.

./city_map none|best|worst|avg time|print|jgraph -map on standard input
Example:
./city_map best print < city-2.txt
