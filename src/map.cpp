#include "map.h"
#include "graphviewer.h"


/*FUNCTIONS FOR CLASS NODE*/

/**
* @brief      Function to add a node to the map.
*/
void Map::addNode (int id, float x, float y, Tipo tipo) {
	Node* novo = new Node(x, y, id, tipo);
	this->pontos.push_back(novo);

	Tipo tip = NONE;
	if (tipo != tip) {
		this->interece.push_back(novo);
	}
}

/**
 * @brief      Gets the information if it is possible to add the road on the map.
 *
 * @return     Whether it is possible to add the road(true) or not(false).
 */
bool Map::addEstrada (int id, int nodeIdInicio, int nodeIdDestino) {
	Node* inicio = NULL;
	Node* destino = NULL;
	bool x = true;
	bool y = true;

	for (unsigned int i = 0; i != this->pontos.size(); i++) {

		if (this->pontos[i]->getId() == nodeIdInicio) {
			x = false;
			inicio = this->pontos[i];
		}

		if (this->pontos[i]->getId() == nodeIdDestino) {
			y = false;
			destino = this->pontos[i];
		}

	}

	if (x || y) {
		return false;
	}
	else
	{
	Estrada novainit = Estrada(id, inicio, destino);
	this->estradas.push_back(&novainit);
	inicio->addEstrada(novainit);
	Estrada novadest = Estrada(id, destino, inicio);
	this->estradas.push_back(&novadest);
	destino->addEstrada(novadest);

	return true;
	}

}
/**
 * @brief      Constructs the object.
 *
 * @param  cidade     	The city of the Map.
 */
Map::Map (string cidade) {
	cout << "É a criar isto?";
	GraphViewer *gv = new GraphViewer(500, 500,false);
	graphviewer = gv;
	gv->createWindow(500, 500);
	gv->defineVertexColor("blue");
	gv->defineEdgeColor("green");

	cout <<  "è";

	string edges = "/T08_edges_";
	string nodes = "/T08_nodes_X_Y_";

	string auxS = "./T08/";
	string auxS2 = ".txt";

	edges = auxS + cidade + edges + cidade + auxS2;
	nodes = auxS + cidade + nodes + cidade + auxS2;

	//Opening files
	ifstream fileNode;
	fileNode.open(nodes, ios::in);
	if (!fileNode.good()) {
		cout << "File node failed to open" << endl;
		return;
	}
	getline(fileNode,nodes);

	ifstream fileEdge;
	fileEdge.open(edges, ios::in);
	if (!fileEdge.good()) {
		cout << "File edge failed to open" << endl;
		return;
	}
	getline(fileEdge,edges);

	unsigned int menorX = -1;
	unsigned int menorY = -1;
	while (getline(fileNode,nodes)) {
		int id;
		float x;
		float y;
		sscanf (nodes.c_str(),"(%d,%f,%f)",&id, &x, &y);
		this->addNode(id,x, y, NONE);
		if (menorX > x)
			menorX = x;
		if (menorY > y)
			menorY = y;
	}

	for (unsigned int i = 0; i != pontos.size(); i++) {
		gv->addNode(pontos[i]->getId(), pontos[i]->getX()-menorX, pontos[i]->getY()-menorY);
	}


	int i = 0;
	while (getline(fileEdge,edges)) {

		int idNodeInit;
		int idNodeDest;

		sscanf (edges.c_str(),"(%d,%d)",&idNodeInit, &idNodeDest);

		if (this->addEstrada(i, idNodeInit, idNodeDest))
			gv->addEdge(i,idNodeInit,idNodeDest,0);
		i++;

	}

}
/**
 * @brief      Gets the points of the map.
 *
 * @return     The points of the map.
 */
std::vector<Node*> Map::getPontos() {
	return this->pontos;
}
/**
 * @brief      Gets the points of interest of the map.
 *
 * @return     The points of interest of the map.
 */
std::vector<Node*> Map::getInterece() {
	return this->interece;
}
/**
 * @brief      Gets the solution of the map.
 *
 * @return     The solution of the map.
 */
std::vector<Node*> Map::getSolucao() {
	return this->solucao;
}
/**
 * @brief      Gets the roads of the map.
 *
 * @return     The roads of the map.
 */
std::vector<Estrada*> Map::getEstradas() {
	return this->estradas;
}
/**
* @brief      Function to add a point of interest on the map.
*/
void Map::addPontoInterece (Node* node) {
	this->interece.push_back(node);
}
/**
* @brief      Function to remove a point of interest on the map.
*/
void Map::removePontoInterece (Node* node) {
	for (unsigned int i = 0; i != this->interece.size(); i++) {
		cout << interece[i]->getId() << endl;
		if (this->interece[i] == node) {
			this->interece.erase(this->interece.begin()+i);
		}
	}
}
/**
* @brief      Function that receives the starting point and makes the solution of the way.
*/
void Map::solution (Node* pontoInicial) {
	this->atualizaInterreceAtual();
	std::vector<Node*> solucaoTemporaria = {};
	Node* final = nullptr;
	double pesoMenor = INF;

	for (unsigned int i = 0; i != this->intereceAtual.size(); i++) {
		if (!this->intereceAtual[i]->isVisited()) {
			if (pesoMenor > this->dijkstra(pontoInicial, intereceAtual[i])){
				solucaoTemporaria = this->getCaminho(pontoInicial, intereceAtual[i]);
				final = intereceAtual[i];
			}
		}
	}
	if (final != nullptr) {
		final->setVisited(true);

		if (this->solucao.size() > 0 && solucaoTemporaria.size() > 0) {
			if (this->solucao.size() > 1)
				this->solucao.insert(this->solucao.end(), solucaoTemporaria.begin()+1, solucaoTemporaria.end());
				this->solution(final);
		}
		else if (this->solucao.size() == 0){
			this->solucao = solucaoTemporaria;
			this->solution(final);
		}
	}
	else {
		this->cleanMapColor();
		VETV* car = new VETV(ALL,new Node(0,0,0,ALL));
		car->setCaminho(solucao);
		this->iluminaSolucaoMapa(car);
	}


}
/**
 * @brief      Gets the destination weight with dijkstra algorithm.
 *
 * @return     The destination weight.
 */
double Map::dijkstra (Node* init, Node* dest) {
	this->inicializacaoDijkstra(init);
	MutablePriorityQueue<Node> q;
	q.insert(init);
	while ( ! q.empty() ) {
		Node* v = q.extractMin();
		for (Estrada e : v->getEstradas()) {
			double arrestaPeso = e.getDestino()->getPeso();
			if ( pesoMelhor(v, e.getDestino(), e.getPeso()))
			{
				if (arrestaPeso == INF)
					q.insert(e.getDestino());
				else
					q.decreaseKey(e.getDestino());
			}
		}
	}
	return dest->getPeso();
}
/**
 * @brief      Gets the best way to go from initial point to destination point.
 *
 * @return     the temporary solution.
 */
std::vector<Node*> Map::getCaminho(Node* init, Node* dest) {
	std::vector<Node*> solucaoTemporaria;
	if (dest->getPeso() == INF)
		return solucaoTemporaria;
	for ( ; dest != nullptr; dest = dest->getCaminho())
		solucaoTemporaria.push_back(dest);
	solucaoTemporaria.push_back(init);
	reverse(solucaoTemporaria.begin(), solucaoTemporaria.end());
	return solucaoTemporaria;
}
/**
 * @brief      Gets the information if the weight of the neighbor node is heavier than the weight of the edge plus the weight of the visited node.
 *
 * @return    If the weight of the neighbor node is heavier than the weight of the edge plus the weight of the visited node(true) or not(false);
 */
bool Map::pesoMelhor (Node* nodeVisitado, Node* nodeVizinho, double pesoArresta) {

	if (nodeVizinho->getPeso() > pesoArresta + nodeVisitado->getPeso()) {
		nodeVizinho->setPeso(pesoArresta + nodeVisitado->getPeso());
		nodeVizinho->setCaminho(nodeVisitado);
		return true;
	}
	else{

		return false;
	}
}
/**
* @brief      Function to initialize the Dijkstra algorithm.
*/
void Map::inicializacaoDijkstra(Node* pontoInicial) {
	for (Node* node : this->pontos) {
		node->setCaminho(nullptr);
		node->setPeso(INF);
	}

	pontoInicial->setPeso(0);

}
/**
* @brief      Function to paint the map solution.
*/
void Map::iluminaSolucaoMapa(VETV* carro) {
	for (unsigned int i = 0; i != carro->getCaminho().size(); i++)
	{
		if (carro->getTipo() == BANCOS)
		{
			if (carro->getCaminho()[i]->getTipo() == BANCOS)
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "BLUE");
			else
			{
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "CYAN");
			}
		}
		else if (carro->getTipo() == MUSEUS)
		{
			if (carro->getCaminho()[i]->getTipo() == MUSEUS)
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "YELLOW");
			else
			{
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "ORANGE");
			}
		}
		else if (carro->getTipo() == CORREIO_URGENTE)
		{
			if (carro->getCaminho()[i]->getTipo() == CORREIO_URGENTE)
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "RED");
			else
			{
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "PINK");
			}
		}
		else if (carro->getTipo() == JUNTAS)
		{
			if (carro->getCaminho()[i]->getTipo() == JUNTAS)
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "DARK_GREY");
			else
			{
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "LIGHT_GREY");
			}
		}
		else
		{
			if (carro->getCaminho()[i]->getTipo() == BANCOS || this->solucao[i]->getTipo() == MUSEUS || this->solucao[i]->getTipo() == CORREIO_URGENTE || this->solucao[i]->getTipo() == JUNTAS)
			{
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "BLACK");
			}
			else
			{
				this->graphviewer->setVertexColor(carro->getCaminho()[i]->getId(), "WHITE");
			}
		}
	}
}
/**
* @brief      Function to exit the window.
*/
void Map::exit() {
	this->graphviewer->closeWindow();
}
/**
 * @brief      Gets the cars.
 *
 * @return     The cars.
 */
std::vector<VETV* > Map::getCarros() {
	return this->carros;
}
/**
* @brief      Function to add a car.
*/
void Map::addCar(VETV* carro) {
	this->carros.push_back(carro);
}
/**
* @brief      Function to make the shortest way for all available cars.
*/
void Map::solution2() {
	this->clearVisitado();
	this->cleanMapColor();

	this->atualizaInterreceAtual();

	std::vector<Node* > vectorVazio = {};
	std::vector<Node* > solucaoTemporaria = {};
	std::vector<Node* > solucaoTemporariaFinal = {};
	VETV* final = nullptr;
	double pesoMenor = INF;


	for (unsigned int i = 0; i != this->intereceAtual.size(); i++) {
		if (!this->intereceAtual[i]->isVisited()) {
			for (unsigned int j = 0; j != carros.size(); j++) {
				if (carros[j]->getTipo() == this->intereceAtual[i]->getTipo() || carros[j]->getTipo() == ALL) {
					int pesoDijkstra;
					if (carros[j]->getCaminho().size() == 0)
						pesoDijkstra = this->dijkstra(carros[j]->getPontoInicail(), intereceAtual[i]);
					else
						pesoDijkstra = this->dijkstra(carros[j]->getCaminho()[carros[j]->getCaminho().size()-1], intereceAtual[i]);

					if (pesoMenor > pesoDijkstra){
						solucaoTemporaria = this->getCaminho(carros[j]->getPontoInicail(), intereceAtual[i]);
						if (final == nullptr) {
							final = carros[j];
							solucaoTemporariaFinal = solucaoTemporaria;
						}
						else if ((final->getCaminho()[final->getCaminho().size()-1]->getPeso() + solucaoTemporariaFinal[solucaoTemporariaFinal.size()-1]->getPeso())
								> (carros[j]->getCaminho()[carros[j]->getCaminho().size()-1]->getPeso() + pesoDijkstra)) {
							final = carros[j];
							solucaoTemporariaFinal = solucaoTemporaria;
						}
					}
				}
			}
		}
		if (final != nullptr) {
			//Apend do caminho ao final
			this->intereceAtual[i]->setVisited(true);
			solucaoTemporaria = final->getCaminho();
			solucaoTemporaria.insert(solucaoTemporaria.end(), solucaoTemporariaFinal.begin()+1, solucaoTemporariaFinal.end());
			final->setCaminho(solucaoTemporaria);
			final = nullptr;
		}
		this->atualizaInterreceAtual();
	}

	//CORES//
	for (unsigned int j = 0; j != carros.size(); j++) {
		this->iluminaSolucaoMapa(carros[j]);
	}

}
/**
* @brief      Function to clear the map color.
*/
void Map::cleanMapColor() {
	for (unsigned int i = 0; i != this->pontos.size(); i++) {
		this->graphviewer->setVertexColor(this->pontos[i]->getId(), "blue");
	}
}

/**
* @brief      Function to clear visited.
*/
void Map::clearVisitado() {
	for (unsigned int i = 0; i != this->interece.size(); i++) {
		this->interece[i]->setVisited(false);
	}
	for (unsigned int i = 0; i != this->dropPoint.size(); i++) {
		this->dropPoint[i]->setVisited(false);
		this->collectionPoint[i]->setVisited(false);
	}
}
/**
 * @brief      Gets map collection points.
 *
 * @return     The map collection points.
 */
std::vector<Node* > Map::getCollectionPoint() {
	return this->collectionPoint;
}
/**
 * @brief      Gets map drop points.
 *
 * @return     The map drop points.
 */
std::vector<Node* > Map::getDropPoint() {
	return this->dropPoint;
}
/**
* @brief      Function to add a collection point to the map.
*/
void Map::addPontoCollection (Node* node) {
	this->collectionPoint.push_back(node);
}
/**
* @brief      Function to add a collection point to the map.
*/
void Map::addPontoDrop (Node* node) {
	this->dropPoint.push_back(node);
}
/**
* @brief      Function to update the current point of interest.
*/
void Map::atualizaInterreceAtual() {
	this->intereceAtual = this->interece;
	for (unsigned int i = 0; i != this->dropPoint.size(); i++)
		this->intereceAtual.push_back(this->collectionPoint[i]);

	for (unsigned int i = 0; i != this->dropPoint.size(); i++) {
		if (this->collectionPoint[i]->isVisited())
			this->intereceAtual.push_back(this->dropPoint[i]);
	}
}
/**
* @brief      Function to clear the way of cars.
*/
void Map::clarCarrosCaminhos() {
	std::vector<Node* > vectorVazio = {};
	for (unsigned int i = 0; i != carros.size(); i++) {
		this->carros[i]->setCaminho(vectorVazio);
	}
}
