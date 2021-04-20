/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>

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

typedef struct {
	Vertex emitter;
	Vertex absorber;
} Link;

void
generate_legs(char **init, size_t n_init, char **fin, size_t n_fin, Leg *Legs)
{
	for (int i=0; i<n_init; ++i) {
		int isGluon = 0;
		if (!strcmp("g", init[i]))
			isGluon = 1;
		Legs[i] = (Leg){init[i], 1, i, 0, isGluon};
	}

	for (int i=0; i<n_fin; ++i) {
		int isGluon = 0;
		if (!strcmp("g", fin[i]))
			isGluon = 1;
		Legs[n_init+i] = (Leg){init[i], 1, n_init+i, 0, isGluon};
	}
}

void
generate_vertices(Leg *Legs, size_t nLegs, int nLoops, Vertex *Vertices)
{
	for (int i=0; i<nLegs; ++i) {
		for (int j=0; j<nLoops; ++j) {
			/* TODO: Don't forget to add vertices for self-energies later */
			Vertices[j+nLoops*i] = (Vertex){i, j+nLoops*i};
		}
	}

	/*
	for (int i = 0; i < nLegs*nLoops; ++i) {
		printf("test %d\n", Vertices[i].legId);
	}
	*/
}

void
generate_loops(Vertex *Vertices, int *deletedVertices, Link *Graphs, size_t nVx, int nLoops, int nGenLoops)
{
	if (deletedVertices[nVx-1])
		return;

	Vertex *AllPossConns = (Vertex *)ecalloc(nVx-1, sizeof(Vertex));

	/* Find the first non-deleted vertex */
	int mVx = 0;
	for (int iVx=0; iVx<nVx; ++iVx) {
		if (!deletedVertices[iVx]) {
			mVx = iVx;
			break;
		}
	}

	size_t nConns = 0;
	for (int iVx=0; iVx<nVx; ++iVx) {
		if (!deletedVertices[iVx] && Vertices[iVx].legId != Vertices[mVx].legId) {
			AllPossConns[nConns++] = Vertices[iVx];
		}
	}

	/* We build the connections here */
	static int iGraph = 0;
	for (int iVx = 0; iVx < nConns; ++iVx) {
		if (!nGenLoops)
			printf("yess");
		Graphs[iGraph*nLoops+nGenLoops] = (Link){Vertices[mVx], AllPossConns[iVx]};
		printf("V%d%d - V%d%d\n", Vertices[mVx].legId, Vertices[mVx].id, AllPossConns[iVx].legId, AllPossConns[iVx].id);
		/* Gluon connection introduces new vertices.
		 * Gluon legId = (emitter legId + 100)
		 */

		/*
		for (int i = 0; i < nLoops-nGenLoops; ++i) {
			AllPossConns[i] = (Vertex){Vertices[mVx].legId+100,i}
		}
		*/

		deletedVertices[mVx] = 1;
		deletedVertices[AllPossConns[iVx].id] = 1;
		nGenLoops++;
		if(nGenLoops < nLoops) {
			generate_loops(Vertices, deletedVertices, Graphs, nVx, nLoops, nGenLoops);
		}
		else if (nGenLoops == nLoops) {
			nGenLoops--;
			iGraph++;
		}
	}
	free(AllPossConns);

	for (int iVx=0; iVx<nVx; ++iVx)
		if (iVx > mVx)
			deletedVertices[iVx] = 0;
		else
			deletedVertices[iVx] = 1;

	generate_loops(Vertices, deletedVertices, Graphs, nVx, nLoops, nGenLoops);

	//memmove(&AllPossVertices[0], &Vertices[1], (n_vx - 1) * sizeof(Vertex));

}


int
main(int argc, char *argv[])
{
	if (argc == 2 && !strcmp("-v", argv[1]))
		die("WilsonWebs-"VERSION);
	else if (argc != 1)
		die("usage: webs [-v]");

	char *initial[] = {"g","g"};
	char *final[]   = {"q","qbar", "g"};

	int nLoops = 2;

	size_t nLegs = NELEMS(initial) + NELEMS(final);
	Leg Legs[nLegs];
	generate_legs(initial,NELEMS(initial), final, NELEMS(final), Legs);

	Vertex *Vertices = (Vertex *)ecalloc(nLoops*nLegs, sizeof(Vertex));
	generate_vertices(Legs, nLegs, nLoops, Vertices);

	printf("%d\n",Vertices[0].legId);
	printf("%d\n",Vertices[1].legId);

	size_t nVx = nLegs*nLoops;
	int *deletedVertices = (int *)ecalloc(nVx, sizeof(int));
	Link *Graphs = (Link *)ecalloc(300*nLoops, sizeof(Link));

	generate_loops(Vertices, deletedVertices, Graphs, nVx, nLoops, 0);

	for (int iVx=0; iVx<300; ++iVx) {
		printf("Graph ID: %d\n",iVx);
		for (int i=0; i<nLoops; ++i) {
			printf("V%d%d-V%d%d\n",Graphs[iVx*nLoops+i].emitter.legId, Graphs[iVx*nLoops+i].emitter.id, Graphs[iVx*nLoops+i].absorber.legId, Graphs[iVx*nLoops+i].absorber.id);
		}
	}

	//draw(Legs, Connections);


	return 0;
}
