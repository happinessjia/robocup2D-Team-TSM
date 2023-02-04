/*
Copyright (c) 2002-2005, ZJUBase, Zhejiang University
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the Zhejiang University nor the names of its
contributors may be used to endorse or promote products derived from this
software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! \file Offlcient.h
<pre>
<b>File:</b>          Offclient.cpp
<b>Project:</b>       Robocup Soccer Simulation Team: ZJUBase
<b>Last Revision:</b> $ID$
<b>Contents:</b>      The OffClient class supports offline client debug.
<hr size=2>
</pre>
*/

#include "Offclient.h"
#include "Parse.h"

#include <stdio.h>
#include <stdarg.h>

Offclient offclient;

Offclient::Offclient() 
{
	run_offclient = false;
	log_offclient = false;
}

Offclient::~Offclient()
{
	if ( run_offclient || !log_offclient || !_outfile )
		return;
	
	_outfile.close();
}

void Offclient::Init(bool log_offclient, bool run_offclient, int offclient_number)
{
	this->log_offclient = log_offclient;
	this->run_offclient = run_offclient;
	this->offclient_number = offclient_number;
}

/*!
*	保存来自网络的消息, 并加以前缀, recv:
*
*\param: char *msg. 	来自网络的消息包
*/
void Offclient::SaveRecv(const char *msg)
{
	if ( run_offclient || !log_offclient )
		return;
	
	string s = "recv:";
	s += msg;
	s += "\n";

	if ( !strncmp(msg, "(init", 5 ) ){
		char *tmp_msg = const_cast<char*>(msg);
		int MyNumber = Parse::parseFirstInt(&tmp_msg);
		char tmp[20];
		sprintf(tmp, "offclient%d.log", MyNumber);
		_outfile.open(tmp);
	}

	if ( !_outfile ){
		return;
	}
	
	Save(s);
}

//------------------------------------------------------------------------
/*!
*	写文件
*
*\param: string s. 	最后写到文件的内容
*/
void Offclient::Save(const string& s)
{
	_outfile << s;	
}

