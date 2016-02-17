#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <list>
#include "SFML\Graphics.hpp"

// Forward references
template <typename DataType, typename NodeType, typename ArcType> class GraphArc;

// -------------------------------------------------------
// Name:        GraphNode
// Description: This is the node class. The node class 
//              contains data, and has a linked list of 
//              arcs.
// -------------------------------------------------------
template<class DataType, class NodeType, class ArcType>
class GraphNode {
private:    
// typedef the classes to make our lives easier.
	typedef GraphArc<DataType, NodeType, ArcType> Arc;
	typedef GraphNode<DataType, NodeType, ArcType> Node;
// -------------------------------------------------------
// Description: data inside the node
// -------------------------------------------------------
	DataType m_data;
// -------------------------------------------------------
// Description: cost inside node
// -------------------------------------------------------
	NodeType m_hCost;
	NodeType m_gCost;
// -------------------------------------------------------
// Description: list of arcs that the node has.
// -------------------------------------------------------
    list<Arc> m_arcList;

// -------------------------------------------------------
// Description: This remembers if the node is marked.
// -------------------------------------------------------
    bool m_marked;

	Node* m_prevNode; 

	sf::CircleShape m_shape;

	sf::Text m_nameTxt;
	sf::Text m_gCostTxt;
	sf::Text m_hCostTxt;

	const int RADIUS = 25;

public:
    // Accessor functions
    list<Arc> const & arcList() const {
        return m_arcList;              
    }

    bool marked() const {
        return m_marked;
    }

    DataType const & data() const {
        return m_data;
    }
    // Manipulator functions
	void setData(DataType data) {
		m_data = data;
		m_nameTxt.setString(m_data);
    }

	void setHCost(NodeType hCost) {
		m_hCost = hCost;
		m_hCostTxt.setString("H(n)= " + to_string(m_hCost));
	}

	void setGCost(NodeType gCost) {
		m_gCost = gCost;
		m_gCostTxt.setString("G(n)= " + to_string(m_gCost));
	}

	NodeType const & hCost() const {
		return m_hCost;
	}

	NodeType const & gCost() const {
		return m_gCost;
	}

	NodeType const & fCost() const {
		return m_hCost + m_gCost;
	}

	void setMarked(bool mark) {
		m_marked = mark;
	}

	void setPrevious(Node* previous) {
		m_prevNode = previous;
	}

	void setColour(sf::Color newColour) {
		m_shape.setFillColor(newColour);
	}

	void setPosition(sf::Vector2f newPosition){
		m_nameTxt.setPosition(newPosition);
		m_hCostTxt.setPosition(newPosition.x - RADIUS, newPosition.y - RADIUS);
		m_gCostTxt.setPosition(newPosition.x - RADIUS, newPosition.y - (RADIUS / 2));
		m_shape.setPosition(newPosition);
	}

	sf::Vector2f getPosition(){
		return m_shape.getPosition();
	}

	sf::CircleShape const & getShape() const{
		return m_shape;
	}

	void reset();

	Node* getPrevious() {
		return m_prevNode;
	}
           
    Arc* getArc( Node* pNode );    
    void addArc( Node* pNode, ArcType pWeight );
	void removeArc(Node* pNode);
	void drawArcs(sf::RenderTarget& target) const;
	void drawText(sf::RenderTarget& target) const;
	GraphNode(sf::Font& font);
};


template<typename DataType, typename NodeType, typename ArcType>
GraphNode<DataType, NodeType, ArcType>::GraphNode(sf::Font& font) :
m_prevNode(0),
m_hCost(-1),
m_gCost(-1){
	m_shape = sf::CircleShape(RADIUS);
	m_shape.setOrigin(RADIUS, RADIUS);

	m_nameTxt = sf::Text("", font, 15);
	m_nameTxt.setOrigin(4, 8);
	m_hCostTxt = sf::Text("H(n)= ?", font, 11);
	m_hCostTxt.setOrigin(7, 7);
	m_gCostTxt = sf::Text("G(n)= ?", font, 11);
	m_gCostTxt.setOrigin(7, 7);
}

// ----------------------------------------------------------------
//  Name:           getArc
//  Description:    This finds the arc in the current node that
//                  points to the node in the parameter.
//  Arguments:      The node that the arc connects to.
//  Return Value:   A pointer to the arc, or 0 if an arc doesn't
//                  exist from this to the specified input node.
// ----------------------------------------------------------------

template<typename DataType, typename NodeType, typename ArcType>
GraphArc<DataType, NodeType, ArcType>* GraphNode<DataType, NodeType, ArcType>::getArc(Node* pNode) {

     list<Arc>::iterator iter = m_arcList.begin();
     list<Arc>::iterator endIter = m_arcList.end();
     Arc* pArc = 0;
     
     // find the arc that matches the node
     for( ; iter != endIter && pArc == 0; ++iter ) {         
          if ( (*iter).node() == pNode) {
               pArc = &( (*iter) );
          }
     }

     // returns null if not found
     return pArc;
}

template<typename DataType, typename NodeType, typename ArcType>
void  GraphNode<DataType, NodeType, ArcType>::reset() {
	m_marked = false;
	m_prevNode = 0;
	m_shape.setFillColor(sf::Color::Blue);
	m_hCost = -1;
	m_gCost = -1;

	m_hCostTxt.setString("H(n)= ?");
	m_gCostTxt.setString("G(n)= ?");
}
// ----------------------------------------------------------------
//  Name:           addArc
//  Description:    This adds an arc from the current node pointing
//                  to the first parameter, with the second parameter 
//                  as the weight.
//  Arguments:      First argument is the node to connect the arc to.
//                  Second argument is the weight of the arc.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename DataType, typename NodeType, typename ArcType>
void GraphNode<DataType, NodeType, ArcType>::addArc(Node* pNode, ArcType weight) {
   // Create a new arc.
   Arc a;
   a.setNode(pNode);
   a.setWeight(weight);
   a.setLine(m_shape.getPosition(), pNode->getPosition());
   // Add it to the arc list.
   m_arcList.push_back( a );
}


// ----------------------------------------------------------------
//  Name:           removeArc
//  Description:    This finds an arc from this node to input node 
//                  and removes it.
//  Arguments:      None.
//  Return Value:   None.
// ----------------------------------------------------------------
template<typename DataType, typename NodeType, typename ArcType>
void GraphNode<DataType, NodeType, ArcType>::removeArc(Node* pNode) {
     list<Arc>::iterator iter = m_arcList.begin();
     list<Arc>::iterator endIter = m_arcList.end();

     int size = m_arcList.size();
     // find the arc that matches the node
     for( ; iter != endIter && m_arcList.size() == size;  
                                                    ++iter ) {
          if ( (*iter).node() == pNode) {
             m_arcList.remove( (*iter) );
          }                           
     }
}


template<typename DataType, typename NodeType, typename ArcType>
void GraphNode<DataType, NodeType, ArcType>::drawArcs(sf::RenderTarget& target) const{
	int arcs = m_arcList.size();
	for (auto a : m_arcList)
		target.draw(a);
}

template<typename DataType, typename NodeType, typename ArcType>
void GraphNode<DataType, NodeType, ArcType>::drawText(sf::RenderTarget& target) const{
	target.draw(m_nameTxt);
	target.draw(m_hCostTxt);
	target.draw(m_gCostTxt);
}

#include "GraphArc.h"

#endif
