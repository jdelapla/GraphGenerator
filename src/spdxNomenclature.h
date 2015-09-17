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
 * spdxNodeNames.hpp
 *
 *  Created on: Jun 25, 2015
 *      Author: jdelapla
 */

#ifndef SPDXNOMENCLATURE_H_
#define SPDXNOMENCLATURE_H_


//SPDX 2.0

//Attribute Names
const char* ATTRIBUTE_ABOUT = "rdf:about";
const char* ATTRIBUTE_RESOURCE = "rdf:resource";


//XMLNode Names
const char* DATA_LICENSE = "spdx:dataLicense";
const char* CONJUNCTIVE_LICENSE_SET = "spdx:ConjunctiveLicenseSet";
const char* DISJUNCTIVE_LICENSE_SET = "spdx:DisjunctiveLicenseSet";
const char* EXTRACTED_LICENSING_INFO = "spdx:ExtractedLicensingInfo";
const char* FILE_DEPENDENCY = "spdx:fileDependency";
const char* FILE_NAME = "spdx:fileName";
const char* HAS_FILE = "spdx:hasFile";
const char* LICENSE_CONCLUDED = "spdx:licenseConcluded";
const char* LICENSE_DECLARED = "spdx:licenseDeclared";
const char* LICENSE_EXCEPTION = "spdx:licenseException";
const char* LICENSE_EXCEPTION_ID = "spdx:licenseExceptionId";
const char* LICENSE_ID = "spdx:licenseId";
const char* MEMBER = "spdx:member";
const char* NAME = "spdx:name";
const char* ORLATER_OPERATOR = "spdx:OrLaterOperator";
const char* RELATED_SPDX_ELEMENT = "spdx:relatedSpdxElement";
const char* RELATIONSHIP = "spdx:Relationship";
const char* RELATIONSHIP_TYPE = "spdx:relationshipType";
const char* SPDX_FILE = "spdx:File";
const char* SPDX_DOCUMENT = "spdx:SpdxDocument";
const char* SPDX_ELEMENT = "spdx:SpdxElement";
const char* SPDX_LICENSE = "spdx:License";
const char* SPDX_LICENSE_EXCEPTION = "spdx:LicenseException";
const char* SPDX_PACKAGE = "spdx:Package";
const char* SPDX_RELATIONSHIP = "spdx:relationship";
const char* VERSION_INFO = "spdx:versionInfo";
const char* WITH_EXCEPTION = "spdx:WithExceptionOperator";

//My relationships
const char* EDGE_HAS_FILE = "has file";
const char* EDGE_DEPENDENCY = "file dependency";






#endif /* SPDXNOMENCLATURE_H_ */
