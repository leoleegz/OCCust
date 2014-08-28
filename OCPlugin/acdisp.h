/*****************************************************************************
Windows Live Messenger Plugin Demo
Copyright (C) 2008  Hernán Di Pietro

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

/*****************************************************************************/
#ifndef ACTION_DISPATCHER_H
#define ACTION_DISPATCHER_H

#include <map>
#include <string>

/* This class implements execution of actions based 
   on a MSAA button */

typedef void (*PFNBTNPROC) (void*);

class CActionDispatcher
{
	typedef std::pair<std::wstring, PFNBTNPROC> DISPATCH_TABLE_ENTRY;
	typedef std::map<std::wstring, PFNBTNPROC> DISPATCH_TABLE;

	DISPATCH_TABLE m_dispTable;

public:
	void RegisterAction(std::wstring, PFNBTNPROC);
	void ExecuteAction(std::wstring, void*);

	CActionDispatcher(void);
	~CActionDispatcher(void);
};

#endif //ACTION_DISPATCHER_H
