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
 * Main.cpp
 *
 *  Created on: Jun 19, 2015
 *      Author: James Delaplane
 */



#include <iostream>
#include <stdio.h>
#include "Node.h"
#include "dotFileMaker.h"
#include "graphConfiguration.h"
#include "spdxParser.h"

#include <string>
#include <set>



/*************************************************************************************************************************

*************************************************************************************************************************/
int main(int argc, char * const argv[]){


	std::set<Node*, comp> tree;

	if(argc < 2)
	{
		std::cout << "Please enter the path of the file you wish the parse following this .exe:\n Example: gg C:\\Users\\Jo"
				<<	"hnSmith\\example.rdf" << std::endl;
	}
	std::string path = "";
	if(argc > 0)
	{
		path = argv[0];
		path = path.substr(0, path.find_last_of("/\\") + 1);
	}
	for(int iterator = 1; iterator < argc; iterator++)
	{
		parseFile(argv[iterator], tree);
	}


	//print statement output of the tree
	for(std::set<Node*>::iterator it = tree.begin();it != tree.end() ; it++ )
	{
		std::cout << (*it)->Name;
		std::cout << " with License(s): ";
		for(unsigned int i = 0; i < (*it)->License.size(); i++)
		{
			std::cout << (*it)->License[i] << " ";
		}
		std::cout << "| has " << (*it)->adjacencyList.size() << " edges: " << std::endl;
		if((*it)->Relationships.size() != 0)
		{
			for(unsigned int i = 0; i < (*it)->Relationships.size(); i++)
			{
				std::cout << "\t" << (*it)->Relationships[i] << std::endl;
				if(i < (*it)->adjacencyList.size())
				{
					std::cout << "\t" << "\t" << (*it)->adjacencyList[i]->Name << std::endl;
				}
			}
		}
		std::cout << std::endl;
	}

	std::set<Node*, comp> & treeReference = tree;
	configGraph(treeReference, path);
	processGraph(treeReference, "outputDot.dot", path);
	std::string command = "dot -Tpng " + path + "outputDot.dot -o " + path;
	command += "xmlGeneratorGraph.png";
	std::cout << command.c_str() << std::endl;
	system(command.c_str());

}



