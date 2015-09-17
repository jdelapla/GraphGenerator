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
 * graphConfiguration.hpp
 *
 *  Created on: Jul 29, 2015
 *      Author: jdelapla
 */

#ifndef GRAPHCONFIGURATION_H_
#define GRAPHCONFIGURATION_H_

#include "BoolComp.h"
#include "Node.h"
#include <set>
#include <map>
#include <fstream>
#include <iostream>
#include <sstream>



void getColor(Node* node, std::map<std::string, std::string> & licenseTree, std::vector<std::string> & dominanceList);
std::string getRelationship(std::string connection, std::map<std::string, std::string> & relationshipTree);
void produceLicenseConfigTree(std::map<std::string, std::string> & licenseTree, std::string path);
void produceRelationshipConfigTree(std::map<std::string, std::string> & relationshipTree, std::string path);
void produceDominanceList(std::vector<std::string> & dominanceList, std::string path);
void removeDuplicateEdges(Node * node, Node * toNode, unsigned int skip);
void removeSelfEdges(std::set<Node*,comp> & graph);
void removeIdenticalEdges(std::set<Node*, comp> & graph);
void removeDoubleEdges(std::set<Node*,comp> & graph);
void establishContains(std::set<Node*,comp> & graph);
void removeUnattachedNodes(std::set<Node*,comp> & graph);
void generateBooleanFormula(Node * node);
BoolComp * findDominantLicense(Node * node, BoolComp * equation, std::vector<std::string> & dominanceList);
bool parentCheck(Node * node1, Node * node2);
bool licenseCheck(Node * node, Node * secondNode);
Node* makeACopyNode(Node * node, Node * container);
void simplifySiblingNodes(Node * nodeIt);
bool collapseACheck(Node * collapser, Node * collapsee, unsigned int index);
bool collapsePCheck(Node * collapser, Node * collapsee, unsigned int index);
void collapseSinkHole(Node * collapser, Node * collapsee, Node * toSkip, bool origin);
void edgeCount(std::set<Node*,comp> & graph);
void collapseNodes(std::set<Node*, comp> & graph);
void configGraph(std::set<Node*,comp> & graph, std::string path);



#endif /* GRAPHCONFIGURATION_HPP_ */
