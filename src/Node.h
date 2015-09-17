/*
 * Copyright (c) 2015 James Delaplane 
All rights reserved. 
Redistribution and use in source and binary forms, with or without modification, are permitted (subject to the limitations in the disclaimer below) provided that the following conditions are met: 
•	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 
•	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. 
•	Neither the name of James Delaplane nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. 
NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE 
COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
 * Node.h
 *
 *  Created on: Jun 23, 2015
 *      Author: jdelapla
 */

#ifndef NODE_H_
#define NODE_H_
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <set>
#include "BoolComp.h"

struct alpha{
	bool operator() (const std::string lhs,const std::string rhs) const
	{
		if(lhs.compare(rhs) < 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};
class Node {


public:
	//Node * adjacencyList[];
	std::string Color;
	mutable std::vector<std::string> License;
	std::string Name;
	std::string aboutId;
	std::string dotNodeName;
	std::string fileOutput;
	std::string dominantLicense;
	bool visited;
	bool secondCheck;
	int isContained;
	int invisNodeCount;
	int edges;
	int * pinvisNodeCount;
	int * pedges;

	BoolComp * equation;
	Node * isCopyOf;
	Node * containedBy;
	mutable std::vector<std::string> toUnderline;
	mutable std::set<std::string> hasCollapsed;
	mutable std::vector<std::string> fromHere;
	mutable std::vector<std::string> Relationships;
	mutable std::vector<std::string> pointerToRelationships;
	mutable std::vector<Node*> adjacencyList;
	mutable std::vector<Node*> pointerToList;
	mutable std::vector<Node*> Copies;

	/*
	struct findAboutId{
		findAboutId(const std::string & aboutId) : aboutId(aboutId) {}
	    bool operator()(const Node* & node) const {
	        return (node->aboutId.compare(aboutId) == 0);
	    }

	private:
	    std::string aboutId;
	};*/


	//might need to make a comparator struct for the std::set
	/*bool operator<(const Node& rhs)const{
		std::cout << "this was used" << std::endl;
		return	( (this->Name > (rhs.Name)));
	}*/
	void changeContain(Node * node);
	void willCollapse(std::string collapsee);
	Node();
	virtual ~Node();
};
struct comp {
  bool operator() (const Node* lhs, const Node* rhs) const
  {
	  return lhs->Name < rhs->Name;
  }
};




#endif /* NODE_H_ */
