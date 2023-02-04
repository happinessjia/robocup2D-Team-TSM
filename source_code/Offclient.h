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
<b>File:</b>          Offclient.h
<b>Project:</b>       Robocup Soccer Simulation Team: ZJUBase
<b>Last Revision:</b> $ID$
<b>Contents:</b>      Header file for the OffClient class. It supports offline client debug.
<hr size=2>
</pre>
*/

#ifndef _OFFCLIENT_H_
#define _OFFCLIENT_H_
#include <string>
#include <fstream>
using namespace std;

class Offclient{
public:
	Offclient();
	~Offclient();
	void SaveRecv(const char *msg);
	void Init(bool log_offclient, bool run_offclient, int offclient_number);

public:	//configuration of offclient
	bool is_log_offclient(){
		return log_offclient;
	}

	bool is_run_offclient(){
		return run_offclient;
	}

	int get_offclient_number(){
		return offclient_number;
	}

	char* get_offclient_filename(){
	}
private:
	void Save(const string& s);

	bool log_offclient;
	bool run_offclient;

	int offclient_number;

	ofstream _outfile;
};

extern Offclient offclient;

#endif

