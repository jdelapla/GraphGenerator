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
 * BoolComp.cpp
 *
 *  Created on: Aug 25, 2015
 *      Author: jdelapla
 */

#include "BoolComp.h"


BoolComp::BoolComp() : parent(nullptr), value(""), boolType(""){

}

BoolComp::~BoolComp(){

}

bool BoolComp::compare(BoolComp * other){
	bool found = false;
	if(this->value == "" && other->value == "")
	{
		if(this->boolType.compare(other->boolType) == 0)
		{
			for(unsigned int firstIt = 0; firstIt < this->content.size(); firstIt++)
			{
				for(unsigned int secondIt = 0; secondIt < other->content.size(); secondIt++)
				{
					if(this->content[firstIt]->compare(other->content[secondIt]))
					{
						found = true;
					}
				}
				if(found)
				{
					found = false;
				}
				else
				{
					return false;
				}
			}
			return true;
		}

	}
	else if(this->value.compare(other->value) == 0)
	{
		return true;
	}
	return false;
}
