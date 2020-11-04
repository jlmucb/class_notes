//
//		Draw Graph in a plane
//
//		Input:
//			Int iDirected	- 0 if undirected graph
//			Nodes:
//				Int nn - number of nodes
//				Tchar *[nn] - name of nodes
//				Color [nn]	- color of nodes
//			Links
//				Int nl - number of links
//				Tchar *[nl] - name of link
//				Color [nl] - color of link
//				(Int,Int)[nl] - link connections


//	(c) Copyright, 1995, John Manferdelli.


#include "stdafx.h"
#include "graph.h"



/* ------------------------------------------------------------------------------------------ */




BOOL RotatePlanarPoint(double xXin, double xYin, double xAngle, double* pxXout, double* pxYout)

{
	*pxXout= xXin*cos(xAngle)-xYin*sin(xAngle);
	*pxYout= xYin*cos(xAngle)-xXin*sin(xAngle);
	return(TRUE);
}



Graph::Graph()

{
	iNumberofNodes= 0;
	pNodeList= NULL;
	iNumberofLinks= 0;
	iLabelMaxHeight= 0;
	iLabelMaxWidth= 0;
	}



BOOL Graph::ScaleGraph(RECT rectDisplayArea)

{
	return TRUE;
	}



BOOL Graph::Init(RECT rectDisplayArea)

{
	}


BOOL Graph::InitLayout(RECT rectDisplayArea)

{
	double xAngle, xSideToRadiusRatio;
	Int iXsize, iYsize;
	Int iXVertex, iYVertex, iCenter;

	// Int iNumberofNodes;	GNode*	pNodes;	RECT rectTotalArea; RECT rectGraphArea;

	if(iNumberofNodes<2)
		return(FALSE);

	xAngle= (2.0*Pi)/((double)(this->iNumberofNodes));
	xSideToRadiusRatio= sqrt(2.0*(1.0-cos(xAngle)));
	iXsize= rectDisplayArea.right-rectDisplayArea.left;
	iYsize= rectDisplayArea.bottom-rectDisplayArea.top;
	iLabelMaxWidth= 80;
	iLabelMaxHeight= 40;

	// Calculate Drawing Area
	// RECT (left, right, top, bottom)
	{
		Int itx, ity, iSquare;

		itx= iXsize-2*iLabelMaxWidth;
		ity= iYsize-2*iLabelMaxHeight;
		if(itx>ity)
			iSquare= ity;
		else
			iSquare= itx;
		rectTotalArea.left= rectDisplayArea.left;
		rectTotalArea.right= rectDisplayArea.right;
		rectTotalArea.top= rectDisplayArea.top;
		rectTotalArea.bottom= rectDisplayArea.bottom;

		rectGraphArea.left= rectTotalArea.left+iLabelMaxWidth;
		rectGraphArea.top= rectTotalArea.top+iLabelMaxHeight;
		rectGraphArea.right= rectGraphArea.left+iSquare;
		rectGraphArea.bottom= rectGraphArea.top+iSquare;

		iCenter= iSquare/2;
		}

	
	iXVertex= rectGraphArea.right;
	iYVertex= rectGraphArea.top+iCenter;

	// Now assign positions to node points
	Int i;
	GNodeList* pThisListPosition= pNodeList;
	for(i=0;i<iNumberofNodes;i++) {

		GNode* pThisNode;
		double xcX,xcY,xnX,xnY;

		if(pThisListPosition==NULL)
			return(FALSE);

		pThisNode= pThisListPosition->pCurrentNode;

		pThisNode->iXPosition= iXVertex;
		pThisNode->iYPosition= iYVertex;

		xcX= (double)(iXVertex-rectGraphArea.left-iCenter);
		xcY= (double)(-(iYVertex-rectGraphArea.top-iCenter));
		// fix this it isnt right
		::RotatePlanarPoint(xcX, xcY, xAngle, &xnX, &xnY);

		iXVertex= (Int)xnX;
		iYVertex= (Int)(-xnY);

		iXVertex+= rectGraphArea.left+iCenter;
		iYVertex+= rectGraphArea.top+iCenter;

		pThisListPosition= pThisListPosition->pNextNodeList;
		}
	
	return(TRUE);
}



void DrawANode(CDC* pDC, const GNode& grNode, Int iRadius)

// Assume Mapmode and name is ok.

{

	pDC->Rectangle(grNode.iXPosition-iRadius,grNode.iYPosition-iRadius,
			       grNode.iXPosition+iRadius,grNode.iYPosition+iRadius);
	pDC->TextOut(grNode.iXPosition+iRadius*2,grNode.iYPosition, grNode.pstrNodeName);
	
	}



void DrawALink(CDC* pDC,const GLink& grLink, Int iWidth)

// assume mapmode and name is ok.

{
	GNode* g1;	GNode* g2;
	Int ix1,iy1,ix2,iy2;

	g1= grLink.pSource;
	g2= grLink.pDestination;
	ASSERT(g1!=NULL);
	ASSERT(g2!=NULL);

	ix1= g1->iXPosition;
	iy1= g1->iYPosition;
	ix2= g2->iXPosition;
	iy2= g2->iYPosition;
	pDC->MoveTo(ix1,iy1);
	pDC->LineTo(ix2,iy2);
	pDC->TextOut(((ix1+ix2)/2)+iWidth,(iy1+iy2)/2,grLink.pstrLinkName);
	}




/* ------------------------------------------------------------------------------------------ */





