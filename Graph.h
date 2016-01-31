#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <queue>

using namespace std;

template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    This is the graph class, it contains all the
//                  nodes.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
class Graph {
private:

    // typedef the classes to make our lives easier.
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;

// ----------------------------------------------------------------
//  Description:    An array of all the nodes in the graph.
// ----------------------------------------------------------------
    Node** m_pNodes;

// ----------------------------------------------------------------
//  Description:    The maximum number of nodes in the graph.
// ----------------------------------------------------------------
    int m_maxNodes;


// ----------------------------------------------------------------
//  Description:    The actual number of nodes in the graph.
// ----------------------------------------------------------------
    int m_count;

	class NodeSearchCostComparer {
	public:
		bool operator()(Node * n1, Node * n2) {
			NodeType p1 = n1->data();
			NodeType p2 = n2->data();
			// adds H(n) and G(n) to get F(n)
			return (get<1>(p1) + get<2>(p1)) > (get<1>(p2) +get<2>(p2));
		}
	};

	sf::Font font;

public:           
    // Constructor and destructor functions
    Graph( int size );
    ~Graph();

    // Accessors
    Node** nodeArray() const {
       return m_pNodes;
    }

    // Public member functions.
	bool addNode(NodeType data, int index, sf::Vector2f position);
    void removeNode( int index );
    bool addArc( int from, int to, ArcType weight, bool directed = true );
    void removeArc( int from, int to );
	Arc* getArc(int from, int to);
	void clearMarks();
	void clearPrevious();
	void depthFirst(Node* pNode, void(*pProcess)(Node*));
	void breadthFirst(Node* pNode, void(*pProcess)(Node*));
	void adaptedBreadthFirst(Node* pStart, Node* pDest, std::vector<Node *>& path);
	void ucs(Node* pStart, Node* pDest, void(*pVisitFunc)(Node*), std::vector<Node *>& path);

	//Pathfinding Assignment
	void aStar(Node* pStart, Node* pDest, void(*pProcess)(Node*), std::vector<Node *>& path);
	void setHeuristics(Node* pDest);
	void drawNodes(sf::RenderTarget& target) const;
	void drawArcs(sf::RenderTarget& target) const;

};

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    Constructor, this constructs an empty graph
//  Arguments:      The maximum number of nodes.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::Graph( int size ) : m_maxNodes( size ) {
   int i;
   m_pNodes = new Node * [m_maxNodes];
   // go through every index and clear it to null (0)
   for( i = 0; i < m_maxNodes; i++ ) {
        m_pNodes[i] = 0;
   }

   // set the node count to 0.
   m_count = 0;

   font.loadFromFile("C:\\Windows\\Fonts\\GARA.TTF");
}

// ----------------------------------------------------------------
//  Name:           ~Graph
//  Description:    destructor, This deletes every node
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::~Graph() {
   int index;
   for( index = 0; index < m_maxNodes; index++ ) {
        if( m_pNodes[index] != 0 ) {
            delete m_pNodes[index];
        }
   }
   // Delete the actual array
   delete m_pNodes;
}

// ----------------------------------------------------------------
//  Name:           addNode
//  Description:    This adds a node at a given index in the graph.
//  Arguments:      The first parameter is the data to store in the node.
//                  The second parameter is the index to store the node.
//  Return Value:   true if successful
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addNode( NodeType data, int index, sf::Vector2f position ) {
   bool nodeNotPresent = false;
   // find out if a node does not exist at that index.
   if ( m_pNodes[index] == 0) {
      nodeNotPresent = true;
      // create a new node, put the data in it, and unmark it.
      m_pNodes[index] = new Node(font);
      m_pNodes[index]->setData(data);
      m_pNodes[index]->setMarked(false);
	  m_pNodes[index]->setPosition(position);
	  m_pNodes[index]->setColour(sf::Color::Blue);

      // increase the count and return success.
      m_count++;
    }
        
    return nodeNotPresent;
}

// ----------------------------------------------------------------
//  Name:           removeNode
//  Description:    This removes a node from the graph
//  Arguments:      The index of the node to return.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeNode( int index ) {
     // Only proceed if node does exist.
     if( m_pNodes[index] != 0 ) {
         // now find every arc that points to the node that
         // is being removed and remove it.
         int node;
         Arc* arc;

         // loop through every node
         for( node = 0; node < m_maxNodes; node++ ) {
              // if the node is valid...
              if( m_pNodes[node] != 0 ) {
                  // see if the node has an arc pointing to the current node.
                  arc = m_pNodes[node]->getArc( m_pNodes[index] );
              }
              // if it has an arc pointing to the current node, then
              // remove the arc.
              if( arc != 0 ) {
                  removeArc( node, index );
              }
         }
        

        // now that every arc pointing to the current node has been removed,
        // the node can be deleted.
        delete m_pNodes[index];
        m_pNodes[index] = 0;
        m_count--;
    }
}

// ----------------------------------------------------------------
//  Name:           addArd
//  Description:    Adds an arc from the first index to the 
//                  second index with the specified weight.
//  Arguments:      The first argument is the originating node index
//                  The second argument is the ending node index
//                  The third argument is the weight of the arc
//  Return Value:   true on success.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addArc( int from, int to, ArcType weight, bool directed ) {
     bool proceed = true; 
     // make sure both nodes exist.
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         proceed = false;
     }
        
     // if an arc already exists we should not proceed
     if( m_pNodes[from]->getArc( m_pNodes[to] ) != 0 ) {
         proceed = false;
     }

     if (proceed == true) {
        // add the arc to the "from" node.
		 m_pNodes[from]->addArc(m_pNodes[to], weight);
		 if (directed == false) //add node back the other way if undirected
			 m_pNodes[to]->addArc(m_pNodes[from], weight);
     }
        
     return proceed;
}

// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This removes the arc from the first index to the second index
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeArc( int from, int to ) {
     // Make sure that the node exists before trying to remove
     // an arc from it.
     bool nodeExists = true;
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         nodeExists = false;
     }

     if (nodeExists == true) {
        // remove the arc.
        m_pNodes[from]->removeArc( m_pNodes[to] );
     }
}


// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    Gets a pointer to an arc from the first index
//                  to the second index.
//  Arguments:      The first parameter is the originating node index.
//                  The second parameter is the ending node index.
//  Return Value:   pointer to the arc, or 0 if it doesn't exist.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
// Dev-CPP doesn't like Arc* as the (typedef'd) return type?
GraphArc<NodeType, ArcType>* Graph<NodeType, ArcType>::getArc( int from, int to ) {
     Arc* pArc = 0;
     // make sure the to and from nodes exist
     if( m_pNodes[from] != 0 && m_pNodes[to] != 0 ) {
         pArc = m_pNodes[from]->getArc( m_pNodes[to] );
     }
                
     return pArc;
}


// ----------------------------------------------------------------
//  Name:           clearMarks
//  Description:    This clears every mark on every node.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::clearMarks() {
	int index;
	for (index = 0; index < m_maxNodes; index++) {
		if (m_pNodes[index] != 0) {
			m_pNodes[index]->setMarked(false);
		}
	}
}
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::clearPrevious() {
	int index;
	for (index = 0; index < m_maxNodes; index++) {
		if (m_pNodes[index] != 0) {
			m_pNodes[index]->setPrevious(0);
		}
	}
}


// ----------------------------------------------------------------
//  Name:           depthFirst
//  Description:    Performs a depth-first traversal on the specified 
//                  node.
//  Arguments:      The first argument is the starting node
//                  The second argument is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::depthFirst( Node* pNode, void (*pProcess)(Node*) ) {
     if( pNode != 0 ) {
           // process the current node and mark it
           pProcess( pNode );
           pNode->setMarked(true);

           // go through each connecting node
           list<Arc>::iterator iter = pNode->arcList().begin();
           list<Arc>::iterator endIter = pNode->arcList().end();
        
		   for( ; iter != endIter; ++iter) {
			    // process the linked node if it isn't already marked.
                if ( (*iter).node()->marked() == false ) {
                   depthFirst( (*iter).node(), pProcess);
                }            
           }
     }
}


// ----------------------------------------------------------------
//  Name:           breadthFirst
//  Description:    Performs a depth-first traversal the starting node
//                  specified as an input parameter.
//  Arguments:      The first parameter is the starting node
//                  The second parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::breadthFirst(Node* pNode, void(*pProcess)(Node*)) {
	if (pNode != 0) {
		queue<Node*> nodeQueue;
		// place the first node on the queue, and mark it.
		nodeQueue.push(pNode);
		pNode->setMarked(true);

		// loop through the queue while there are nodes in it.
		while (nodeQueue.size() != 0) {
			// process the node at the front of the queue.
			pProcess(nodeQueue.front());

			// add all of the child nodes that have not been 
			// marked into the queue
			list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
			list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();

			for (; iter != endIter; iter++) {
				if ((*iter).node()->marked() == false) {
					// mark the node and add it to the queue.
					(*iter).node()->setMarked(true);
					nodeQueue.push((*iter).node());
				}
			}

			// dequeue the current node.
			nodeQueue.pop();
		}
	}
}


template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::adaptedBreadthFirst(Node* pStart, Node* pDest, vector<Node *>& path) {
	if (pStart != 0 && pDest != 0) {
		queue<Node*> nodeQueue;
		nodeQueue.push(pStart);
		pStart->setMarked(true);
		bool goalReached = false;

		while (nodeQueue.size() != 0 && goalReached == false) {
			//pProcess(nodeQueue.front());
			list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
			list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();

			for (; iter != endIter && goalReached == false; iter++) {
				Node* currNode = (*iter).node();
				if (currNode->marked() == false) {
					currNode->setMarked(true);
					currNode->setPrevious(nodeQueue.front());

					if (currNode != pDest){ //we havnt reached the goal
						nodeQueue.push(currNode);
					}
					else{//we have reached the goal
						goalReached = true;
						Node* previous = currNode; //finaly node/goal
						for (; previous->getPrevious() != 0; previous = previous->getPrevious()){
							path.push_back(previous); //add each previous node to the path
						}
						path.push_back(pStart); //first node in path should be the start node so add it to the end
						std::reverse(path.begin(), path.end()); //reverse the path so it goes from start to goal
					}
				}
			}
			nodeQueue.pop();
		}
	}
}


template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::ucs(Node* pStart, Node* pDest, void(*pVisitFunc)(Node*), std::vector<Node *>& path){
	if (pStart != 0 && pDest != 0) {
		priority_queue<Node*, vector<Node*>, NodeSearchCostComparer> pq;
		pq.push(pStart);
		pStart->setMarked(true);
		pStart->setData(NodeType(pStart->data().first, 0));

		while (pq.size() != 0 && pq.top() != pDest) {
			list<Arc>::const_iterator iter = pq.top()->arcList().begin();
			list<Arc>::const_iterator endIter = pq.top()->arcList().end();

			for (; iter != endIter; iter++) {
				Node* child = (*iter).node();
				Arc arc = (*iter);
				if (child != pq.top()->getPrevious()){
					int distC = pq.top()->data().second + arc.weight();
					if (distC < child->data().second){
						child->setData(NodeType(child->data().first, distC));
						child->setPrevious(pq.top());
					}
					if (child->marked() == false) {
						pq.push(child);
						child->setMarked(true);
					}
				}
			}
			pq.pop();
		}

		if (pq.top() == pDest){
			Node* previous = pDest;
			for (; previous->getPrevious() != 0; previous = previous->getPrevious()){
				path.push_back(previous);
			}
			path.push_back(pStart);
			std::reverse(path.begin(), path.end());
		}
	}
}


template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::aStar(Node* pStart, Node* pDest, void(*pProcess)(Node*), std::vector<Node *>& path){
	if (pStart != 0 && pDest != 0) {
		priority_queue<Node*, vector<Node*>, NodeSearchCostComparer> pq;
		pq.push(pStart);
		pStart->setMarked(true);
		pStart->setData(NodeType(get<0>(pStart->data()), get<1>(pStart->data()), 0));

		while (pq.size() != 0 && pq.top() != pDest) {
			list<Arc>::const_iterator iter = pq.top()->arcList().begin();
			list<Arc>::const_iterator endIter = pq.top()->arcList().end();

			for (; iter != endIter; iter++) {
				Node* child = (*iter).node();
				if (child != pq.top()->getPrevious()){
					Arc arc = (*iter);
					NodeType data = child->data();
					//Sleep(1000);
					int Hc = get<1>(data);
					int Gc = get<2>(pq.top()->data()) + arc.weight();
					int Fc = Hc + Gc;
					int currentFc = get<1>(data) +get<2>(data);
					if (Fc < currentFc  || get<2>(data) == -1){  //is G(n) not set, H(n) will be set with setHeuristics()
						child->setData(NodeType(get<0>(data), Hc, Gc));
						child->setPrevious(pq.top());
					}

					if (child->marked() == false) {
						pq.push(child);
						child->setMarked(true);
						child->setColour(sf::Color(0, 128, 128, 255));
					}
				}
			}
			pq.pop();
		}

		if (pq.size() != 0 && pq.top() == pDest){
			for (Node* previous = pDest; previous->getPrevious() != 0; previous = previous->getPrevious()){
				path.push_back(previous);
				previous->setColour(sf::Color::Magenta);
			}
			path.push_back(pStart);
			std::reverse(path.begin(), path.end());
		}
		else if (pq.size() == 0)
			cout << "Couldn't find path." << endl;
	}
}



template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::setHeuristics(Node* pDest){
	if (pDest != 0) {
		for (int i = 0; i < m_count; i++){
			sf::Vector2f vectorTo = pDest->getPosition() - m_pNodes[i]->getPosition();
			int Hc = (int)(sqrt((vectorTo.x * vectorTo.x) + (vectorTo.y * vectorTo.y)));
			NodeType data = m_pNodes[i]->data();
			m_pNodes[i]->setData(NodeType(get<0>(data), Hc, get<2>(data)));
		}
	}
}



//draw the nodes
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::drawNodes(sf::RenderTarget& target) const{
	for (int i = 0; i < m_count; i++){
		target.draw(m_pNodes[i]->getShape());
		m_pNodes[i]->drawText(target);
	}
}

//draw arcs
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::drawArcs(sf::RenderTarget& target) const{
	for (int i = 0; i < m_count; i++)
		m_pNodes[i]->drawArcs(target);
}



#include "GraphNode.h"
#include "GraphArc.h"


#endif
