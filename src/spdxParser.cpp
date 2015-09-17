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
 * spdxParser.cpp
 *
 *  Created on: Jul 29, 2015
 *      Author: jdelapla
 */
#include "spdxParser.h"
#include "spdxNomenclature.h"

//a debug tool I may or may not use
#define BAD_THING 50

//because there are far too many
using namespace tinyxml2;

const char* getStringFromXMLNode(XMLNode * node);
XMLNode * traverseToSpecificXMLNode(XMLNode* node, int arguments, const char* targets []);
XMLNode * findXMLNodeWithAttribute(XMLNode * node, const char* attribute, const char* target);
void relationshipFinder(XMLNode* node, Node* newNode, std::set<Node*,comp> & tree);
std::string licenseFinder(XMLNode * tempXMLNode);
Node * createNodeFromPackage(XMLNode ** node, std::set<Node*,comp> & graph);
Node * createNodeFromFile(XMLNode ** node, std::set<Node*,comp> & graph);
Node * createNodeFromDocument(XMLNode ** node, std::set<Node*,comp> & graph);
void traverseXML(XMLNode * node, std::set<Node*,comp> & graph);
void parseFile(const char* file, std::set<Node*,comp> & graph);



//our "graph"
static XMLNode * root;



/*************************************************************************************************************************
Name: getStringFromXMLNode
Description: find a text node child of a given XMLNode and pass it as a const char*
Input: node - pointer to node with suspected text child.
Output: returns a string if there is a text node,
		otherwise returns NULL and prints an error message.
*************************************************************************************************************************/
const char* getStringFromXMLNode(XMLNode * node){
	if(node->FirstChild() != nullptr)
	{
		return node->FirstChild()->ToText()->Value();
	}
	else
	{
		return "";
	}
}
/*************************************************************************************************************************
Name: traverseToSpecificXMLNode
Description: Traverses a specific set nodes (children, not siblings)
Input: 	node - pointer to node that fathers the nodes you wish to traverse
		arguments - number of children to traverse
		targets - the list of children to traverse in order of traversal (node->child->child->child-> etc.)
Output: returns a pointer to the node traversed last if the traversal was successful
		If the traversal fails at any point it will return that last successfully traversed node,
		If no successful traversals occur, it will return the "node" argument originally passed.
*************************************************************************************************************************/
XMLNode * traverseToSpecificXMLNode(XMLNode* node, int arguments, const char* targets []){

	for(int amount = 0; amount < arguments; amount++)
	{
		if(node->FirstChildElement(targets[amount]) != nullptr)
		{
			node = node->FirstChildElement(targets[amount]);
		}
		else
		{
			return node;
		}
	}
	return node;
}


/*************************************************************************************************************************
Name: findXMlNodeWithAttribute
Description: find the first declaration of a type of attribute with a target string value, starting from the passed node.
Input: 	node - pointer to XMLNode to begin traversing from
		attribute - name of type of attribute to search for (const char*)
		target - string for the attribute to contain (const char*)
Output: returns pointer to node containing the target attribute,
		or nullptr if it doesn't exist.
*************************************************************************************************************************/
XMLNode * findXMLNodeWithAttribute(XMLNode * node, const char* attribute, const char* target){
	if(node == nullptr)
	{
		return nullptr;
	}
	else if(node->ToElement() != nullptr )
	{
		if( (node->ToElement()->Attribute(attribute, target)) != nullptr)
		{
			return node;
		}
	}
	XMLNode * temp = findXMLNodeWithAttribute(node->FirstChild(), attribute, target);
	if(temp == nullptr)
	{
		temp = findXMLNodeWithAttribute(node->NextSibling(), attribute, target);
	}
	return temp;

}
/*************************************************************************************************************************
Name: relationshipFinder
Description: identifies the contents of a <spdx:relationship> class for a passed node, regardless of node type, and
			 stores found information in the graph.
Input:	node - pointer to XMLNode to find relationships of
		newNode - pointer to Node that will be inserted in the graph
Output: updates the std::vector<std::string> Relationships
				and std::vector<Node*> adjacencylist
				for newNode if any relationships are found.
*************************************************************************************************************************/
void relationshipFinder(XMLNode* node, Node* newNode, std::set<Node*,comp> & tree){
	XMLNode * tempXMLNode;
	const char* relationshipType;
	const char* resource;
	if(node->FirstChild())
	{
		node = node->FirstChild();
		while(node)
		{
			if(((std::string)node->ToElement()->Value()).compare(SPDX_RELATIONSHIP) == 0)
			{

				tempXMLNode = node->FirstChildElement(RELATIONSHIP);
				if((relationshipType = tempXMLNode->FirstChildElement(RELATIONSHIP_TYPE)->Attribute(ATTRIBUTE_RESOURCE)))
				{
					newNode->Relationships.push_back(relationshipType);
					if((resource = tempXMLNode->FirstChildElement(RELATED_SPDX_ELEMENT)->Attribute(ATTRIBUTE_RESOURCE)))
					{
						//If this happens it means the node associated with this relationship was
						//already created

						//need to find the already created node

						for(std::set<Node*>::iterator it = tree.begin(); it != tree.end() ; it++)
						{
							if((*it)->aboutId.compare(resource)==0)
							{
								newNode->adjacencyList.push_back(*it);
							}
						}


					}
					else if(tempXMLNode->FirstChildElement(RELATED_SPDX_ELEMENT)->FirstChild())
					{
						//if this happens it's a new node entirely
						XMLNode * temp = tempXMLNode->FirstChildElement(RELATED_SPDX_ELEMENT)->FirstChild();
						Node * tempNode = nullptr;
						if(((std::string)temp->Value()).compare(SPDX_ELEMENT) == 0)
						{
							//only known to occur for "copy of" relationship
							tempNode = new Node();
							tempNode->Name = temp->ToElement()->Attribute(ATTRIBUTE_ABOUT);
						}
						if(((std::string)temp->Value()).compare(SPDX_FILE) == 0)
						{
							tempNode = createNodeFromFile(&temp, tree);
						}
						else if(((std::string)temp->Value()).compare(SPDX_PACKAGE) == 0)
						{
							tempNode = createNodeFromPackage(&temp, tree);
						}
						else if(((std::string)temp->Value()).compare(SPDX_DOCUMENT) == 0)
						{
							tempNode = createNodeFromDocument(&temp, tree);
						}
						if(tempNode != nullptr)
						{
							newNode->adjacencyList.push_back(tempNode);
						}

					}
				}

			}
			node = node->NextSibling();
		}

	}
}
/*************************************************************************************************************************
Name: booleanLicenseParser
Description: find concluded license for passed XMLNode
Input:	tempXMLNode - pointer to XMLNode to find license information for
Output:	returns license information as a string
*************************************************************************************************************************/
void booleanLicenseParser(XMLNode * xmlNode, std::string * license){
	//This needs to be recursive so that it comes OUT of the inner traverses easier
	static std::stack<const char*> andOrStack;
	static bool addAPlusSign = false;
	static std::string exception = "";
	if(xmlNode != nullptr)
	{

	}
	//first find concluded/declared License
	if(xmlNode == nullptr)
	{
		if(andOrStack.size() > 0)
		{
			std::string check =	(std::string)andOrStack.top();
			andOrStack.pop();
			*license = license->substr(0, license->size()-3);
			if(check != ORLATER_OPERATOR && check != WITH_EXCEPTION)
			{
				license->append("\n)");
			}
		}
		return;
	}
	else if(xmlNode->FirstChildElement(LICENSE_CONCLUDED) != nullptr)
	{
		booleanLicenseParser(xmlNode->FirstChildElement(LICENSE_CONCLUDED), license);
	}
	else if(xmlNode->FirstChildElement(LICENSE_DECLARED) != nullptr)
	{
		booleanLicenseParser(xmlNode->FirstChildElement(LICENSE_DECLARED), license);
	}
	if(xmlNode->FirstChild() == nullptr)
	{

		license->append(getStringFromXMLNode(
							traverseToSpecificXMLNode(
								findXMLNodeWithAttribute(root, ATTRIBUTE_ABOUT,
										xmlNode->ToElement()->Attribute(ATTRIBUTE_RESOURCE)),
								1,
								&LICENSE_ID
								)
							)
						);

		if(addAPlusSign)
		{
			addAPlusSign = false;
			license->append("+");
		}
		if(exception != "")
		{
			license->append(" WITH " + exception);
			exception = "";
		}

	}
	else if(xmlNode->FirstChildElement(SPDX_LICENSE) != nullptr)
	{
		license->append(getStringFromXMLNode(
							traverseToSpecificXMLNode(
								findXMLNodeWithAttribute(root, ATTRIBUTE_ABOUT,
										xmlNode->FirstChildElement(SPDX_LICENSE)->ToElement()->Attribute(ATTRIBUTE_ABOUT)),
								1,
								&LICENSE_ID
								)
							)
						);

		if(addAPlusSign)
		{
			addAPlusSign = false;
			license->append("+");
		}
		if(exception != "")
		{
			license->append(" WITH " + exception);
			exception = "";
		}

	}
	else if(xmlNode->FirstChildElement(EXTRACTED_LICENSING_INFO) != nullptr)
	{
		license->append(getStringFromXMLNode(
							traverseToSpecificXMLNode(
								findXMLNodeWithAttribute(root, ATTRIBUTE_ABOUT,
										xmlNode->FirstChildElement(EXTRACTED_LICENSING_INFO)->ToElement()->Attribute(ATTRIBUTE_ABOUT)),
								1,
								&LICENSE_ID
								)
							)
						);

		if(addAPlusSign)
		{
			addAPlusSign = false;
			license->append("+");
		}
		if(exception != "")
		{
			license->append(" WITH " + exception);
			exception = "";
		}
	}
	else if(xmlNode->FirstChildElement(WITH_EXCEPTION) != nullptr)
	{
		const char* tempList[] = {WITH_EXCEPTION, LICENSE_EXCEPTION, SPDX_LICENSE_EXCEPTION, LICENSE_EXCEPTION_ID};
		andOrStack.push(WITH_EXCEPTION);
		exception = getStringFromXMLNode(
						traverseToSpecificXMLNode(
								xmlNode,
								4,
								tempList
								));
		booleanLicenseParser(xmlNode->FirstChildElement(WITH_EXCEPTION)->FirstChildElement(MEMBER), license);
	}
	else if(xmlNode->FirstChildElement(ORLATER_OPERATOR) !=  nullptr)
	{
		addAPlusSign = true;
		andOrStack.push(ORLATER_OPERATOR);
		booleanLicenseParser(xmlNode->FirstChildElement(ORLATER_OPERATOR)->FirstChildElement(MEMBER), license);
	}

	else if(xmlNode->FirstChildElement(CONJUNCTIVE_LICENSE_SET) != nullptr)
	{
		if(xmlNode->FirstChildElement(CONJUNCTIVE_LICENSE_SET)->FirstChildElement(MEMBER) != nullptr)
		{
			license->append("(\n");
			andOrStack.push(CONJUNCTIVE_LICENSE_SET);
			booleanLicenseParser(xmlNode->FirstChildElement(CONJUNCTIVE_LICENSE_SET)->FirstChildElement(MEMBER), license);
		}
	}
	else if(xmlNode->FirstChildElement(DISJUNCTIVE_LICENSE_SET) != nullptr)
	{
		if(xmlNode->FirstChildElement(DISJUNCTIVE_LICENSE_SET)->FirstChildElement(MEMBER) != nullptr)
		{
			license->append("(\n");
			andOrStack.push(DISJUNCTIVE_LICENSE_SET);

			booleanLicenseParser(xmlNode->FirstChildElement(DISJUNCTIVE_LICENSE_SET)->FirstChildElement(MEMBER), license);
		}
	}
	if(xmlNode != nullptr && andOrStack.size() > 0)
	{
		if(andOrStack.top() == CONJUNCTIVE_LICENSE_SET)
		{
			license->append("\n,");
		}
		else
		{
			license->append("\n/");
		}
		license->append("\n");
	}
	if(((std::string)xmlNode->Value()).compare(MEMBER) == 0)
	{
		booleanLicenseParser(xmlNode->NextSibling(), license);
	}


}



/*************************************************************************************************************************
Name: licenseFinder
Description: find concluded license for passed XMLNode. Starting point is best if at LicenseConcluded or LicenseDeclared
Input:	tempXMLNode - pointer to XMLNode to find license information for
Output:	returns license information as a string
*************************************************************************************************************************/
std::string licenseFinder(XMLNode * tempXMLNode){
	std::string license;
	const char* licensehttp = NULL;
	bool commas = false;
	std::stack<const char*> andOrStack;
	while(tempXMLNode != nullptr)
	{
		//first if-else chain to identify the licensehttp
		if(tempXMLNode->FirstChild() == nullptr)
		{
			licensehttp = tempXMLNode->ToElement()->Attribute(ATTRIBUTE_RESOURCE);
		}
		else if(tempXMLNode->FirstChildElement(SPDX_LICENSE) != nullptr)
		{
			licensehttp = tempXMLNode->FirstChildElement(SPDX_LICENSE)->ToElement()->Attribute(ATTRIBUTE_ABOUT);
		}
		else if(tempXMLNode->FirstChildElement(EXTRACTED_LICENSING_INFO) != nullptr)
		{
			licensehttp = tempXMLNode->FirstChildElement(EXTRACTED_LICENSING_INFO)->ToElement()->Attribute(ATTRIBUTE_ABOUT);
		}
		//incase there isn't a disjunctive license set
		else if(tempXMLNode->FirstChildElement(CONJUNCTIVE_LICENSE_SET)->FirstChildElement(MEMBER) != nullptr)
		{
			tempXMLNode = tempXMLNode->FirstChildElement(CONJUNCTIVE_LICENSE_SET)->FirstChildElement(MEMBER);
			license.append("(\n");
			commas = true;
		}
		else if(tempXMLNode->FirstChildElement(DISJUNCTIVE_LICENSE_SET)->FirstChildElement(MEMBER) != nullptr)
		{
			tempXMLNode = tempXMLNode->FirstChildElement(DISJUNCTIVE_LICENSE_SET)->FirstChildElement(MEMBER);
			license.append("(\n");
			commas = false;
		}
		else
		{
			licensehttp = NULL;
		}

		//create the license string
		if(licensehttp)
		{
			license.append(	getStringFromXMLNode(
								traverseToSpecificXMLNode(
									findXMLNodeWithAttribute(root, ATTRIBUTE_ABOUT, licensehttp),
									1,
									&LICENSE_ID
									)
								)
							);
		}
		//license information not found on this pass

		//determine whether to look for more licenses or not
		if(((std::string)tempXMLNode->Value()).compare(MEMBER) == 0)
		{
			tempXMLNode = tempXMLNode->NextSibling();
		}
		else break;
		if(tempXMLNode != nullptr)
		{
			if(commas)
			{
				license.append("\n,");
			}
			else
			{
				license.append("\n/");
			}
			license.append("\n");
			continue;
		}
		else break;
	}
	return license;
}

/************************************************************************************************************************************
Name: createNodeFromPackage
Description: create a pointer to a Node object from a <spdx:Package> class, inserts the Node* into the graph, and finds all its
			 edges. Also changes the causes the xmlnode pointer passed to point to its sibling.
Input: XMLNode** node -	Address of a pointer to a XMLNode whose descendants have the information needed to make a Node.
Output:	returns a pointer to the Node that was created.
************************************************************************************************************************************/
Node * createNodeFromPackage(XMLNode ** node, std::set<Node*,comp> & graph){
	//node to be made
	Node * newNode = new Node();
	std::pair<std::set<Node*,comp>::iterator, bool> insertCheck;

	//string members to be added to node
	std::string name = "";
	std::string throwaway = "";
	std::string  * license = &throwaway;
	std::string aboutId = "";

	//get aboutId
	aboutId.append((*node)->ToElement()->Attribute(ATTRIBUTE_ABOUT));


	//get name values, and append
	name.append(getStringFromXMLNode(traverseToSpecificXMLNode(*node, 1, &NAME)));
	name.append(" [version: ");
	name.append(getStringFromXMLNode(traverseToSpecificXMLNode(*node, 1, &VERSION_INFO)));
	name.append("]");


	//get license values

	booleanLicenseParser(*node, license);
	/*const char* tempList [] = {LICENSE_CONCLUDED, DISJUNCTIVE_LICENSE_SET, MEMBER};
	if(traverseToSpecificXMLNode(*node, 3, tempList) == *node)
	{
		tempList[0] = LICENSE_DECLARED;
		booleanLicenseParser(traverseToSpecificXMLNode(*node, 3, tempList), license);
	}
	else
	{
		booleanLicenseParser(traverseToSpecificXMLNode(*node, 3, tempList), license);
	}*/

	//insert node
	newNode->aboutId = aboutId;
	newNode->Name = name;
	std::size_t tempPos = 0;

	while(tempPos!=std::string::npos)
	{
		newNode->License.push_back(license->substr(tempPos, license->find("\n", tempPos) - tempPos));
		tempPos = license->find("\n", tempPos);
		if(tempPos != std::string::npos)
			tempPos++;
	}

	insertCheck = graph.insert(newNode);
	if(!insertCheck.second)
	{
		newNode = *(insertCheck.first);
	}


	//Find all edges.
	XMLNode * temp = traverseToSpecificXMLNode(*node, 1, &HAS_FILE);
	XMLNode * temp2;
	while(temp != nullptr)
	{
		if(((std::string)(temp)->Value()).compare(HAS_FILE) == 0)
		{
			newNode->Relationships.push_back(EDGE_HAS_FILE);
			if(temp->FirstChild() != nullptr)
			{
				temp2 = temp->FirstChild();
				newNode->adjacencyList.push_back(createNodeFromFile(&temp2, graph));
			}
			else
			{
				std::string about = temp->ToElement()->Attribute(ATTRIBUTE_RESOURCE);
				for(std::set<Node*>::iterator it = graph.begin(); it != graph.end() ; it++)
				{
					if((*it)->aboutId.compare(about)==0)
					{
						newNode->adjacencyList.push_back(*it);
					}
				}
			}
		}
		temp = temp->NextSibling();
	}

	relationshipFinder(*node, newNode, graph);

	*node = (*node)->NextSibling();

	return newNode;

}

/*************************************************************************************************************************
Name: createNodeFromPackage
Description: create a pointer to a Node object from a <spdx:File> class, inserts the Node* into the graph, and finds all its
			 edges. Also changes the causes the xmlnode pointer passed to point to its sibling.
Input: XMLNode** node -	Address of a pointer to a XMLNode whose descendants have the information needed to make a Node.
Output:	returns a pointer to the Node that was created.
*************************************************************************************************************************/
Node * createNodeFromFile(XMLNode ** node, std::set<Node*,comp> & graph){
	//the new Node object
	Node * newNode = new Node();
	std::pair<std::set<Node*,comp>::iterator, bool> insertCheck;

	//strings to become members of object
	std::string name = "";
	std::string throwaway = "";
	std::string  * license = &throwaway;

	std::string aboutId = "";

	//find aboutId
	aboutId.append((*node)->ToElement()->Attribute(ATTRIBUTE_ABOUT));

	//find the name
	name.append(getStringFromXMLNode(
					traverseToSpecificXMLNode(
								*node,
								1,
								&FILE_NAME)
					)
				);
	//go with concluded license, otherwise go with declared license

	booleanLicenseParser(*node, license);
	/*const char* tempList [] = {LICENSE_CONCLUDED, DISJUNCTIVE_LICENSE_SET, MEMBER};
	if(traverseToSpecificXMLNode(*node, 3, tempList) == *node)
	{
		tempList[0] = LICENSE_DECLARED;
		booleanLicenseParser(traverseToSpecificXMLNode(*node, 3, tempList), license);
	}
	else
	{
		booleanLicenseParser(traverseToSpecificXMLNode(*node, 3, tempList), license);
	}*/

	newNode->aboutId = aboutId;
	newNode->Name = name;
	std::size_t tempPos = 0;

	while(tempPos!=std::string::npos)
	{
		newNode->License.push_back(license->substr(tempPos, license->find("\n", tempPos) - tempPos));
		tempPos = license->find("\n", tempPos);
		if(tempPos != std::string::npos)
			tempPos++;
	}

	insertCheck = graph.insert(newNode);
	if(!insertCheck.second)
	{
		newNode = *(insertCheck.first);
	}
	relationshipFinder(*node, newNode, graph);


	//file dependency check
	XMLNode * temp = traverseToSpecificXMLNode(*node, 1, &FILE_DEPENDENCY);
	XMLNode * temp2;
	while(temp != nullptr)
	{
		if(((std::string)(temp)->Value()).compare(FILE_DEPENDENCY) == 0)
		{
			newNode->Relationships.push_back(EDGE_DEPENDENCY);
			if(temp->FirstChild() != nullptr)
			{
				temp2 = temp->FirstChild();
				newNode->adjacencyList.push_back(createNodeFromFile(&temp2, graph));
			}
			else
			{
				std::string about = temp->ToElement()->Attribute(ATTRIBUTE_RESOURCE);
				for(std::set<Node*>::iterator it = graph.begin(); it != graph.end() ; it++)
				{
					if((*it)->aboutId.compare(about)==0)
					{
						newNode->adjacencyList.push_back(*it);
					}
				}
			}
		}
		temp = temp->NextSibling();
	}


	*node = (*node)->NextSibling();

	return newNode;
}

/*************************************************************************************************************************
Name: createNodeFromPackage
Description: create a pointer to a Node object from a <spdx:Document> class, inserts the Node* into the graph, and finds all its
			 edges. Also changes the causes the xmlnode pointer passed to point to its sibling.
Input: XMLNode** node -	Address of a pointer to a XMLNode whose descendants have the information needed to make a Node.
Output:	returns a pointer to the Node that was created.
*************************************************************************************************************************/
Node * createNodeFromDocument(XMLNode ** node, std::set<Node*,comp> & graph){
	Node* newNode = new Node();
	std::pair<std::set<Node*,comp>::iterator, bool> insertCheck;

	//strings to become members of object
	std::string name = "";
	std::string throwaway = "";
	std::string  * license = &throwaway;
	std::string aboutId = "";

	//find aboutId
	aboutId.append((*node)->ToElement()->Attribute(ATTRIBUTE_ABOUT));

	name.append(getStringFromXMLNode(traverseToSpecificXMLNode(*node, 1, &NAME)));

	booleanLicenseParser(traverseToSpecificXMLNode(*node, 1, &DATA_LICENSE), license);

	newNode->Name = name;
	std::size_t tempPos = 0;

	while(tempPos!=std::string::npos)
	{
		newNode->License.push_back(license->substr(tempPos, license->find("\n", tempPos) - tempPos));
		tempPos = license->find("\n", tempPos);
		if(tempPos != std::string::npos)
			tempPos++;
	}

	newNode->aboutId = aboutId;
	insertCheck = graph.insert(newNode);
	if(!insertCheck.second)
	{
		newNode = *(insertCheck.first);
	}

	//find edges
	relationshipFinder(*node, newNode, graph);


	*node = (*node)->NextSibling();

	return newNode;
}

/************************************************************************************************************************************
 * Input: node; XMLNode you wish to traverse from
 * 	 	  target; string name of the node you want to find
 *
 * Output: traverses, if you want something to happen during the traversal, change it.
************************************************************************************************************************************/
void traverseXML(XMLNode * node, std::set<Node*,comp> & graph)
{
	if(node == nullptr)
	{
		return;
	}
	else if( ((std::string)(node->Value())).compare(SPDX_PACKAGE) == 0)
	{
		createNodeFromPackage(&node, graph);
	}
	else if ( ((std::string)(node->Value())).compare(SPDX_FILE) == 0)
	{
		createNodeFromFile(&node, graph);
	}
	else if ( ((std::string)(node->Value())).compare(SPDX_DOCUMENT) == 0)
	{
		createNodeFromDocument(&node, graph);
	}

	if(node != nullptr)
	{
		traverseXML(node->FirstChild(), graph);
		traverseXML(node->NextSibling(), graph);
	}
}


/*************************************************************************************************************************
Name: parseFile
Description: initializes XMLDocument from a passed file path, initializes the root, and traverses the xml.
Input: const char* file	- path to input file
Output: if the file is loaded incorrectly an error message is stated,
		otherwise there is no output.
*************************************************************************************************************************/
void parseFile(const char* file, std::set<Node*,comp> & graph){
	//load our document
	XMLDocument doc2;
	doc2.LoadFile(file);
	std::cout << file << std::endl;
	root = doc2.FirstChild();

	//check if file loaded correctly
	if(doc2.ErrorID() != 0)
	{

	}
	if(root == nullptr)
	{

		return;
	}


	//traverse the file, in search of "spdx:Package"    (currently a linear search)
	traverseXML(root, graph);

}



