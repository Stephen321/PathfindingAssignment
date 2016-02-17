#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <queue>

using namespace std;

template <class DataType, class NodeType, class ArcType> class GraphArc;
template <class DataType, class NodeType, class ArcType> class GraphNode;

// ----------------------------------------------------------------
//  Name:           Graph
//  Description:    This is the graph class, it contains all the
//                  nodes.
// ----------------------------------------------------------------
template<class DataType, class NodeType, class ArcType>
class Graph {
private:

    // typedef the classes to make our lives easier.
    typedef GraphArc<DataType, NodeType, ArcType> Arc;
	typedef GraphNode<DataType, NodeType, ArcType> Node;

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
			NodeType f1 = n1->hCost() + n1->gCost();
			NodeType f2 = n2->hCost() + n2->gCost();
			// adds H(n) and G(n) to get F(n)
			return f1 > f2;
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
	bool addNode(DataType data, int index, sf::Vector2f position);
    void removeNode( int index );
    bool addArc( int from, int to, ArcType weight, bool directed = true );
    void removeArc( int from, int to );
	Arc* getArc(int from, int to);
	void reset();
	int getMaxNodes();

	//Pathfinding Assignment
	void aStar(Node* pStart, Node* pDest, std::vector<Node *>& path);
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
template<class DataType, class NodeType, class ArcType>
Graph<DataType, NodeType, ArcType>::Graph(int size) : m_maxNodes(size) {
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
template<class DataType, class NodeType, class ArcType>
Graph<DataType, NodeType, ArcType>::~Graph() {
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
template<class DataType, class NodeType, class ArcType>
bool Graph<DataType, NodeType, ArcType>::addNode(DataType data, int index, sf::Vector2f position) {
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
template<class DataType, class NodeType, class ArcType>
void Graph<DataType, NodeType, ArcType>::removeNode(int index) {
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
template<class DataType, class NodeType, class ArcType>
bool Graph<DataType, NodeType, ArcType>::addArc(int from, int to, ArcType weight, bool directed) {
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
template<class DataType, class NodeType, class ArcType>
void Graph<DataType, NodeType, ArcType>::removeArc(int from, int to) {
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
template<class DataType, class NodeType, class ArcType>
// Dev-CPP doesn't like Arc* as the (typedef'd) return type?
GraphArc<DataType, NodeType, ArcType>* Graph<DataType, NodeType, ArcType>::getArc(int from, int to) {
     Arc* pArc = 0;
     // make sure the to and from nodes exist
     if( m_pNodes[from] != 0 && m_pNodes[to] != 0 ) {
         pArc = m_pNodes[from]->getArc( m_pNodes[to] );
     }
                
     return pArc;
}

// ----------------------------------------------------------------
//  Name:           getMaxNodes
//  Description:    Gets the number of elements in the list of nodes.
//  Arguments:      None
//  Return Value:   m_maxNodes
// ----------------------------------------------------------------
template<class DataType, class NodeType, class ArcType>
int Graph<DataType, NodeType, ArcType>::getMaxNodes() {

	return m_maxNodes;
}



// ----------------------------------------------------------------
//  Name:           clearMarks
//  Description:    This clears every mark on every node.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class DataType, class NodeType, class ArcType>
void Graph<DataType, NodeType, ArcType>::reset() {
	int index;
	for (index = 0; index < m_maxNodes; index++) {
		if (m_pNodes[index] != 0) {
			m_pNodes[index]->reset();
		}
	}
}

template<class DataType, class NodeType, class ArcType>
void Graph<DataType, NodeType, ArcType>::aStar(Node* pStart, Node* pDest, std::vector<Node *>& path){
	if (pStart != 0 && pDest != 0) {
		priority_queue<Node*, vector<Node*>, NodeSearchCostComparer> pq;
		pq.push(pStart);
		pStart->setMarked(true);
		pStart->setHCost(NodeType());
		pStart->setGCost(NodeType());

		while (pq.size() != 0 && pq.top() != pDest) {
			list<Arc>::const_iterator iter = pq.top()->arcList().begin();
			list<Arc>::const_iterator endIter = pq.top()->arcList().end();

			for (; iter != endIter; iter++) {
				Node* child = (*iter).node();
				if (child != pq.top()->getPrevious()){
					Arc arc = (*iter);
					//Sleep(1000);
					int Hc = child->hCost();
					int Gc = pq.top()->gCost() + arc.weight();
					int Fc = Hc + Gc;
					if (Fc < child->fCost() || child->gCost() == -1){  //is G(n) not set, H(n) should be always set with setHeuristics()
						child->setHCost(Hc);
						child->setGCost(Gc);
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



template<class DataType, class NodeType, class ArcType>
void Graph<DataType, NodeType, ArcType>::setHeuristics(Node* pDest){
	if (pDest != 0) {
		for (int i = 0; i < m_count; i++){
			sf::Vector2f vectorTo = pDest->getPosition() - m_pNodes[i]->getPosition();
			int Hc = (int)(sqrt((vectorTo.x * vectorTo.x) + (vectorTo.y * vectorTo.y)));
			m_pNodes[i]->setHCost(Hc);
		}
	}
}



//draw the nodes
template<class DataType, class NodeType, class ArcType>
void Graph<DataType, NodeType, ArcType>::drawNodes(sf::RenderTarget& target) const{
	for (int i = 0; i < m_count; i++){
		target.draw(m_pNodes[i]->getShape());
		m_pNodes[i]->drawText(target);
	}
}

//draw arcs
template<class DataType, class NodeType, class ArcType>
void Graph<DataType, NodeType, ArcType>::drawArcs(sf::RenderTarget& target) const{
	for (int i = 0; i < m_count; i++)
		m_pNodes[i]->drawArcs(target);
}



#include "GraphNode.h"
#include "GraphArc.h"


#endif
