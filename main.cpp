//----------------------------------------------------------------------------------------------
//I declare that the work I am submitting for assessing by the Institute examiner(s) is entirely my own
//work, save for any portion(s) thereof where the author or source has been duly referenced and
//attributed.
//Signed: Stephen Ennis C00181305
//----------------------------------------------------------------------------------------------

#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"glu32.lib") 

#include "SFML/Graphics.hpp" 
#include "SFML/Graphics/Shader.hpp"
#include "SFML/OpenGL.hpp" 

#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include "Graph.h"

using namespace std;


//typedef GraphArc<tuple<string, int, int>, int> Arc;
typedef GraphNode<tuple<string, int, int>, int> Node;

void visit(Node * pNode) {
	cout << "Visiting: Data= " << get<0>(pNode->data()) << " H(n)= " << get<1>(pNode->data()) << " G(n)= " << get<2>(pNode->data()) << endl;
}

int main(int argc, char *argv[]) {

	//create window 
	int width = 1200, height = 800;
	sf::RenderWindow App(sf::VideoMode(width, height, 32), "SFML PathfindingAssement");

	//set up graph
	const int NODE_COUNT = 30;
	Graph<tuple<string, int, int>, int> graph(NODE_COUNT);
	
	//set up nodes
	string n;
	int i = 0;
	ifstream myfile;
	myfile.open ("nodes.txt");

	sf::Vector2f graphPosition(100, 100);
	sf::Vector2f offset;
	while (myfile >> n >> offset.x >> offset.y) {
		graph.addNode(tuple<string, int, int>(n, numeric_limits<int>::max() / 2, numeric_limits<int>::max() / 2), i++, graphPosition + offset);
	}
	myfile.close();


	//set up arcs
	myfile.open("arcs.txt");
	//if first line of text file says undirected then make the graph undirected
	string dirText;
	myfile >> dirText;
	bool directed = true;
	if (dirText == "undirected")
		directed = false;
	else{
		myfile.clear();//clear buffer
		myfile.seekg(0, ios::beg); //reset back to start of file
	}
	int from, to, weight;
	while ( myfile >> from >> to >> weight ){
		graph.addArc(from, to, weight, directed);
	}
    myfile.close();


	int startNode = 0;
	int endNode = 20;

	//reset graph
	vector<Node*> path;
	graph.clearMarks();
	graph.clearPrevious();
	path.clear();
	//							  1					    v
	graph.aStar(graph.nodeArray()[0], graph.nodeArray()[20], visit, path);
	graph.nodeArray()[startNode]->setColour(sf::Color::Green);
	graph.nodeArray()[endNode]->setColour(sf::Color::Red);

	//display nodes in path
	for (Node* n : path)
		visit(n);


	// Start game loop 
	while (App.isOpen())
	{
		// Process events 
		sf::Event Event;
		while (App.pollEvent(Event))
		{
			if (Event.type == sf::Event::Closed)
				App.close();
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
				App.close();
		}



		App.clear();
		graph.drawArcs(App);
		graph.drawNodes(App);
		App.display();
	}

	return EXIT_SUCCESS;
}
