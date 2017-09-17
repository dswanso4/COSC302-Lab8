#include "city_map.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstdio>
#include <math.h>
#include <list>

/*
	city_map.cpp
	David Swanson
	November 3, 2015

	This program implements the constructor and the Dijkstra function in the City_Map
	class from the city_map.h header file. In the constructor, a graph is built depicting 
	a city with one way and both way streets. The edges are road segments and the nodes are
	intersections. The Dijkstra function implements Dijkstra's algorithm to find the fastest
	path from the first intersection to the furthest intersection for the best, worst, and
	average case.
*/

using namespace std;

City_Map::City_Map() {
	int i, j, k;		/* Induction variables */
	int s, a;			/* Input street and avenue number */
    int ssize, asize;	/* Number of steets, number of avenues */
	double c, d;		/* Input x and y coordinates */
	double gs, ga;		/* Green light times for both */
	char buf[50];		/* Buffer for sprintf */
	string str;			/* String that sprintf writes to */
	vector <int> tmp1, tmp2;				/* Helps keep input order*/
	vector <string> input;					/* Holds all the inputs */
	vector< vector<Intersection *> > grid;	/* 2D vector that represents city */
	Road_Segment * rs;						
	Intersection * I;
	i = 0;
	j = 0;

	/* Stores all the inputs into a vector so that. This detour is done so that the max
	   street number and avenue number can be found before making the grid. */

	while (cin >> s >> a >> c >> d >> gs >> ga) {
	  if (s > i)
		i = s;
	  if (a > j)
		j = a;
	  sprintf(buf, "%d %d %lf %lf %lf %lf", s, a, c, d, gs, ga);
	  str = buf;
	  input.push_back(str);
	}

	/* Grid is resized with max values */

	ssize = i+1;
	asize = j+1;
	grid.resize(ssize);
	for (k = 0; k < ssize; k++) {
	  grid[k].resize(asize);
	}

	/* Values are grabbed from the inputs and put into an instance of an Intersection.
	   backedge and best_time are initialized for the Dijkstra function. Temp vectors are 
	   used to keep track of the order that the intersections are input. */

	for (i = 0; i < input.size(); i++) {
	  sscanf(input[i].c_str(),"%d %d %lf %lf %lf %lf", &s, &a, &c, &d, &gs, &ga);
	  I = new Intersection;
	  I->street = s;
	  I->avenue = a;
	  I->x = c;
	  I->y = d;
	  I->green[0] = gs;
	  I->green[1] = ga;
	  I->backedge = NULL;
	  I->best_time = -1;
	  grid[s][a] = I;
	  tmp1.push_back(s);
	  tmp2.push_back(a);
	}

	/* first and last point to the starting intersection and the furthest intersection.
	   Then edges are set based on given requirements. Even street and avenue numbers are 
	   one way descending order, odd street and avenue numbers are one way ascending order,
	   multiples of 5 are both ways, and largest avenue is both ways. Each road segment is 
	   an edge.*/

	first = grid[0][0];
	last = grid[ssize-1][asize-1];
	for (i = 0; i < ssize; i++) {
	  for (j = 0; j < asize; j++) {
		/* Sets East edges */
		if (j < (asize-1) && (i%2 == 0 || i%5 == 0)) {
		  rs = new Road_Segment;
		  rs->type = STREET;
		  rs->number = i;
		  rs->from = grid[i][j];
		  rs->to = grid[i][j+1];
		  c = grid[i][j]->x - grid[i][j+1]->x;
		  d = grid[i][j]->y - grid[i][j+1]->y;
		  rs->distance = sqrt((c*c) + (d*d));
		  grid[i][j]->adj.push_back(rs);
		}
		/* Sets West edges */
		if (j != 0 && (i%2 != 0 || i%5 == 0)) {
		  rs = new Road_Segment;
		  rs->type = STREET;
		  rs->number = i;
		  rs->from = grid[i][j];
		  rs->to = grid[i][j-1];
		  c = grid[i][j]->x - grid[i][j-1]->x;
		  d = grid[i][j]->y - grid[i][j-1]->y;
		  rs->distance = sqrt((c*c) + (d*d));
		  grid[i][j]->adj.push_back(rs);
		}
		/* Sets North edges */
		if (i != 0 && (j%2 != 0 || j%5 == 0 || j == (asize-1))) {
		  rs = new Road_Segment;
		  rs->type = AVENUE;
		  rs->number = j;
		  rs->from = grid[i][j];
		  rs->to = grid[i-1][j];
		  c = grid[i][j]->x - grid[i-1][j]->x;
		  d = grid[i][j]->y - grid[i-1][j]->y;
		  rs->distance = sqrt((c*c) + (d*d));
		  grid[i][j]->adj.push_back(rs);
		}
		/* Sets South edges */
		if (i < (ssize-1) && (j%2 == 0 || j%5 == 0 || j == (asize-1))) {
		  rs = new Road_Segment;
		  rs->type = AVENUE;
		  rs->number = j;
		  rs->from = grid[i][j];
		  rs->to = grid[i+1][j];
		  c = grid[i][j]->x - grid[i+1][j]->x;
		  d = grid[i][j]->y - grid[i+1][j]->y;
		  rs->distance = sqrt((c*c) + (d*d));
		  grid[i][j]->adj.push_back(rs);
		}
	  }
	}

	/* Intersections are pushed into the provided list based on the order they were input */

	for (i = 0; i < input.size(); i++) {
	  all.push_back(grid[tmp1[i]][tmp2[i]]);
	}

	/* Prep for the Dijsktra function. */

	first->best_time = 0;
	bfsq.insert(make_pair(first->best_time, first));
}

double City_Map::Dijkstra(int avg_best_worst) {
	double time;		/* Time to travel road segment plus previous time */
	double R, G, avg;	/* Used to add time for average case */
	multimap<double, Intersection *>::iterator mit;	
	list <class Road_Segment *>::iterator lit;		
	Intersection * I;								

	/* All calculations are done assuming best case first. The program loops until there
	   are no road segments left to process. Each road segment attached to an intersection
	   is traversed. */

	while (bfsq.size() != 0) {
	  mit = bfsq.begin();
	  for (lit = mit->second->adj.begin(); lit != mit->second->adj.end(); ++lit) {
		time = (mit->second->best_time) + ((*lit)->distance / 30 * 3600);
		/* Adds corresponding red light times for all intersections if worst case.*/
		if (avg_best_worst == 'W') {
		  if ((*lit)->type == STREET)
			time += (*lit)->to->green[1];
		  else 
			time += (*lit)->to->green[0];
		}
		/* Adds the average waiting time for all intersections if average case */
		if (avg_best_worst == 'A') {
		  if ((*lit)->type == STREET) {
			R = (*lit)->to->green[1];
			G = (*lit)->to->green[0];
			avg = (R*R)/(2*(R+G));
			time += avg;
		  }
		  else {
			G = (*lit)->to->green[1];
			R = (*lit)->to->green[0];
		    avg = (R*R)/(2*(R+G));
			time += avg;
		  }
		}

		/* A node is only processed if it has not been visited yet or the time corresponding 
		   to the node is greater than the current time. If the node is already in the queue,
		   it is deleted. The values in the intersection that the road segement is pointing 
		   to are set and the next nodes are added to the queue. Once done, the currently 
		   processed node is removed from the queue. */

	    if((*lit)->to->best_time == -1 || (*lit)->to->best_time > time) {
		  mit->second->bfsq_ptr = bfsq.find((*lit)->to->best_time);
		  if (mit->second->bfsq_ptr != bfsq.end()) {
			bfsq.erase(mit->second->bfsq_ptr);
		  }
          (*lit)->to->best_time = time;
		  (*lit)->to->backedge = (*lit);
		  bfsq.insert(make_pair(time, (*lit)->to));
		}
	  }
	  bfsq.erase(mit);
	}

	/* Backedges are followed to find the shortest path. Push_front is used to keep the path
	   in the correct order. */

	I = last;
	while (I->backedge != NULL) {
	  path.push_front(I->backedge);
	  I = I->backedge->from;
	}

	return 0;

}
