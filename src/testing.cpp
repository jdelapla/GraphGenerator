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
//============================================================================
// Name        : testing.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
*/
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <set>
#include "Node.h"
#include "dotFileMaker.h"
#include "graphConfiguration.h"
#include "noCollapseConfig.h"

std::set<Node*,comp> graph;

void dummyTest(){
	static int count = 0;
	count++;


		if(count > 10)
		{
			return;
		}
		std::cout << count << std::endl;
		dummyTest();
}


int main( int argc, char *  const argv[]){
	std::string names = "node";
	std::string tempNames = "randomNode";

	int count = 0;
	std::stringstream stream;

	Node * newNode;
	Node * tempNode;
	Node * firstNode;

	std::cout << "Welcome to the testing module! I am capable of a limited number of tests, please select one of the following:"
				<< "\n\tChoose by number: \n\t(1)basicStatic \n\t(2)altContains \n\t(3)oneMillionNodes (cannot display uncollapsed)\n\t(4)copyContains \n\t(5)seriesofContains"
				<< "\n\t(6)oddNumberofNodes\n\t(7)A-BBB-AalternatingStringOfStaticLinks\n\t(8)ABCalternatingStringOfStaticLinks"
				<< "\n\t(9)A-BB-AalternatingStringOfContains\n\t(10)emptyTest"
				<< "\n if you enter non-integer input this program will divide the world by 0" <<  std::endl;
	std::string input = "";
	std::cin >> input;
	std::string::size_type sz;
	int convert = 0;
	bool checker = true;
	while(1)
	{
		for(unsigned int i =0; i < input.size(); i++)
		{
			if(input[i] > 57 || input[i] < 48)
			{
				checker = false;
			}
		}
		if(checker)
		{
			convert = std::stoi(input, &sz);
			newNode = nullptr;
			tempNode = nullptr;
			firstNode = nullptr;
			count = 0;
			graph.clear();
			switch(convert)
			{
				case 0:
				{
					break;
				}
				case 1:
				{
					for(int cycle = 0;cycle < 2; cycle++)
					{
						count = 0;
						for(int i = 0; i < 10; i++)
						{
							tempNode = newNode;
							newNode = new Node();
							if(i > 0)
							{
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_staticLink");
								newNode->adjacencyList.push_back(tempNode);
							}
							stream << count;
							newNode->Name = names + stream.str();
							stream.str(std::string());
							count++;
							newNode->License.push_back("LGPL-2.0");
							graph.insert(newNode);
						}
						if(cycle == 1)
						{
							configGraph(graph);
							processGraph(graph, "test1Collapsed.dot");
							system("dot -Tpng  test1Collapsed.dot -o test1Collapsed.png");
						}
						else
						{
							configGraphNoCollapse(graph);
							processGraph(graph, "test1Uncollapsed.dot");
							system("dot -Tpng test1Uncollapsed.dot -o test1Uncollapsed.png");
						}
						graph.clear();
					}
					break;
				}
				case 2:
				{
					for(int cycle = 0; cycle < 2; cycle++)
					{
						count = 0;
						for(int i = 0; i < 23; i++)
						{
							tempNode = newNode;
							newNode = new Node();
							if(i > 0)
							{
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_staticLink");
								newNode->adjacencyList.push_back(tempNode);
							}
							stream << count;
							newNode->Name = names + stream.str();
							stream.str(std::string());
							count++;

							if(i == 6)
							{
								*(tempNode->License.begin()) = "Apache-1.1";
								newNode->adjacencyList.push_back(tempNode->adjacencyList[0]);
								newNode->Relationships.push_back(tempNode->Relationships[0]);
								tempNode->adjacencyList.clear();
								tempNode->Relationships.clear();
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_contains");
								newNode->adjacencyList.push_back(tempNode);
							}
							if(i == 3)
							{
								*(tempNode->License.begin()) = "Apache-2.0";
								newNode->adjacencyList.push_back(tempNode->adjacencyList[0]);
								newNode->Relationships.push_back(tempNode->Relationships[0]);
								tempNode->adjacencyList.clear();
								tempNode->Relationships.clear();
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_contains");
								newNode->adjacencyList.push_back(tempNode);
							}
							if(i == 18)
							{
								*(tempNode->License.begin()) = "Apache-2.0";
								newNode->adjacencyList.push_back(tempNode->adjacencyList[0]);
								newNode->Relationships.push_back(tempNode->Relationships[0]);
								tempNode->adjacencyList.clear();
								tempNode->Relationships.clear();
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_contains");
								newNode->adjacencyList.push_back(tempNode);
							}
							if(i == 12)
							{
								*(tempNode->License.begin()) = "asfgauisgsak";
								newNode->adjacencyList.push_back(tempNode->adjacencyList[0]);
								newNode->Relationships.push_back(tempNode->Relationships[0]);
								tempNode->adjacencyList.clear();
								tempNode->Relationships.clear();
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_contains");
								newNode->adjacencyList.push_back(tempNode);
							}


							newNode->License.push_back("LGPL-2.0");
							graph.insert(newNode);
						}
						if(cycle)
						{
							configGraph(graph);
							processGraph(graph, "test2Collapsed.dot");
							system("dot -Tpng test2Collapsed.dot -o test2Collapsed.png");
						}
						else
						{
							configGraphNoCollapse(graph);
							processGraph(graph, "test2Uncollapsed.dot");
							system("dot -Tpng test2Uncollapsed.dot -o test2Uncollapsed.png");
						}
						graph.clear();
					}
					break;
				}
				case 3:
				{
					count = 0;
					for(int i = 0; i < 1000000; i++)
					{
						tempNode = newNode;
						newNode = new Node();
						if(i > 0)
						{
							newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_staticLink");
							newNode->adjacencyList.push_back(tempNode);
						}
						stream << count;
						newNode->Name = names + stream.str();
						stream.str(std::string());
						count++;
						newNode->License.push_back("LGPL-2.0");
						graph.insert(newNode);
					}

					configGraph(graph);
					processGraph(graph, "test3Collapsed.dot");
					system("dot -Tpng test3Collapsed.dot -o test3Collapsed.png");
					graph.clear();

					break;
				}
				case 4:
				{
					for(int cycle = 0; cycle < 2; cycle++)
					{
						count = 0;
						for(int i = 0; i < 10; i++)
						{
							tempNode = newNode;
							newNode = new Node();
							stream << count;
							newNode->Name = names + stream.str();
							stream.str(std::string());
							count++;
							newNode->License.push_back("LGPL-2.0");
							if(i > 0)
							{
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_contains");
								newNode->adjacencyList.push_back(firstNode);
								if(i > 1)
								{
									newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_staticLink");
									newNode->adjacencyList.push_back(tempNode);
								}
							}
							else
							{
								firstNode = newNode;
								firstNode->License[0] = "Apache-1.1";
							}
							graph.insert(newNode);
						}

						if(cycle == 1)
						{
							configGraph(graph);
							processGraph(graph, "test4Collapsed.dot");
							system("dot -Tpng test4Collapsed.dot -o test4Collapsed.png");
						}
						else
						{
							configGraphNoCollapse(graph);
							processGraph(graph, "test4Uncollapsed.dot");
							system("dot -Tpng test4Uncollapsed.dot -o test4Uncollapsed.png");
						}

						graph.clear();
					}
					break;
				}
				case 5:
				{
					std::vector<Node*> list;
					for(int cycle = 0; cycle < 2; cycle++)
					{
						list.clear();
						int switcher = 0;
						count = 0;
						for(int i = 0; i < 7; i++)
						{
							newNode = new Node();
							if(i > 0)
							{
								for(int j = i-1; j != -1; j--)
								{
									newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_contains");
									newNode->adjacencyList.push_back(list[j]);
								}
							}
							if(i%2 == 0)
							{
								Node * tempNode = new Node();
								stream << count;
								tempNode->Name = tempNames + stream.str();
								stream.str(std::string());
								tempNode->License.push_back("Apache-1.1");
								newNode->adjacencyList.push_back(tempNode);
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_contains");
								graph.insert(tempNode);
							}

							stream << count;
							newNode->Name = names + stream.str();
							stream.str(std::string());
							count++;
							if(switcher == 0)
							{
								newNode->License.push_back("LGPL-2.0");
								switcher++;
							}
							else if(switcher == 1)
							{
								newNode->License.push_back("Apache-2.0");
								switcher++;
							}
							else if(switcher == 2)
							{
								newNode->License.push_back("Apache-2.0");
								switcher=0;
							}
							list.push_back(newNode);
							graph.insert(newNode);
						}

						if(cycle)
						{
							configGraph(graph);
							processGraph(graph, "test5Collapsed.dot");
							system("dot -Tpng test5Collapsed.dot -o test5Collapsed.png");
						}
						else
						{
							configGraphNoCollapse(graph);
							processGraph(graph, "test5Uncollapsed.dot");
							system("dot -Tpng test5Uncollapsed.dot -o test5Uncollapsed.png");
						}

						graph.clear();
					}
					break;
				}
				case 6:
				{
					for(int cycle = 0; cycle < 2; cycle++)
					{
						count = 0;
						for(int i = 0; i < 11; i++)
						{
							tempNode = newNode;
							newNode = new Node();
							if(i > 0)
							{
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_staticLink");
								newNode->adjacencyList.push_back(tempNode);
							}
							stream << count;
							newNode->Name = names + stream.str();
							stream.str(std::string());
							count++;
							newNode->License.push_back("LGPL-2.0");
							graph.insert(newNode);
						}
						if(cycle)
						{
							configGraph(graph);
							processGraph(graph, "test6Collapsed.dot");
							system("dot -Tpng test6Collapsed.dot -o test6Collapsed.png");
						}
						else
						{
							configGraphNoCollapse(graph);
							processGraph(graph, "test6Uncollapsed.dot");
							system("dot -Tpng test6Uncollapsed.dot -o test6Uncollapsed.png");
						}

						graph.clear();
					}
					break;
				}
				case 7:
				{
					for(int cycle = 0; cycle < 2; cycle++)
					{
						count = 0;
						for(int i = 0; i < 15; i++)
						{
							tempNode = newNode;
							newNode = new Node();
							if(i > 0)
							{
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_staticLink");
								newNode->adjacencyList.push_back(tempNode);
							}
							stream << count;
							newNode->Name = names + stream.str();
							stream.str(std::string());
							count++;
							if( i%4 == 0)
							{
								newNode->License.push_back("LGPL-2.0");
							}
							else
							{
								newNode->License.push_back("Apache-1.1");
							}


							graph.insert(newNode);
						}
						if(cycle)
						{
							configGraph(graph);
							processGraph(graph, "test7Collapsed.dot");
							system("dot -Tpng test7Collapsed.dot -o test7Collapsed.png");
						}
						else
						{
							configGraphNoCollapse(graph);
							processGraph(graph, "test7Uncollapsed.dot");
							system("dot -Tpng test7Uncollapsed.dot -o test7Uncollapsed.png");
						}

						graph.clear();
					}
					break;
				}
				case 8:
				{
					for(int cycle = 0; cycle < 2; cycle++)
					{
						count = 0;
						int switcher = 0;
						for(int i = 0; i < 10; i++)
						{
							tempNode = newNode;
							newNode = new Node();
							if(i > 0)
							{
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_staticLink");
								newNode->adjacencyList.push_back(tempNode);
							}
							stream << count;
							newNode->Name = names + stream.str();
							stream.str(std::string());
							count++;
							if(switcher == 0)
							{
								newNode->License.push_back("LGPL-2.0");
								switcher++;
							}
							else if(switcher == 1)
							{
								newNode->License.push_back("Apache-1.1");
								switcher++;
							}
							else if(switcher == 2)
							{
								newNode->License.push_back("Apache-2.0");
								switcher = 0;
							}
							graph.insert(newNode);
						}
						if(cycle)
						{
							configGraph(graph);
							processGraph(graph, "test8Collapsed.dot");
							system("dot -Tpng test8Collapsed.dot -o test8Collapsed.png");
						}
						else
						{
							configGraphNoCollapse(graph);
							processGraph(graph, "test8Uncollapsed.dot");
							system("dot -Tpng test8Uncollapsed.dot -o test8Uncollapsed.png");
						}

						if(cycle)
						{

						}
						else
						{

						}
						graph.clear();
					}
					break;
				}
				case 9:
				{
					for(int cycle = 0; cycle < 2; cycle++)
					{
						count = 0;
						int switcher = 0;
						for(int i = 0; i < 15; i++)
						{
							tempNode = newNode;
							newNode = new Node();
							if(i > 0)
							{
								newNode->Relationships.push_back("http://spdx.org/rdf/terms#relationshipType_contains");
								newNode->adjacencyList.push_back(tempNode);
							}
							if(switcher == 0)
							{
								newNode->License.push_back("LGPL-2.0");
								switcher++;
							}
							else if(switcher == 1)
							{
								newNode->License.push_back("Apache-2.0");
								switcher++;
							}
							else if(switcher == 2)
							{
								newNode->License.push_back("Apache-2.0");
								switcher=0;
							}

							stream << count;
							newNode->Name = names + stream.str();
							stream.str(std::string());
							count++;
							graph.insert(newNode);
						}

						if(cycle)
						{
							configGraph(graph);
							processGraph(graph, "test9Collapsed.dot");
							system("dot -Tpng test9Collapsed.dot -o test9Collapsed.png");
						}
						else
						{
							configGraphNoCollapse(graph);
							processGraph(graph, "test9Uncollapsed.dot");
							system("dot -Tpng test9Uncollapsed.dot -o test9Uncollapsed.png");
						}
						graph.clear();
					}
					break;
				}
				case 10:
				{
					configGraph(graph);
					processGraph(graph, "test10Collapsed.dot");
					system("dot -Tpng test10Collapsed.dot -o test10Collapsed.png");
					graph.clear();
					break;
				}
				default:{
					std::cout << "That was not one of the available choices, please try again" << std::endl;
					break;
				}
			}
		}
		else
		{
			std::cout << "Please enter an integer between 1-10" << std::endl;
		}
		for(std::set<Node*,comp>::iterator it = graph.begin(); it != graph.end(); it++)
		{
			delete (*it);
		}
		graph.clear();
		std::cout << "\n\tChoose by number: \n\t(1)basicStatic \n\t(2)altContains \n\t(3)oneMillionNodes \n\t(4)copyContains \n\t(5)seriesOfContains" <<
		"\n\t(6)oddNumberofNodes\n\t(7)A-BBB-AalternatingStringOfStaticLinks\n\t(8)ABCalternatingStringOfStaticLinks"
		<< "\n\t(9)A-BB-AalternatingStringOfContains\n\t(10)emptyTest" << std::endl;
		std::cin.sync();
		std::cin >> input;
	}

}
