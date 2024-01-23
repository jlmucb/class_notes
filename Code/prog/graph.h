

//	(c) Copyright, 1995, John Manferdelli.


/* ------------------------------------------------------------------------------------------ */



#ifndef GRAPH_H
#define GRAPH_H



#include "MyTypes.h"
#include <math.h>



struct GNode;


struct LinkListNode {
	GNode*			pNode;
	LinkListNode*	pNextLink;
	};


struct ShortLink {
	Int		iSource;
	GNode*	pSource;
	Int		iDestination;
	GNode*	pDestination;
	};


struct GNode {
	Int				iNodeNumber;
	Tchar*			pstrNodeName;
	Int				iInDegree;
	Int				iOutDegree;
	Color			cNodeColor;
	Int				iXPosition;
	Int				iYPosition;
	RECT			rectTextRectangle;
	LinkListNode*	lLinks;
	};


struct GNodeList {
	GNode*		pCurrentNode;
	GNodeList*	pNextNodeList;
	};


struct GLink {
	Int			iSource;
	GNode*		pSource;
	Int			iDestination;
	GNode*		pDestination;
	Tchar*		pstrLinkName;
	Color		cLinkColor;
	RECT		rectTextRectangle;
	};


class Graph {

public:
	Int			iNumberofNodes;
	GNodeList*	pNodeList;
	Int			iNumberofLinks;
	GLink*		pLinks;
	RECT		rectTotalArea;
	RECT		rectGraphArea;
	Int			iLabelMaxWidth, iLabelMaxHeight;
	Int			iNodeRadius;
	Int			iLinkWidth;

public:
	Graph();
	
	BOOL	InitGraph(Int iNodes, Tchar* pstrNodeName[], Color cNodeColors[],
					  Int iLinks, ShortLink links[],Tchar* pstrLinkName[], Color cLinkColors[]);
	BOOL	Init(RECT);
	BOOL	InitLayout(RECT rectDisplayArea);
	BOOL	ScaleGraph(RECT rectDisplayArea);
	BOOL	DrawGraph(CDC*);
	void	DrawNodes(CDC*);
	void	DrawLinks(CDC*);
	};



#endif



/* ------------------------------------------------------------------------------------------ */

