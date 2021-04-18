/* See LICENSE file for copyright and license details. */
#include <stdio.h>

#include "util.h"

typedef struct {
	char *name;
	int init;
	unsigned int id;
	int isMassive;
	int isGluon;
} Leg;

typedef struct {
	unsigned int legId;
	unsigned int id;
} Vertex;

void generate_legs(char **init, size_t n_init, char **fin, size_t n_fin, Leg *Legs)
{
	for (int i=0; i<n_init; i++) {
		int isGluon = 0;
		if (!strcmp("g", init[i]))
			isGluon = 1;
		Legs[i] = (Leg){init[i], 1, i, 0, isGluon};
	}

	for (int i=0; i<n_fin; i++) {
		int isGluon = 0;
		if (!strcmp("g", fin[i]))
			isGluon = 1;
		Legs[n_init+i] = (Leg){init[i], 1, n_init+i, 0, isGluon};
	}
}

void generate_vertices(Leg *Legs, size_t nLegs, int nLoops, Vertex *Vertices)
{
	for (int i=0; i<nLegs; i++) {
		for (int j=0; j<nLoops; j++){
			Vertices[i+j] = (Vertex){i, i+j};
		}
	}
}

int main(int argc, char *argv[])
{
	if (argc == 2 && !strcmp("-v", argv[1]))
		die("WilsonWebs-"VERSION);
	else if (argc != 1)
		die("usage: webs [-v]");

	char *initial[] = {"g","g"};
	char *final[]   = {"q","qbar", "g"};
	int nLoops = 1;

	size_t nLegs = NELEMS(initial) + NELEMS(final);
	Leg Legs[nLegs];
	generate_legs(initial,NELEMS(initial), final, NELEMS(final), Legs);

	Vertex *Vertices = (Vertex *)ecalloc(nLoops*nLegs, sizeof(Vertex));
	generate_vertices(Legs, nLegs, nLoops, Vertices);

	printf("%d\n",Vertices[1].legId);

	//Connections = generate_loops(Vertices);

	//draw(Legs, Connections);


	return 0;
}
