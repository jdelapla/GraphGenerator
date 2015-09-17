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
 * dotFileMaker.cpp
 *
 *  Created on: Jun 30, 2015
 *      Author: jdelapla
 */
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "Node.h"
#include "graphConfiguration.h"

#include <string>
#include <set>
#include <map>
#include <sstream>

std::string edgeHandler(Node * node, std::string path);
void makeASubgraph(Node * node, std::string path);

void writeToDotFile(std::string outputName, std::string content);
void processGraph(std::set<Node*,comp> & graph, const char* outputFileName, std::string path);

//used so that all " -- " edges are placed after every subgraph has already been written
std::string remainingEdges = "";

/*************************************************************************************************************************
Name: edgeHandler
Description: identifies all edge relationships for a given node, and produces the necessary dot code
Input: node - pointer to node to identify edges for
Output: string of dot code
*************************************************************************************************************************/
std::string edgeHandler(Node * node, std::string path){
	std::string toBeContained = "";
	std::string forTheEnd = "";
	std::stringstream temp;
	temp.str(std::string());
	std::map<std::string, std::string> licenseList;
	produceLicenseConfigTree(licenseList, path);
	std::string dynColor = licenseList.find("dynamicLink")->second;
	std::string statColor = licenseList.find("staticLink")->second;
	//traverse all edges of the node
	for(unsigned int iterator = 0; iterator < node->Relationships.size(); iterator++)
	{
		if(node->adjacencyList[iterator]->Name == node->Name)
		{
			//no self edges
		}
		else if(node->Relationships[iterator] == "error")
		{
			//internal error
		}
		else if(node->Relationships[iterator] == "contain")
		{
			//for contains the node needs to already have its dot code created
			if(!node->adjacencyList[iterator]->visited)
			{
				node->adjacencyList[iterator]->visited = true;
				makeASubgraph(node->adjacencyList[iterator], path);
			}
			toBeContained += node->adjacencyList[iterator]->fileOutput;
			node->adjacencyList[iterator]->fileOutput = "";
			node->adjacencyList[iterator]->isContained++;

		}
		else if(node->Relationships[iterator] == "blank")
		{
			//skip this edge
		}
		else if(node->Relationships[iterator] == "static")
		{
			temp << *(node->pinvisNodeCount);
			forTheEnd += "invis" + node->dotNodeName + "x" + temp.str() +" -- invis" +
					node->adjacencyList[iterator]->dotNodeName;
			temp.str(std::string());
			temp << *(node->adjacencyList[iterator]->pinvisNodeCount);
			forTheEnd += "x" + temp.str() + " [xlabel = \"Static\\nLink\", ltail = " + node->dotNodeName + ", lhead = "
						+ node->adjacencyList[iterator]->dotNodeName + ", color = \"" + statColor + "\"]\n";
			temp.str(std::string());

			//update pointer to invisNodeCount in order to update the dot variables for each node.
			*(node->pinvisNodeCount)+= 1;
			*(node->adjacencyList[iterator]->pinvisNodeCount)+= 1;

			//also create the same edge for every copy
			for(unsigned int it2 = 0; it2 < node->adjacencyList[iterator]->Copies.size(); it2++)
			{
				temp << *(node->pinvisNodeCount);
				forTheEnd += "invis" + node->dotNodeName + "x" + temp.str() +" -- invis" +
						node->adjacencyList[iterator]->Copies[it2]->dotNodeName;
				temp.str(std::string());
				temp << *(node->adjacencyList[iterator]->Copies[it2]->pinvisNodeCount);
				forTheEnd += "x" + temp.str() + " [xlabel = \"Static\\nLink\", ltail = " + node->dotNodeName + ", lhead = "
							+ node->adjacencyList[iterator]->Copies[it2]->dotNodeName + ", color = \"" + statColor + "\"]\n";
				temp.str(std::string());
				*(node->pinvisNodeCount)+= 1;
				*(node->adjacencyList[iterator]->Copies[it2]->pinvisNodeCount)+= 1;
			}
		}
		else if(node->Relationships[iterator] == "dynamic")
		{
			temp << *(node->pinvisNodeCount);
			forTheEnd += "invis" + node->dotNodeName + "x" + temp.str() + " -- invis" +
					node->adjacencyList[iterator]->dotNodeName;
			temp.str(std::string());
			temp << node->adjacencyList[iterator]->invisNodeCount;
			forTheEnd += "x" + temp.str() + " [xlabel = \"Dynamic\\nLink\", ltail = " + node->dotNodeName + ", lhead = "
						+ node->adjacencyList[iterator]->dotNodeName + ", color = \"" + dynColor + "\"]\n";
			temp.str(std::string());

			//update pointer to invisNodeCount in order to update the dot variables for each node.
			*(node->pinvisNodeCount)+= 1;
			*(node->adjacencyList[iterator]->pinvisNodeCount)+= 1;

			//also create the same edge for every copy
			for(unsigned int it2 = 0; it2 < node->adjacencyList[iterator]->Copies.size(); it2++)
			{
				temp << *(node->pinvisNodeCount);
				forTheEnd += "invis" + node->dotNodeName + "x" + temp.str() +" -- invis" +
						node->adjacencyList[iterator]->Copies[it2]->dotNodeName;
				temp.str(std::string());
				temp << *(node->adjacencyList[iterator]->Copies[it2]->pinvisNodeCount);
				forTheEnd += "x" + temp.str() + " [xlabel = \"Dynamic\\nLink\", ltail = " + node->dotNodeName + ", lhead = "
							+ node->adjacencyList[iterator]->Copies[it2]->dotNodeName + ", color = \"" + dynColor + "\"]\n";
				temp.str(std::string());
				*(node->pinvisNodeCount)+= 1;
				*(node->adjacencyList[iterator]->Copies[it2]->pinvisNodeCount)+= 1;
			}
		}

	}
	//we want to place all "link" edges of every node at the END of the code.
	remainingEdges += forTheEnd;

	return toBeContained;
}
/*************************************************************************************************************************
Name: makeASubgraph
Description: produces a string of dot code from information in a Node.
Input: node - pointer to node to get information from
Output: stores string output in node->fileOutput
*************************************************************************************************************************/
void makeASubgraph(Node * node, std::string path){
	double sizeComp = 0.0;
	double width = 0.0;
	std::ostringstream widthStr;
	node->visited = true;
	std::string dotSubgraph = "subgraph " + node->dotNodeName + "{\n";
	dotSubgraph += "style = filled;";
	dotSubgraph += "fillcolor= \"" + node->Color + "\";";
	dotSubgraph += "label=<<TABLE BORDER = \"0\"><TR><TD ALIGN = \"LEFT\">" + node->Name + "</TD></TR>";
	if(node->aboutId.compare("I-have.Eaten-Other.Nodes") == 0)
	{
		int counter = 0;
		for(std::set<std::string>::iterator pass = node->hasCollapsed.begin(); pass != node->hasCollapsed.end(); pass++)
		{
			dotSubgraph += "<TR><TD ALIGN = \"LEFT\">" + (*pass) + "</TD></TR>";
			counter++;
			//decided I didn't want to show more than 10 names on a single node
			if(counter > 9)
			{
				dotSubgraph += "<TR><TD ALIGN = \"LEFT\">(etc.)</TD></TR>";
				break;
			}
		}
	}
	dotSubgraph += "<TR><TD ALIGN = \"LEFT\">";
	if(node->License.size() == 1)
	{
		dotSubgraph += "(";
	}
	for(unsigned int count =  0; count < node->License.size(); count++)
	{
		for(unsigned int it = 0; it < node->toUnderline.size(); it++)
		{
			if(node->License[count] == node->toUnderline[it])
			{
				dotSubgraph += "<U><B>";
			}
		}
		dotSubgraph += node->License[count];
		for(unsigned int it = 0; it < node->toUnderline.size(); it++)
		{
			if(node->License[count] == node->toUnderline[it])
			{
				dotSubgraph += "</B></U>";
			}
		}
		if(count != node->License.size()-1)
		{
			dotSubgraph += " ";
		}
		sizeComp += node->License[count].size() + 1;
	}
	if(node->License.size() == 0)
	{
		dotSubgraph += "No License Found";
	}
	if(sizeComp > node->Name.size())
	{
		width = sizeComp * 0.08; //0.08 is average size of a Times New Roman character in 12 point font
	}
	else
	{
		width = (node->Name.size())*0.08;
	}
	if(node->License.size() == 1)
	{
		dotSubgraph += ")";
	}
	dotSubgraph += "</TD></TR></TABLE>>;\n";
	dotSubgraph += "labeljust = l;\n";
	dotSubgraph += "labelloc = t;\n";
	dotSubgraph += edgeHandler(node, path);
	int cycles = 0;

	if(*(node->pedges) != 0)
	{
		width /= *(node->pedges);
		width -= 0.26;
		cycles = *(node->pedges);
	}
	else
	{
		cycles = 1;
	}


	widthStr << width;

	std::stringstream temp;
	for(int count = 0; count < cycles; count++)
	{
		temp.str(std::string());
		temp << (count);
		dotSubgraph += "\ninvis" + node->dotNodeName + "x" + temp.str() + " [label = \"\", style = invis, fixedsize=true, width =" + widthStr.str() + "];\n";
	}
	dotSubgraph += "}";

	node->fileOutput = dotSubgraph;
}



/*************************************************************************************************************************
Name: writeToDotFile
Description: writes a string to a file
Input: outputName - name of file to write to
	   content -	content to write to file
Output: File now contains additional content
*************************************************************************************************************************/
void writeToDotFile(std::string outputName, std::string content){
	std::ofstream myfile;
	myfile.open(outputName);
	myfile << content;
	myfile.close();
}
/*************************************************************************************************************************
Name: processGraph
Description: Acts as the "main" for all .dot file related functions. Modifies the relationships member of each Node* in
			 the passed std::set<Node*, comp> by using the relationshipList.txt config file, then produces a .dot file
			 output named after the passed const char* .

Input: graph	- graph to be processed
	   outputFileName -	name of output file

Output: produces a .dot file from the graph
*************************************************************************************************************************/
void processGraph(std::set<Node*,comp> & graph, const char* outputFileName, std::string path){
	std::string fileContents = "graph G{\nfontsize=20.0;\nratio=0.5625;\nautosize=false;\nresolution=100;\nbgcolor=\"transparent\";\nsplines=\"ortho\";\ncompound=true;\nedge[penwidth=4, minlen=3];\n";
	remainingEdges = "";




	//traversal to produce the fileOutput for each node
	for(std::set<Node*,comp>::iterator it = graph.begin(); it != graph.end(); it++)
	{
		if(!(*it)->visited)
		{
			makeASubgraph(*it, path);
		}
	}


	//combine all the fileOutput
	for(std::set<Node*,comp>::iterator it = graph.begin(); it != graph.end(); it++)
	{
		fileContents += (*it)->fileOutput;
	}
	path += outputFileName;
	fileContents += remainingEdges;
	fileContents += "\n}";
	writeToDotFile(path, fileContents);

}



