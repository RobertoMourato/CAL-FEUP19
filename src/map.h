#ifndef MAP_H_
#define MAP_H_

#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



#include "Estrada.h"
#include "node.h"
#include "graphviewer.h"


class Map {
private:
	GraphViewer graphviewer;
	std::vector<Node> pontos;
	std::vector<Node> interece;
	std::vector<Estrada> estradas;
	std::vector<Node> solucao;

	void addNode (int id, float x, float y, Tipo tipo);
	void addEstrada (int id, int nodeIdInicio, int nodeIdDestino);
public:
	Map (string cidade);
	void solution ();
	void dijkstra (Node init, Node dest);
};


#endif
