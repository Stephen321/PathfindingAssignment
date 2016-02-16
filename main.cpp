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
typedef GraphNode<string, int, int> Node;

void visit(Node * pNode) {
	cout << "Visiting: Data= " << pNode->data() << " H(n)= " << pNode->hCost() << " G(n)= " << pNode->gCost() << endl;
}

int main(int argc, char *argv[]) {

	//create window 
	int width = 1200, height = 800;
	sf::RenderWindow App(sf::VideoMode(width, height, 32), "SFML PathfindingAssement");

	//arc weight text
	typedef struct _WeightDisplay{
		sf::Text text;
		sf::RectangleShape rectangle;
	}WeightDisplay;
	vector<WeightDisplay> weightTexts;
	sf::Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\GARA.TTF");

	//set up graph
	const int NODE_COUNT = 30;
	Graph<string, int, int> graph(NODE_COUNT);
	
	//set up nodes
	string n;
	int i = 0;
	ifstream myfile;
	myfile.open ("nodes.txt");

	sf::Vector2f graphPosition(100, 100);
	sf::Vector2f offset;
	while (myfile >> n >> offset.x >> offset.y) {
		graph.addNode(n, i++, graphPosition + offset);
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

		//add weight text
		WeightDisplay wgtDisplay;
		wgtDisplay.text = sf::Text(to_string(weight), font, 15);
		wgtDisplay.text.setPosition(graph.nodeArray()[from]->getPosition() + ((graph.nodeArray()[to]->getPosition() - graph.nodeArray()[from]->getPosition()) / 2.f));
		wgtDisplay.text.setColor(sf::Color(255, 255, 128, 255));
		wgtDisplay.text.setOrigin(8, 8);

		//add weight back panel
		wgtDisplay.rectangle = sf::RectangleShape(sf::Vector2f(22, 14));
		wgtDisplay.rectangle.setOrigin(8, 5);
		wgtDisplay.rectangle.setFillColor(sf::Color::Black);
		wgtDisplay.rectangle.setOutlineColor(sf::Color(128, 128, 128, 255));
		wgtDisplay.rectangle.setOutlineThickness(1.f);
		wgtDisplay.rectangle.setPosition(wgtDisplay.text.getPosition());

		weightTexts.push_back(wgtDisplay);
	}
    myfile.close();


	int startNode = 0;
	int endNode = 17;
	//15,22,26 breaks when starting at 0
	//s, x, g

	//path
	vector<Node*> path;

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
			if (Event.type == sf::Event::MouseButtonReleased){
				graph.aStar(graph.nodeArray()[startNode], graph.nodeArray()[endNode], path);
				graph.nodeArray()[startNode]->setColour(sf::Color::Green);
				graph.nodeArray()[endNode]->setColour(sf::Color::Red);
			}
			if (Event.type == sf::Event::MouseButtonPressed){
				graph.clearMarks();
				graph.clearPrevious();
				path.clear();
				graph.setHeuristics(graph.nodeArray()[endNode]);
				graph.nodeArray()[startNode]->setColour(sf::Color::Green);
				graph.nodeArray()[endNode]->setColour(sf::Color::Red);
			}
		}



		App.clear();
		graph.drawArcs(App);
		graph.drawNodes(App);
		for (WeightDisplay wD : weightTexts){
			App.draw(wD.rectangle);
			App.draw(wD.text);
		}
		App.display();
	}

	return EXIT_SUCCESS;
}
