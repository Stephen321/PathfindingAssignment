#ifndef GRAPHARC_H
#define GRAPHARC_H

#include "GraphNode.h"
#include <iostream>
// -------------------------------------------------------
// Name:        GraphArc
// Description: This is the arc class. The arc class
//              points to a graph node, and contains a 
//              weight.
// -------------------------------------------------------

template<class DataType, class NodeType, class ArcType>
class GraphArc : public sf::Drawable {
private:

// -------------------------------------------------------
// Description: pointer to the node that the arc points to
// -------------------------------------------------------
	GraphNode<DataType, NodeType, ArcType>* m_pNode;

// -------------------------------------------------------
// Description: Weight of the arc
// -------------------------------------------------------
    ArcType m_weight;


	sf::Vertex m_line[2];
	
	sf::Font font;
	sf::Text m_text;



public:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const{
		target.draw(m_line, 2, sf::Lines);
	}

	void setLine(sf::Vector2f start, sf::Vector2f end){
		m_line[0] = start;
		m_line[1] = end;
	}

	void setColour(sf::Color colour, int index){
		m_line[index].color = colour;
	}

    // Accessor functions
    GraphNode<DataType, NodeType, ArcType>* node() const {
        return m_pNode;
    }
                              
    ArcType weight() const {
        return m_weight;
    }
    
    // Manipulator functions
    void setNode(GraphNode<DataType, NodeType, ArcType>* pNode) {
		m_pNode = pNode;
    }
    
    void setWeight(ArcType weight) {
       m_weight = weight;
    }
    
};

#endif
