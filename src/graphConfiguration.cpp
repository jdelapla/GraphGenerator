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
 * graphConfiguration.cpp
 *
 *  Created on: Jul 29, 2015
 *      Author: jdelapla
 */


#ifdef DEBUG
#define Debug(x) std::cout << x
#else
#define Debug(x)
#endif

#include "BoolComp.h"
#include "Node.h"
#include <set>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>

void getColor(Node* node, std::map<std::string, std::string> & licenseTree, std::vector<std::string> & dominanceList);
std::string getRelationship(std::string connection, std::map<std::string, std::string> & relationshipTree);
void produceLicenseConfigTree(std::map<std::string, std::string> & licenseTree, std::string path);
void produceRelationshipConfigTree(std::map<std::string, std::string> & relationshipTree, std::string path);
void removeDuplicateEdges(Node * node, Node * toNode, unsigned int skip);
void removeSelfEdges(std::set<Node*,comp> & graph);
void removeIdenticalEdges(std::set<Node*, comp> & graph);
void removeDoubleEdges(std::set<Node*,comp> & graph);
bool parentCheck(Node * node1, Node * node2);
bool licenseCheck(Node * node, Node * secondNode);
Node* makeACopyNode(Node * node, Node * container);
void simplifySiblingNodes(Node * nodeIt);
bool collapseACheck(Node * collapser, Node * collapsee, unsigned int index);
bool collapsePCheck(Node * collapser, Node * collapsee, unsigned int index);
void collapseSinkHole(Node * collapser, Node * collapsee, Node * toSkip, bool origin);
void edgeCount(std::set<Node*,comp> & graph);
void collapseNodes(std::set<Node*, comp> & graph);
void configGraph(std::set<Node*,comp> & graph);




/*************************************************************************************************************************
Name: getColor
Description: using the already generated license config tree, identify the color matching the license and modify the node
Input:	node - pointer to node to be modified
		licenseTree - license configuration map
		dominanceList - vectors of colors in order of dominance
Output: node->Color is modified
*************************************************************************************************************************/
void getColor(Node* node, std::map<std::string, std::string> & licenseTree, std::vector<std::string> & dominanceList){
	std::map<std::string, std::string>::iterator it;
	std::string modifiedLicense = "";
	std::size_t found;
	node->Color = (licenseTree.find("unknown"))->second;
	if(node->dominantLicense != "")
	{
		if(node->dominantLicense.back() == '+')
		{
			modifiedLicense = node->dominantLicense.substr(0, node->dominantLicense.find("+"));
		}
		else if((found = node->dominantLicense.find(" WITH ")) != std::string::npos)
		{
			modifiedLicense = node->dominantLicense.substr(0, found);
		}
		else
			modifiedLicense = node->dominantLicense;
		it = licenseTree.find(modifiedLicense);
		if(it == licenseTree.end())
		{
			node->Color = (licenseTree.find("unknown"))->second;
		}
		else
		{
			if(it->second.compare("") != 0)
			{
				node->Color = it->second;
			}
			else
			{
				node->Color = (licenseTree.find("default"))->second;
			}
		}

	}
	/*
	else if(node->License.size() > 0)
	{
		unsigned int worst = 0;
		std::string dominant = "";
		std::string found = "";
		for(unsigned int it = 0; it < node->License.size(); it++)
		{

			if(licenseTree.find(node->License[it]) != licenseTree.end())
			{
				found = licenseTree.find(node->License[it])->second;
			}
			else
			{
				found = licenseTree.find("unknown")->second;
			}
			for(unsigned int check = 0; check < dominanceList.size(); check++)
			{
				if(found.compare(dominanceList[check]) == 0)
				{
					if(check > worst)
					{
						worst = check;
						dominant = found;
					}
				}
			}
		}
		node->Color = found;
	}*/
}

/*************************************************************************************************************************
Name: getRelationship
Description: identified a SPDX relationship string and returns a corresponding generalized
			 relationship string.
Input: 	relationshipTree -- reference to a map<string,string> of all known relationships and their
						   	corresponding generalized nodes.
		connection		 -- string value of SPDX relationship
return value: generalized relationship string
*************************************************************************************************************************/
std::string getRelationship(std::string connection, std::map<std::string, std::string> & relationshipTree){
	std::map<std::string, std::string>::iterator it;
	it = relationshipTree.find(connection);
	if(it != relationshipTree.end())
	{
		return it->second;
	}
	return "error1";
}

/*************************************************************************************************************************
Name: produceDominanceList
Description: uses the dominanceList.txt file to produce an ordered vector of dominant colors for this graph.
Input: dominanceList -- vector to update
Output: dominanceList is updated from the config file "dominanceList.txt"
*************************************************************************************************************************/
void produceDominanceList(std::vector<std::string> & dominanceList, std::string path){
	std::ifstream configFile;
	path += "domincanceList.txt";

	configFile.open(path);
	std::string line;
	while(std::getline(configFile, line))
	{
		if(line.substr(0,2).compare("//") != 0)
		{
			dominanceList.push_back(line);
		}
	}
}

/*************************************************************************************************************************
Name: produceLicenseConfigTree
Description: produces a map from licenseList.txt
Input: licenseTree - reference to a map to edit based on file input
Output: completely edited licenseTree
*************************************************************************************************************************/
void produceLicenseConfigTree(std::map<std::string, std::string> & licenseTree, std::string path){
	std::ifstream configFile;
	path += "LicenseList.txt";

	configFile.open(path);
	std::string line;
	while(std::getline(configFile, line))
	{
		std::size_t tempPos = line.find("\t");
		if(line.substr(0,2).compare("//") == 0)
		{

		}
		else if(tempPos!=std::string::npos)
		{
			licenseTree[line.substr(0, tempPos)] = line.substr(tempPos+1, std::string::npos);
		}
		else
		{
			std::map<std::string, std::string>::iterator it;
			it = licenseTree.find("default");
			licenseTree[line] = it->second;
		}
	}
	configFile.close();
}
/*************************************************************************************************************************
Name: produceRelationshipConfigTree
Description: produces a map from relationshipList.txt
Input: relationshipTree - reference to a map to edit from the file.
Output: completely edited relationshipTree
*************************************************************************************************************************/
void produceRelationshipConfigTree(std::map<std::string, std::string> & relationshipTree, std::string path){
	std::ifstream configFile;
	path += "relationshipList.txt";
	configFile.open(path);
	std::string line;
	while(std::getline(configFile, line))
	{
		std::size_t tempPos = line.find("\t");
		if(line.substr(0,2).compare("//") == 0)
		{

		}
		else if(tempPos != std::string::npos)
		{
			relationshipTree[line.substr(0,tempPos)] = line.substr(tempPos+1, std::string::npos);
		}
		else
			relationshipTree[line] = "error2";
	}
	configFile.close();
}

/*************************************************************************************************************************
Name: removeDuplicateEdges
Description: change all edges between node and toNode to "blank" except for the edge at iteration point
			 "skip" for node's adjacency list
Input:	node - pointer to primary node
		toNode - pointer to secondary node
		skip - integer value to skip over when iterating through node's adjacency list
Output: some values in relationships[] for node and toNode will be changed to "blank"
*************************************************************************************************************************/
void removeDuplicateEdges(Node * node, Node * toNode, unsigned int skip){
	//traverse first node's adjacency list
	for(unsigned int traverse = 0; traverse < node->adjacencyList.size(); traverse++)
	{
		if(traverse == skip) traverse++;
		if(traverse >= node->adjacencyList.size()) break;
		if(node->adjacencyList[traverse]->dotNodeName.compare(toNode->dotNodeName) == 0)
		{
			Debug("Node: " << node->Name << " (" << node->dotNodeName << ")"<< " had edge: " << node->adjacencyList[traverse]->Name <<
					" (" << node->adjacencyList[traverse]->dotNodeName << ")"<<" \t of type: " << node->Relationships[traverse] <<" \t blanked"<< std::endl);
			for(unsigned int edge = 0; edge < node->adjacencyList[traverse]->pointerToList.size(); edge++)
			{
				if(node->adjacencyList[traverse]->pointerToList[edge]->dotNodeName.compare(
						node->dotNodeName) == 0)
				{
					if(node->adjacencyList[traverse]->pointerToRelationships[edge].compare(
							node->Relationships[traverse]) == 0)
					{
						node->adjacencyList[traverse]->pointerToRelationships[edge] = "blank";
					}
				}
			}
			node->Relationships[traverse] = "blank";
		}
	}
	//traverse second node
	for(unsigned int traverse2 = 0; traverse2 < toNode->adjacencyList.size(); traverse2++)
	{

		if(toNode->adjacencyList[traverse2]->dotNodeName.compare(node->dotNodeName) == 0)
		{
			Debug( "Node: " << toNode->Name << " (" << toNode->dotNodeName << ")"<< " had edge: " << toNode->adjacencyList[traverse2]->Name <<
					" (" << toNode->adjacencyList[traverse2]->dotNodeName << ")"<<" \t of type: " << toNode->Relationships[traverse2] << " \t blanked"<< std::endl);

			for(unsigned int edge = 0; edge < toNode->adjacencyList[traverse2]->pointerToList.size(); edge++)
			{
				if(toNode->adjacencyList[traverse2]->pointerToList[edge]->dotNodeName.compare(
						toNode->dotNodeName) == 0)
				{
					if(toNode->adjacencyList[traverse2]->pointerToRelationships[edge].compare(
												toNode->Relationships[traverse2]) == 0)
					{
						toNode->adjacencyList[traverse2]->pointerToRelationships[edge] = "blank";
					}
				}
			}
			toNode->Relationships[traverse2] = "blank";
		}
	}
}
/*************************************************************************************************************************
Name: removeSelfEdges
Description: sets all edges in the tree that come from and point to the same node to be "blank"
Input: reference to the graph
Output: self edges psuedo-deleted.
*************************************************************************************************************************/
void removeSelfEdges(std::set<Node*,comp> & graph){
	for(std::set<Node*,comp>::iterator nodeIt = graph.begin(); nodeIt != graph.end(); nodeIt++)
	{
		for(unsigned int edgeIt = 0; edgeIt < (*nodeIt)->adjacencyList.size(); edgeIt++)
		{
			if((*nodeIt)->dotNodeName.compare((*nodeIt)->adjacencyList[edgeIt]->dotNodeName) == 0)
			{
				(*nodeIt)->Relationships[edgeIt] = "blank";
			}
		}
		for(unsigned int edgeIt = 0; edgeIt < (*nodeIt)->pointerToList.size(); edgeIt++)
		{
			if((*nodeIt)->dotNodeName.compare((*nodeIt)->pointerToList[edgeIt]->dotNodeName) == 0)
			{
				(*nodeIt)->pointerToRelationships[edgeIt] = "blank";
			}
		}
	}
}
/*************************************************************************************************************************
Name: removeIdenticalEdges
Description: if 2 or more edges are identical, remove all but 1 edge
Input: reference to the graph (std::set)
Output: no identical edges will exist in the graph
*************************************************************************************************************************/
void removeIdenticalEdges(std::set<Node*, comp> & graph){
	for(std::set<Node*,comp>::iterator nodeIt = graph.begin(); nodeIt != graph.end(); nodeIt++)
		{
			for(unsigned int edgeIt = 0; edgeIt < (*nodeIt)->adjacencyList.size(); edgeIt++)
			{
				for(unsigned int check = edgeIt + 1; check <(*nodeIt)->adjacencyList.size(); check++)
				{
					if((*nodeIt)->adjacencyList[edgeIt]->dotNodeName.compare(
							(*nodeIt)->adjacencyList[check]->dotNodeName) == 0)
					{
						if((*nodeIt)->Relationships[edgeIt].compare(
								(*nodeIt)->Relationships[check]) == 0)
						{

							for(unsigned int edge = 0; edge < (*nodeIt)->adjacencyList[check]->pointerToList.size(); edge++)
							{
								if((*nodeIt)->adjacencyList[check]->pointerToList[edge]->dotNodeName.compare(
											(*nodeIt)->dotNodeName) == 0)
									{
										if((*nodeIt)->adjacencyList[check]->pointerToRelationships[edge].compare(
												(*nodeIt)->Relationships[check]) == 0)
										{
											(*nodeIt)->adjacencyList[check]->pointerToRelationships[edge] = "blank";
										}
									}
							}
							(*nodeIt)->Relationships[check] = "blank";
						}
					}
				}
			}
		}
}

/*************************************************************************************************************************
Name: removeDoubleEdges
Description: remove edges that appear identical due to their mirrored nature. For example, foo has a static edge to bar,
			 and bar has a static edge to foo.
Input: reference to the graph
Output: all double edges removed from the graph
*************************************************************************************************************************/
void removeDoubleEdges(std::set<Node*,comp> & graph){
	for(std::set<Node*,comp>::iterator nodeIt = graph.begin(); nodeIt != graph.end(); nodeIt++)
	{
		if(!(*nodeIt)->visited)
		{
			for(unsigned int edgeIt = 0; edgeIt < (*nodeIt)->adjacencyList.size(); edgeIt++)
			{
				if((*nodeIt)->Relationships[edgeIt].compare("blank") == 0)
				{

				}
				else
				{
					for(unsigned int findIt = 0; findIt < (*nodeIt)->adjacencyList[edgeIt]->adjacencyList.size(); findIt++)
					{
						if((*nodeIt)->Name.compare(
								(*nodeIt)->adjacencyList[edgeIt]->adjacencyList[findIt]->Name) == 0)
						{
							if((*nodeIt)->Relationships[edgeIt].compare(
									(*nodeIt)->adjacencyList[edgeIt]->Relationships[findIt]) == 0)
							{
								for(unsigned int edge = 0; edge < (*nodeIt)->adjacencyList[edgeIt]->pointerToList.size(); edge++)
								{
									if((*nodeIt)->adjacencyList[edgeIt]->pointerToList[edge]->dotNodeName.compare(
												(*nodeIt)->dotNodeName) == 0)
										{
											if((*nodeIt)->adjacencyList[edgeIt]->pointerToRelationships[edge].compare(
													(*nodeIt)->Relationships[edgeIt]) == 0)
											{
												(*nodeIt)->adjacencyList[edgeIt]->pointerToRelationships[edge] = "blank";
											}
										}
								}
								(*nodeIt)->Relationships[edgeIt] = "blank";
							}
						}
					}
				}
			}
		}
	}
}

/*************************************************************************************************************************
Name: generateBooleanFormula
Description: creates a BoolComp to represent the boolean algebra of license agreements of a given node.
Input: Node to have a formula generated for.
Output: node->equation member holds the BoolComp object.
*************************************************************************************************************************/
void generateBooleanFormula(Node * node){
	std::stack<BoolComp*> exStack;
	BoolComp * firstEquation;
	BoolComp * newExpression;
	if(node->License.size() == 1)
	{
		newExpression = new BoolComp();
		newExpression->value = node->License[0];
		node->equation = newExpression;
	}
	else if(node->License.size() > 1)
	{
		for(unsigned int it = 0; it < node->License.size(); it++)
		{
			if(node->License[it].compare("(") == 0)
			{
				newExpression = new BoolComp();
				if(!(exStack.empty()))
				{
					newExpression->parent = exStack.top();
					exStack.top()->content.push_back(newExpression);
				}
				exStack.push(newExpression);
			}
			else if(node->License[it].compare(")") == 0)
			{
				BoolComp * temp = exStack.top();
				exStack.pop();
				if(exStack.empty())
				{
					firstEquation = temp;
				}
			}
			else if(node->License[it].compare(",") == 0)
			{
				exStack.top()->boolType = "and";
			}
			else if(node->License[it].compare("/") == 0)
			{
				exStack.top()->boolType = "or";
			}
			else
			{
				newExpression = new BoolComp();
				if(!(exStack.empty()))
				{
					newExpression->parent = exStack.top();
					exStack.top()->content.push_back(newExpression);
				}
				newExpression->value = node->License[it];
			}

		}
		node->equation = firstEquation;
	}
	else
	{
		std::cerr << node->Name << " does not have a License" << std::endl;
	}
	return;

}

/*************************************************************************************************************************
Name: parentCheck
Description: checks if the parental conditions of two nodes are compatible for collapsing
Input: 2 node pointers
return value: true 	-- if the conditions are compatible
			  false -- if the conditions are not compatible
*************************************************************************************************************************/
bool parentCheck(Node * node1, Node * node2){
	while(node1->visited)
	{
		node1 = node1->containedBy;
	}
	while(node2->visited)
	{
		node2 = node2->containedBy;
	}
	if(node1->containedBy == nullptr || node2->containedBy == nullptr)
	{
		return true;
	}
	else if(node1->containedBy->Name == node2->containedBy->Name)
	{
		return true;
	}
	return false;
}

/*************************************************************************************************************************
Name: licenseCheck
Description: checks if the licenses between two nodes are identical
Input: two pointers to nodes to compare
return value: true 	-- if the nodes have at least 1 license in common.
			  false -- if the nodes have no licenses in common.
*************************************************************************************************************************/
bool licenseCheck(Node * node, Node * secondNode){
	if(node->License.size() == 1 && secondNode->License.size() == 1)
	{
		if(node->License[0].compare(secondNode->License[0]) == 0)
		{
			return true;
		}
	}
	else if(node->License.size() > 1 && secondNode->License.size() > 1)
	{
		return node->equation->compare(secondNode->equation);
	}
	return false;

}

/*************************************************************************************************************************
Name: findDominantLicenses
Description: finds the dominant license of a node's given license list
Input: node -- pointer to node
	   licenseTree -- configuration tree containing license and their associated color.
	   dominanceList -- vector of ordered licenses from most acceptable to least acceptable.
Output: node's dominantLicenceses vector is updated, and can be used for color association and collapsing.
*************************************************************************************************************************/
BoolComp * findDominantLicense(Node * node, BoolComp * equation, std::vector<std::string> & dominanceList){
	unsigned int best = 0;
	BoolComp * chosenLicense = nullptr;
	BoolComp * currentLicense = nullptr;
	std::string modifiedLicense = "";
	std::size_t found;
	if(equation->value != "")
	{
		return equation;
	}
	else if(equation->boolType.compare("or") == 0)
	{
		best = dominanceList.size();
		for(unsigned int it = 0; it < equation->content.size(); it++)
		{
			currentLicense = findDominantLicense(node, equation->content[it], dominanceList);
			if(currentLicense == nullptr)
			{

			}
			else if(currentLicense->value.back() == '+')
			{
				modifiedLicense = currentLicense->value.substr(0, currentLicense->value.find("+"));
			}
			else if((found = currentLicense->value.find(" WITH ")) != std::string::npos)
			{
				modifiedLicense = currentLicense->value.substr(0, found);
			}
			else
				modifiedLicense = currentLicense->value;
			for(unsigned int list = 0; list < dominanceList.size(); list++)
			{

				if(dominanceList[list].compare(modifiedLicense) == 0)
				{
					if(best > list)
					{
						best = list;
						chosenLicense = currentLicense;
					}
					break;
				}
			}
		}
		if(best == dominanceList.size())
		{
			chosenLicense = currentLicense;
		}
		equation->dominantChoices.push_back(chosenLicense->value);
		return chosenLicense;
	}
	else if(equation->boolType.compare("and") == 0)
	{
		best = 0;
		for(unsigned int it = 0; it < equation->content.size(); it++)
		{
			currentLicense = findDominantLicense(node, equation->content[it], dominanceList);
			equation->dominantChoices.push_back(currentLicense->value);
			if(currentLicense == nullptr)
			{

			}
			else if(currentLicense->value.back() == '+')
			{
				modifiedLicense = currentLicense->value.substr(0, currentLicense->value.find("+"));
			}
			else if((found = currentLicense->value.find(" WITH ")) != std::string::npos)
			{
				modifiedLicense = currentLicense->value.substr(0, found);
			}
			else
				modifiedLicense = currentLicense->value;
			for(unsigned int list = 0; list < dominanceList.size(); list++)
			{

				if(dominanceList[list].compare(modifiedLicense) == 0)
				{
					if(best <= list)
					{
						best = list;
						chosenLicense = currentLicense;
					}
					break;
				}
			}
			if(chosenLicense ==  nullptr)
			{
				chosenLicense = currentLicense;
			}
		}
		return chosenLicense;
	}
	return nullptr;

























	/*std::string andOr = *(node->License.begin());
	if(andOr.compare(",") == 0)
	{
		node->dominantLicenses.clear();
		return;
	}
	std::string dominant = "";
	unsigned int best = dominanceList.size();
	for(unsigned int it = 0; it < node->License.size(); it++)
	{
		if((node->License[it].compare(",") == 0) ||
				(node->License[it].compare("(") == 0) ||
				(node->License[it].compare(")") == 0) ||
				(node->License[it].compare("/") == 0))
		{
			it++;
			continue;
		}
		std::string found = "";
		if(licenseTree.find((node->License[it])) != licenseTree.end())
		{
			found = licenseTree.find(node->License[it])->second;
		}
		else
		{
			found = licenseTree.find("unknown")->second;
		}
		for(unsigned int check = 0; check < dominanceList.size(); check++)
		{
			if(found.compare(dominanceList[check]) == 0)
			{
				if(check < best)
				{
					best = check;
					dominant = node->License[it];
				}
			}
		}

	}
	if(dominant.compare("") != 0) node->dominantLicenses.insert(dominant);
	else node->dominantLicenses.clear();*/
}

/*************************************************************************************************************************
Name: removeCopy
Description: remove a copy Node from the list of copies
Input: copy node to remove
Output: the copy node is no longer listed in the original's Copies vector.
*************************************************************************************************************************/
void removeCopy(Node * copy){
	if(copy->isCopyOf != nullptr)
	{
		for(unsigned int iterator = 0; iterator < copy->isCopyOf->Copies.size(); iterator++)
		{
			if(copy->isCopyOf->Copies[iterator]->dotNodeName.compare(copy->dotNodeName) == 0)
			{
				copy->isCopyOf->Copies.erase(copy->isCopyOf->Copies.begin() + iterator);
				return;
			}
		}
	}
}

/*************************************************************************************************************************
Name: makeACopyNode
Description: create an identical node, with a different dotNodeName and an empty pointerToList, to be contained by
			 another node.
Input: node -- pointer to node to make a copy of
	   container -- pointer to node to contain the copy
Output: returns the copy node.
*************************************************************************************************************************/
Node* makeACopyNode(Node * node, Node * container){
	Node * newNode = new Node();
	std::stringstream temp;
	newNode->visited = false;
	newNode->Name = node->Name;
	newNode->Color = node->Color;
	temp << node->isContained;
	newNode->dotNodeName = node->dotNodeName + "x" + temp.str();
	newNode->License = node->License;
	newNode->Relationships = node->Relationships;
	newNode->adjacencyList = node->adjacencyList;

	//copy nodes are not allowed to contain other nodes
	for(unsigned int removeContains = 0; removeContains < node->adjacencyList.size(); removeContains++)
	{
		if(newNode->Relationships[removeContains].compare("contain") == 0)
		{
			newNode->Relationships[removeContains] = "blank";
		}
	}
	newNode->isCopyOf = node;
	newNode->edges = node->edges;
	newNode->containedBy = container;
	node->Copies.push_back(newNode);
	return newNode;
}

/*************************************************************************************************************************
Name: containWipe
Description: Recursively removes all edges between a node, and all the descendants of another node. (descendant by contain)
Input:	container -- pointer to original node.
		containedNode -- pointer to node to identify descendants from.
Output: edges to the descendants of another node removed.
*************************************************************************************************************************/
void containWipe(Node * container, Node * containedNode){
	for(unsigned int iterator = 0; iterator < containedNode->adjacencyList.size(); iterator++)
	{
		if(containedNode->Relationships[iterator].compare("contain") == 0)
		{
			removeDuplicateEdges(container, containedNode->adjacencyList[iterator], container->adjacencyList.size() + 2);
			containWipe(container, containedNode->adjacencyList[iterator]);
		}
	}
}

/*************************************************************************************************************************
Name: simplifySiblingNodes
Description: identifies sibling nodes that have no direct edges between them, checks for collapse conditions and if
			 the conditions are met, collapsed the nodes.
Input: nodeIt 	-- pointer to parent node.
Output: all possibly collapsed children nodes are collapsed.
*************************************************************************************************************************/
void simplifySiblingNodes(Node * nodeIt){
	if(!((nodeIt)->visited) && !(nodeIt->secondCheck))
	{
		//original child traversal
		for(unsigned int edgeIt = 0; edgeIt < (nodeIt)->adjacencyList.size(); edgeIt++)
		{
			if(((nodeIt)->Relationships[edgeIt].compare("contain") == 0) && !((nodeIt)->adjacencyList[edgeIt]->visited))
			{
				Debug("\n\n\t\t\t\t1" <<(nodeIt)->adjacencyList[edgeIt]->Name << " (" <<(nodeIt)->adjacencyList[edgeIt]->dotNodeName << ") not visited?" << std::endl);
				//find another child
				for(unsigned int remainingEdges = 0; remainingEdges < (nodeIt)->adjacencyList.size(); remainingEdges++)
				{
					//no duplicates or same edge
					if((nodeIt)->adjacencyList[remainingEdges]->dotNodeName.compare((nodeIt)->adjacencyList[edgeIt]->dotNodeName) != 0)
					{

						if(((nodeIt)->Relationships[remainingEdges].compare("contain") == 0) && !((nodeIt)->adjacencyList[remainingEdges]->visited)
								&& !((nodeIt)->adjacencyList[edgeIt]->visited))
						{
							Debug( "\n\n\t\t\t\t2" <<(nodeIt)->adjacencyList[remainingEdges]->Name << " (" << (nodeIt)->adjacencyList[remainingEdges]->dotNodeName << ") not visited?" << std::endl);
							if(licenseCheck((nodeIt)->adjacencyList[edgeIt], (nodeIt)->adjacencyList[remainingEdges]))
							{
								//passes all checks, determine who should collapse into who
								Node * collapsee = (nodeIt)->adjacencyList[remainingEdges];
								Node * collapser = (nodeIt)->adjacencyList[edgeIt];
								if(collapser->Name.compare(collapsee->Name) == 0 ){
									if(collapsee->Copies.size() > 0)
									{
										if(collapser->Copies.size() == 0)
										{
											Node * temp;
											temp = collapser;
											collapser = collapsee;
											collapsee = temp;
										}
									}
								}
								Debug( "\nChildren of " << (nodeIt)->Name << " (" << (nodeIt)->dotNodeName << ")"
										<< "\n\t" << collapsee->Name << " (" << collapsee->dotNodeName << ") |collapsed by| "
															<< collapser->Name << " (" << collapser->dotNodeName << ")");
								removeDuplicateEdges(collapser, collapsee, collapser->adjacencyList.size() + 2);
								Debug( "\n\t\t Marking " << collapsee->Name << " (" << collapsee->dotNodeName << ") as visited");
								collapsee->visited = true;
								removeCopy(collapsee);
								//transfer edges
								for(unsigned int sendEdge = 0; sendEdge < collapsee->adjacencyList.size(); sendEdge++)
								{

									if(collapsee->Relationships[sendEdge].compare("blank") != 0)
									{

										Debug( "\n\t Adding " << collapsee->adjacencyList[sendEdge]->Name << " (" << collapsee->adjacencyList[sendEdge]->dotNodeName
														<< ") to " << collapser->Name << " (" << collapser->dotNodeName << ")");

										collapser->adjacencyList.push_back(collapsee->adjacencyList[sendEdge]);
										collapser->Relationships.push_back(collapsee->Relationships[sendEdge]);
										collapser->fromHere.push_back(collapsee->dotNodeName);
										if(collapsee->Relationships[sendEdge].compare("contain") == 0)
										{
											collapsee->adjacencyList[sendEdge]->changeContain(collapser);
											containWipe(collapser, collapsee->adjacencyList[sendEdge]);
											removeDuplicateEdges(collapser,collapsee->adjacencyList[sendEdge], collapser->adjacencyList.size() - 1);
										}
										for(unsigned int edge = 0; edge < collapsee->adjacencyList[sendEdge]->pointerToList.size(); edge++)
										{
											if(collapsee->adjacencyList[sendEdge]->pointerToList[edge]->dotNodeName.compare(
													collapsee->dotNodeName) == 0)
											{
												collapsee->adjacencyList[sendEdge]->pointerToList[edge] = collapser;
											}
										}
									}
								}
								//used to display collapsed node names
								if(collapsee->isCopyOf == nullptr)
								{

									if(collapsee->Name.compare(collapser->Name) != 0)
									{
										collapser->willCollapse(collapsee->Name);
									}
								}
								int counter1 = 0;
								for(std::set<std::string>::iterator pass = collapsee->hasCollapsed.begin(); pass != collapsee->hasCollapsed.end(); pass++)
								{
									if(counter1 > 9)
									{
										break;
									}

									if(pass->compare(collapser->Name) != 0)
									{
										collapser->willCollapse(*pass);
									}
									counter1++;
								}
								collapser->aboutId = "I-have.Eaten-Other.Nodes";
								//change pointerToList
								for(unsigned int edge = 0; edge < collapsee->pointerToList.size(); edge++)
								{

									Node * tempParent = collapser->containedBy;
									while((tempParent != nullptr))
									{
										if(tempParent->dotNodeName.compare(collapsee->pointerToList[edge]->dotNodeName) == 0)
										{
											if(collapsee->pointerToRelationships[edge].compare("blank") != 0)
											{

												Debug( "\n\tBlanked from withThis being owned by toReplace (at some point)" << std::endl);
												Debug( "\n\t edge: " << collapsee->pointerToList[edge]->Name << " (" << collapsee->pointerToList[edge]->dotNodeName << ")"
														<< " had edge " << collapsee->Name << " (" << collapsee->dotNodeName << ") of type:" << collapsee->pointerToRelationships[edge]);
												for(unsigned int checking = 0; checking < collapsee->pointerToList[edge]->adjacencyList.size(); checking++)
												{
													if(collapsee->pointerToList[edge]->adjacencyList[checking]->dotNodeName.compare(collapsee->dotNodeName) == 0)
													{
														if(collapsee->pointerToList[edge]->Relationships[checking].compare(collapsee->pointerToRelationships[edge]) == 0)
														{
															collapsee->pointerToList[edge]->Relationships[checking] = "blank";
															collapsee->pointerToRelationships[edge] = "blank";
														}
													}
												}
											}
										}
										tempParent = tempParent->containedBy;
									}

									for(unsigned int edge2 = 0; edge2 < collapsee->pointerToList[edge]->adjacencyList.size(); edge2++)
									{
										if(collapsee->pointerToList[edge]->adjacencyList[edge2]->dotNodeName.compare(
												collapsee->dotNodeName) == 0)
										{
											if(collapsee->pointerToList[edge]->Relationships[edge2].compare("blank") != 0)
											{
												collapsee->pointerToList[edge]->adjacencyList[edge2] = collapser;
											}
										}
									}
								}
								collapsee->changeContain(collapser);
							}
						}
					}
				}
			}
		}
	}
}

/*************************************************************************************************************************
Name: collapseACheck
Description: checks all collapse conditions between potential "collapser" and "collapsee" and the fields related to the
			 collapsee's AdjacencyList
Input: 	collapser 	-- pointer to potential collapser node
		collapsee	-- pointer to owner of the adjacencyList that contains a potential collapsee node
		index		-- index of adjacencyList of "collapsee" input to check.
return value: true	-- if conditions are met
			  false	-- if conditions are not met
*************************************************************************************************************************/
bool collapseACheck(Node * collapser, Node * collapsee, unsigned int index){

	if(collapsee->adjacencyList[index]->visited)
	{
		return false;
	}
	else if((collapsee->Relationships[index] == "contain") || (collapsee->Relationships[index] == "static"))
	{

		if(licenseCheck(collapser, collapsee->adjacencyList[index]))
		{

			if((collapsee->Relationships[index] == "contain") || (parentCheck(collapser, collapsee->adjacencyList[index])))
			{
				return true;
			}
		}
	}

	return false;
}

/*************************************************************************************************************************
Name: collapsePCheck
Description: checks all collapse conditions between potential "collapser" and "collapsee" and the fields related to the
			 collapsee's PointerToList
Input: 	collapser 	-- pointer to potential collapser node
		collapsee	-- pointer to owner of the adjacencyList that contains a potential collapsee node
		index		-- index of pointerToList of "collapsee" input to check.
return value: true	-- if conditions are met
			  false	-- if conditions are not met
*************************************************************************************************************************/
bool collapsePCheck(Node * collapser, Node * collapsee, unsigned int index){

	if(collapsee->pointerToList[index]->visited)
	{
		return false;
	}
	else if((collapsee->pointerToRelationships[index] == "contain")
					|| (collapsee->pointerToRelationships[index] == "static"))
	{

		if(licenseCheck(collapser, collapsee->pointerToList[index]))
		{

			if(parentCheck(collapser, collapsee->pointerToList[index]))
			{
				if(collapsee->pointerToRelationships[index] == "contain")
				{
					return false;
				}
				return true;
			}
		}
	}

	return false;
}

/*************************************************************************************************************************
Name: collapseSinkHole
Description: A recursive collapsing function. This function identifies if the "collapser" node can collapse with any of
			 the edges pointing to the "collapsee" node, or any of the edges the "collapsee" node is pointing to.
			 This acts as a Depth First Search to find all collapses between the original node (collapser) and an already
			 known collapsed node (collapsee).
Input:	collapser	--	pointer to node to collapse into.
		collapsee	--	pointer to already identified node to collapse.
		toSkip		--	pointer to the last identified collapsed node.
		origin		--	bool to help manage overflow. If true the overflow checker is reset.
Output:	-All edges that were owned by the collapsed nodes are transfered to the collapser
			--exceptions include duplicate edges, self edges, and hierarchial incorrect edges.
		-All edges that were pointing to collapsed nodes are redirected to the collapser
			--exceptions apply
		-All collapsed nodes are marked as visited (they will not be added to the dot code).
		Essentially, all possible collapsable nodes for the collapser are identified and collapsed.
*************************************************************************************************************************/
void collapseSinkHole(Node * collapser, Node * collapsee, Node * toSkip, bool origin){
	//avoid overflow
	static int overflowChecker = 0;
	if(origin)
	{
		overflowChecker = 0;
	}
	//remove ALL edges between collapser and collapsee
	removeDuplicateEdges(collapser, collapsee, collapser->adjacencyList.size() + 2);

	//used to be able to display collapsed node names
	if(collapsee->isCopyOf == nullptr)
	{

		if(collapsee->Name.compare(collapser->Name) != 0)
		{
			collapser->willCollapse(collapsee->Name);
		}
	}
	int counter2 = 0;
	for(std::set<std::string>::iterator pass = collapsee->hasCollapsed.begin(); pass != collapsee->hasCollapsed.end(); pass++)
	{
		if(counter2 > 9)
		{
			break;
		}
		if(pass->compare(collapser->Name) != 0)
		{
			collapser->willCollapse(*pass);
		}
		counter2++;
	}
	for(unsigned int lapsee = 0; lapsee < collapsee->adjacencyList.size(); lapsee++)
	{
		//don't traverse the same edge
		if(collapsee->adjacencyList[lapsee]->dotNodeName.compare(toSkip->dotNodeName) == 0)
		{

		}
		//visit check
		else if(!(collapsee->adjacencyList[lapsee]->visited))
		{
			//identify more collapses
			if(collapseACheck(collapser, collapsee, lapsee))
			{
				overflowChecker++;
				if(overflowChecker > 5000)
				{
					break;
				}
				else
				{
					Debug( "\tpassed CollapseACheck" << std::endl);
					Debug( "collapsing | " << collapsee->adjacencyList[lapsee]->Name << " ("
							<< collapsee->adjacencyList[lapsee]->dotNodeName << ")" << "\tinto "
							<< collapser->Name << " (" << collapser->dotNodeName << ")" << std::endl);
					collapsee->adjacencyList[lapsee]->visited = true;
					removeCopy(collapsee->adjacencyList[lapsee]);
					removeDuplicateEdges(collapsee, collapsee->adjacencyList[lapsee], collapsee->adjacencyList.size() +2);
					collapsee->adjacencyList[lapsee]->changeContain(collapser);
					collapseSinkHole(collapser, collapsee->adjacencyList[lapsee], collapsee, false);
				}
			}
			else
			{
				for(unsigned int sendEdge = 0; sendEdge < collapsee->adjacencyList.size(); sendEdge++)
				{
					if(collapsee->adjacencyList[sendEdge]->dotNodeName.compare(
							collapser->dotNodeName) == 0)
					{

					}
					else if(!(collapsee->Relationships[sendEdge].compare("blank") == 0))
					{

						Node * tempParent = collapser->containedBy;
						while((tempParent != nullptr))
						{
							if((collapsee->adjacencyList[sendEdge]->dotNodeName.compare(
										tempParent->dotNodeName) == 0))
							{
								for(unsigned int edge = 0; edge < collapsee->adjacencyList[sendEdge]->pointerToList.size(); edge++)
								{
									if(collapsee->adjacencyList[sendEdge]->pointerToList[edge]->dotNodeName.compare(
												collapsee->dotNodeName) == 0)
										{
											if(collapsee->adjacencyList[sendEdge]->pointerToRelationships[edge].compare(
													collapsee->Relationships[sendEdge]) == 0)
											{
												collapsee->adjacencyList[sendEdge]->pointerToRelationships[edge] = "blank";
											}
										}
								}
								collapsee->Relationships[sendEdge] = "blank";
							}
							tempParent = tempParent->containedBy;
						}
						if(collapsee->Relationships[sendEdge].compare("contain") != 0)
						{
							//check that you're not adding an edge between a node that collapser already contains
							//at some level
							tempParent = collapsee->adjacencyList[sendEdge]->containedBy;
							while((tempParent != nullptr))
							{
								if((tempParent->dotNodeName.compare(
										collapser->dotNodeName) == 0))
								{
									Debug("\n\tHierarchy Blank on " << collapsee->adjacencyList[sendEdge]->Name<<
												" (" << collapsee->adjacencyList[sendEdge]->dotNodeName << ") owned by "
												<< collapsee->Name << " (" << collapsee->dotNodeName << ") of type:\t"
												<< collapsee->Relationships[sendEdge] <<std::endl);
									for(unsigned int edge = 0; edge < collapsee->adjacencyList[sendEdge]->pointerToList.size(); edge++)
									{
										if(collapsee->adjacencyList[sendEdge]->pointerToList[edge]->dotNodeName.compare(
													collapsee->dotNodeName) == 0)
											{
												if(collapsee->adjacencyList[sendEdge]->pointerToRelationships[edge].compare(
														collapsee->Relationships[sendEdge]) == 0)
												{
													collapsee->adjacencyList[sendEdge]->pointerToRelationships[edge] = "blank";
												}
											}
									}
									collapsee->Relationships[sendEdge] = "blank";

								}
								tempParent = tempParent->containedBy;
							}
						}
						//check to make sure you're not adding an edge collapser already has
						for(unsigned int check = 0; check < collapser->adjacencyList.size(); check++)
						{
							if(collapsee->adjacencyList[sendEdge]->dotNodeName.compare(
										collapser->adjacencyList[check]->dotNodeName) == 0)
							{
								if(collapsee->Relationships[sendEdge].compare(
											collapser->Relationships[check]) == 0)
								{
									for(unsigned int edge = 0; edge < collapsee->adjacencyList[sendEdge]->pointerToList.size(); edge++)
									{
										if(collapsee->adjacencyList[sendEdge]->pointerToList[edge]->dotNodeName.compare(
													collapsee->dotNodeName) == 0)
											{
												if(collapsee->adjacencyList[sendEdge]->pointerToRelationships[edge].compare(
														collapsee->Relationships[sendEdge]) == 0)
												{
													collapsee->adjacencyList[sendEdge]->pointerToRelationships[edge] = "blank";
												}
											}
									}
									collapsee->Relationships[sendEdge] = "blank";
								}
							}
						}
						//avoid adding psuedo-deleted edges
						if(collapsee->Relationships[sendEdge].compare("blank") != 0)
						{
							//remove all edges to descendants
							if(collapsee->Relationships[sendEdge].compare("contain") == 0)
							{
								collapsee->adjacencyList[sendEdge]->changeContain(collapser);
								removeDuplicateEdges(collapser, collapsee->adjacencyList[sendEdge], collapser->adjacencyList.size() - 1);
								containWipe(collapser, collapsee->adjacencyList[sendEdge]);
								tempParent = collapser->containedBy;
								while((tempParent != nullptr))
								{
									removeDuplicateEdges(tempParent, collapsee->adjacencyList[sendEdge], tempParent->adjacencyList.size() + 2);
									tempParent = tempParent->containedBy;
								}

							}
							Debug( "\n\t Adding " << collapsee->adjacencyList[sendEdge]->Name << " (" << collapsee->adjacencyList[sendEdge]->dotNodeName
									<< ") to " << collapser->Name << " (" << collapser->dotNodeName << ")" << std::endl);
							collapser->adjacencyList.push_back(collapsee->adjacencyList[sendEdge]);
							collapser->Relationships.push_back(collapsee->Relationships[sendEdge]);
							collapser->fromHere.push_back(collapsee->dotNodeName);

							//change pointerToList of collapsee
							for(unsigned int edge = 0; edge < collapsee->adjacencyList[sendEdge]->pointerToList.size(); edge++)
							{
								if(collapsee->adjacencyList[sendEdge]->pointerToList[edge]->dotNodeName.compare(
											collapsee->dotNodeName) == 0)
								{
									if(collapsee->adjacencyList[sendEdge]->pointerToRelationships[edge].compare(
											collapsee->Relationships[sendEdge]) == 0)
									{
										collapsee->adjacencyList[sendEdge]->pointerToList[edge] = collapser;
									}
								}
							}
						}
					}
				}
			}

		}
	}
	//traverse pointerToList
	for(unsigned int lapsee = 0; lapsee < collapsee->pointerToList.size(); lapsee++)
	{
		if(collapsee->pointerToList[lapsee]->dotNodeName.compare(toSkip->dotNodeName) == 0)
		{

		}
		else if (collapsee->pointerToRelationships[lapsee].compare("blank") == 0)
		{

		}
		else if(!(collapsee->pointerToList[lapsee]->visited))
		{
			//identify more collapses
			if(collapsePCheck(collapser, collapsee, lapsee))
			{
				overflowChecker++;
				if(overflowChecker > 5000)
				{
					break;
				}
				else if(collapsee->pointerToList[lapsee]->dotNodeName.compare(collapser->dotNodeName) == 0)
				{

				}
				else
				{
					Debug( "passed CollapsePCheck" << std::endl);
					Debug( "collapsing | " << collapsee->pointerToList[lapsee]->Name << " (" << collapsee->pointerToList[lapsee]->dotNodeName << ")" << "\tinto "
							<< collapser->Name << " (" << collapser->dotNodeName << ")" << "\n\t\t because of already collapsed: " << collapsee->Name << " (" << collapsee->dotNodeName << ")"<< std::endl);
					collapsee->pointerToList[lapsee]->visited = true;
					removeCopy(collapsee->pointerToList[lapsee]);
					removeDuplicateEdges(collapsee, collapsee->pointerToList[lapsee], collapsee->adjacencyList.size()+2);
					collapsee->pointerToList[lapsee]->changeContain(collapser);
					collapseSinkHole(collapser, collapsee->pointerToList[lapsee], collapsee, false);
				}
			}
			else
			{
				//find the index of the node on the collapsee's pointerToList's adjacencyList that is the collapsee
				unsigned int foundEdge = 0;
				for(unsigned int edge = 0; edge < collapsee->pointerToList[lapsee]->adjacencyList.size(); edge++ )
				{
					if((collapsee->pointerToList[lapsee]->adjacencyList[edge]->dotNodeName.compare(
							collapsee->dotNodeName) == 0)
							&& (collapsee->pointerToRelationships[lapsee].compare("blank") != 0))
					{
						foundEdge = edge;
						break;
					}
				}

				//avoid adding edges to the collapser to nodes it already contains at some level
				Node * tempParent = collapsee->pointerToList[lapsee]->containedBy;
				while((tempParent != nullptr))
				{
					if(tempParent->dotNodeName.compare(collapser->dotNodeName) == 0)
					{
						Debug("happened for " << collapsee->pointerToList[lapsee]->Name << " (" << collapsee->pointerToList[lapsee]->dotNodeName << ")"<<
								" with " << collapsee->pointerToList[lapsee]->adjacencyList[foundEdge]->Name << " ("
								<< collapsee->pointerToList[lapsee]->adjacencyList[foundEdge]->dotNodeName << ")"
								<< std::endl);
						for(unsigned int edge = 0; edge < collapsee->pointerToList[lapsee]->adjacencyList.size(); edge++ )
						{
							if((collapsee->pointerToList[lapsee]->adjacencyList[edge]->dotNodeName.compare(
									collapsee->dotNodeName) == 0)
									&& (collapsee->pointerToRelationships[lapsee].compare("blank") != 0))
							{
								collapsee->pointerToList[lapsee]->Relationships[edge] = "blank";
							}
						}

						collapsee->pointerToRelationships[lapsee] = "blank";
					}
					tempParent = tempParent->containedBy;
				}

				//avoid adding edges to the collapser to nodes that contain it
				tempParent = collapser->containedBy;
				while(tempParent != nullptr)
				{
					if(tempParent->dotNodeName.compare(collapsee->pointerToList[lapsee]->dotNodeName) == 0)
					{
						Debug( "\n\tBlanked from withThis being owned by toReplace (at some point)" << std::endl);
						Debug( "\n\t egde: " << collapsee->pointerToList[lapsee]->Name << " (" << collapsee->pointerToList[lapsee]->dotNodeName << ")" << " pointingTo "
								<< collapsee->pointerToList[lapsee]->adjacencyList[foundEdge]->Name << " (" << collapsee->pointerToList[lapsee]->adjacencyList[foundEdge]->dotNodeName << ")"
								<< "\t of type: " << collapsee->pointerToList[lapsee]->Relationships[foundEdge]);
						collapsee->pointerToList[lapsee]->Relationships[foundEdge] = "blank";
						collapsee->pointerToRelationships[lapsee] = "blank";
					}
					tempParent = tempParent->containedBy;
				}
				//ignore psuedo-deleted edges
				if(!(collapsee->pointerToList[lapsee]->Relationships[foundEdge].compare("blank") == 0))
				{
					//if the edge is a contain, do the clears for descendants
					if(collapsee->pointerToList[lapsee]->Relationships[foundEdge].compare("contain") == 0)
					{
						collapser->changeContain( collapsee->pointerToList[lapsee]);
						removeDuplicateEdges(collapsee->pointerToList[lapsee], collapser, foundEdge);
						containWipe(collapsee->pointerToList[lapsee], collapser);
						tempParent = collapser->containedBy->containedBy;
						while(tempParent != nullptr)
						{
							removeDuplicateEdges(tempParent, collapser, tempParent->adjacencyList.size() + 2);
							tempParent = tempParent->containedBy;
						}

					}
					Debug( "\n\t" << collapsee->pointerToList[lapsee]->Relationships[foundEdge]
							  <<"\t" << collapsee->pointerToList[lapsee]->adjacencyList[foundEdge]->Name << " (" << collapsee->pointerToList[lapsee]->adjacencyList[foundEdge]->dotNodeName << ")"
							  << " to be " << collapser->Name << " (" << collapser->dotNodeName << ")"
							  << "\n\t\t\t owned by" << collapsee->pointerToList[lapsee]->Name << std::endl);
					collapsee->pointerToList[lapsee]->adjacencyList[foundEdge] = collapser;
					collapser->pointerToList.push_back(collapsee->pointerToList[lapsee]);
					collapser->pointerToRelationships.push_back(collapsee->pointerToList[lapsee]->Relationships[foundEdge]);
				}
				else
				{
					Debug( "\n\t" << collapsee->pointerToList[lapsee]->Relationships[foundEdge]
					         <<"\t" << collapsee->pointerToList[lapsee]->adjacencyList[foundEdge]->Name
							 << "\n\t\t\t owned by" << collapsee->pointerToList[lapsee]->Name << std::endl);
				}
			}
		}
	}
}

/*************************************************************************************************************************
Name: edgeCount
Description: traverses the graph in order to count how many edges each node has.
Input: graph -- reference to the graph
Output: pedges member of each node in the graph is updated.
*************************************************************************************************************************/
void edgeCount(std::set<Node*,comp> & graph){
	for(std::set<Node*,comp>::iterator it = graph.begin(); it != graph.end(); it++)
	{
		if(!(*it)->visited)
		{
			for(unsigned int relIt = 0; relIt < (*it)->Relationships.size(); relIt++)
			{
				if((*it)->adjacencyList[relIt]->Name == (*it)->Name)
				{
					//no self edges
				}
				else if(((*it)->Relationships[relIt] == "dynamic") || ((*it)->Relationships[relIt] == "static") )
				{
					*((*it)->pedges)+= 1;
					*((*it)->adjacencyList[relIt]->pedges)+= 1;
					for(unsigned int copyIt = 0; copyIt < (*it)->Copies.size(); copyIt++)
					{
						*((*it)->Copies[copyIt]->pedges)+= 1;
						*((*it)->adjacencyList[relIt]->pedges)+= 1;
					}
					for(unsigned int copyIt2 = 0; copyIt2 < (*it)->adjacencyList[relIt]->Copies.size(); copyIt2++)
					{
						*((*it)->pedges)+= 1;
						*((*it)->adjacencyList[relIt]->Copies[copyIt2]->pedges)+= 1;
						for(unsigned int copyIt = 0; copyIt < (*it)->Copies.size(); copyIt++)
						{
							*((*it)->Copies[copyIt]->pedges)+= 1;
							*((*it)->adjacencyList[relIt]->Copies[copyIt2]->pedges)+= 1;
						}
					}
				}
			}
		}
	}
}

/*************************************************************************************************************************
Name: collapseNodes
Description: Traverses the entire graph to identify uncollapsed nodes. Used to begin collapseSinkHole when the depth
			 first search reaches it's conclusion, and find other collapse sink holes.
Input:	graph	-- reference the std::set graph.
Output: All possible collapses (excluding sibling node collapses) will be resolved.
*************************************************************************************************************************/
void collapseNodes(std::set<Node*, comp> & graph){
	for(std::set<Node*,comp>::iterator nodeIt = graph.begin(); nodeIt != graph.end(); ++nodeIt)
	{

		Debug( (*nodeIt)->Name << " ");
		Debug( (*nodeIt)->Relationships.size() << std::endl);
		for(unsigned int edgeIt = 0; edgeIt < (*nodeIt)->Relationships.size(); edgeIt++)
		{

			Debug( "\n\t" << (*nodeIt)->adjacencyList[edgeIt]->Name << " ");
			if(collapseACheck(*nodeIt, *nodeIt, edgeIt))
			{
				Node * collapsee = (*nodeIt)->adjacencyList[edgeIt];
				Node * collapser = (*nodeIt);
				if(collapser->Name.compare(collapsee->Name) == 0 ){
					if(collapsee->Copies.size() > 0)
					{
						if(collapser->Copies.size() == 0)
						{
							Node * temp;
							temp = collapser;
							collapser = collapsee;
							collapsee = temp;
						}
					}
				}
				collapser->aboutId = "I-have.Eaten-Other.Nodes";
				collapsee->visited = true;
				removeCopy(collapsee);
				Debug( "collapsing | " << collapsee->Name << " ("
						<< collapsee->dotNodeName << ")" << "\tinto "
						<< collapser->Name << " (" << collapser->dotNodeName << ")" << std::endl);
				collapsee->changeContain(collapser);
				collapseSinkHole(collapser, collapsee, collapser, true);
			}
			Debug(std::endl);
		}
	}
}
/*************************************************************************************************************************
Name: removeUnattachedNodes
Description: psuedo-delete nodes that are apart of the graph, but have no relationships with any other nodes.
Input:	graph -- reference to the graph of nodes
Output: all nodes in the graph that consist of either no edges, or only blank edges (coming from OR pointing to)
		are marked as visited.
*************************************************************************************************************************/
void removeUnattachedNodes(std::set<Node*,comp> & graph){
	int checker = 0;
	for(std::set<Node*,comp>::iterator nodeIt = graph.begin(); nodeIt != graph.end(); nodeIt++)
	{
		if(!((*nodeIt)->visited))
		{
			checker++;
		}
	}
	if(checker < 2)
	{
		return;
	}
	for(std::set<Node*,comp>::iterator nodeIt = graph.begin(); nodeIt != graph.end(); nodeIt++)
	{

		bool isUsed = false;
		if(!((*nodeIt)->visited))
		{
			for(unsigned int traverse = 0; traverse < (*nodeIt)->adjacencyList.size(); traverse++)
			{
				if((*nodeIt)->Relationships[traverse].compare("blank") != 0)
				{
					isUsed = true;
				}
			}
			for(unsigned int traverse = 0; traverse < (*nodeIt)->pointerToList.size(); traverse++)
			{
				if((*nodeIt)->pointerToRelationships[traverse].compare("blank") != 0)
				{
					isUsed = true;
				}
			}
			if(!isUsed)
			{
				(*nodeIt)->visited = true;
			}
		}
	}
}
/*************************************************************************************************************************
Name: establishContains
Description: contain relationships are prepared by removing other unnecessary edges, and making copy nodes when a
			 single node is contained more than once.
			 In addition, this function also sets up the toPointerList for all nodes in the graph.
Input:	graph -- reference to the graph of nodes
Output: graph is configured to handle contain relationships, copy nodes are made, toPointerList is updated for all nodes.
*************************************************************************************************************************/
void establishContains(std::set<Node*,comp> & graph){
	for(std::set<Node*,comp>::iterator it = graph.begin(); it != graph.end(); it++)
	{
		for(unsigned int relIt = 0; relIt < (*it)->Relationships.size(); relIt++)
		{
			if((*it)->adjacencyList[relIt]->Name == (*it)->Name)
			{
				//no self edges
			}
			else if((*it)->Relationships[relIt] == "contain")
			{
				removeDuplicateEdges(*it, (*it)->adjacencyList[relIt], relIt);

				if((*it)->adjacencyList[relIt]->isContained)
				{
					(*it)->adjacencyList[relIt]->isContained++;
					(*it)->adjacencyList[relIt] = makeACopyNode((*it)->adjacencyList[relIt], (*it));
				}
				(*it)->adjacencyList[relIt]->changeContain((*it));
				(*it)->adjacencyList[relIt]->pointerToList.push_back((*it));
				(*it)->adjacencyList[relIt]->pointerToRelationships.push_back((*it)->Relationships[relIt]);
				(*it)->adjacencyList[relIt]->isContained++;

			}
			else
			{
				(*it)->adjacencyList[relIt]->pointerToList.push_back((*it));
				(*it)->adjacencyList[relIt]->pointerToRelationships.push_back((*it)->Relationships[relIt]);
			}
		}

	}
}

/*************************************************************************************************************************
Name: configGraph
Description: The "main" function of this library. Utilizes the library to remove all unnecessary edges, instantiate
			 collapses, set up contain relationships, and properly color the nodes.
Input:	graph -- reference to the graph of nodes
Output: properly configured graph (for dot code writing). All redundancies removed.
*************************************************************************************************************************/
void configGraph(std::set<Node*,comp> & graph, std::string path){
	std::string temp = "cluster";
	std::map<std::string, std::string> licenses;
	std::map<std::string, std::string> relationshipTree;
	std::vector<std::string> dominanceList;
	int count = 0;
	std::stringstream countStr;
	produceLicenseConfigTree(licenses, path);
	produceRelationshipConfigTree(relationshipTree, path);
	produceDominanceList(dominanceList, path);

	//dotNodeNames and color
	for(std::set<Node*,comp>::iterator it = graph.begin(); it != graph.end(); it++)
	{
		generateBooleanFormula(*it);

		BoolComp * dominanceResult = findDominantLicense((*it), (*it)->equation, dominanceList);

		if(dominanceResult->parent != nullptr)
		{
			for(unsigned int i = 0; i < dominanceResult->parent->dominantChoices.size(); i++)
			{
				(*it)->toUnderline.push_back(dominanceResult->parent->dominantChoices[i]);
			}
		}
		else
		{
			(*it)->toUnderline.push_back(dominanceResult->value);
		}

		(*it)->dominantLicense = dominanceResult->value;
		/*for(std::set<std::string, alpha>::iterator i = (*it)->License.begin(); i != (*it)->License.end(); i++)
		{

		}*/

		++count;
		countStr << count;
		(*it)->dotNodeName = temp + countStr.str();
		countStr.str(std::string());
		getColor(*it, licenses, dominanceList);
		for(unsigned int relIt = 0; relIt < (*it)->Relationships.size(); relIt++)
		{
			(*it)->Relationships[relIt] = getRelationship((*it)->Relationships[relIt], relationshipTree);
		}
	}

	removeSelfEdges(graph);
	establishContains(graph);
	collapseNodes(graph);
	//simplifySiblingNodes is faster if done from the node instead of from the graph.
	for(std::set<Node*,comp>::iterator nodeIt = graph.begin(); nodeIt != graph.end(); ++nodeIt)
	{
		simplifySiblingNodes(*nodeIt);
		(*nodeIt)->secondCheck = true;
		for(unsigned int traverse = 0; traverse < (*nodeIt)->adjacencyList.size(); traverse++)
		{
			if((*nodeIt)->Relationships[traverse].compare("blank") != 0)
			{
				simplifySiblingNodes((*nodeIt)->adjacencyList[traverse]);
				(*nodeIt)->adjacencyList[traverse]->secondCheck = true;
			}
		}
	}
	removeDoubleEdges(graph);
	removeIdenticalEdges(graph);
	edgeCount(graph);
	removeUnattachedNodes(graph);


}


