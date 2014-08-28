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
#include "acdisp.h"

CActionDispatcher::CActionDispatcher(void)
{
}

CActionDispatcher::~CActionDispatcher(void)
{

}

void CActionDispatcher::RegisterAction (std::wstring strAct, PFNBTNPROC pfnAct)
{
	m_dispTable.insert(DISPATCH_TABLE_ENTRY(strAct, pfnAct));
}


void CActionDispatcher::ExecuteAction (std::wstring strAct, void* param)
{
	DISPATCH_TABLE::iterator it = m_dispTable.find(strAct);
	if (it != m_dispTable.end())
		it->second(param);		// call ptr to function
}
