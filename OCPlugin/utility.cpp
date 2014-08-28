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

#include "utility.h"

void StringFromGUID(const GUID guid, WCHAR* wszBuffer)
{
#ifdef _DEBUG
	_ASSERTE(wszBuffer);

	swprintf_s(wszBuffer, MAXSTRL, L"%x%x%x%x%x%x%x%x%x%x%x", guid.Data1, guid.Data2, 
		guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], 
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
#endif			
}

void _OutputDebugString(LPWSTR lpOutputString, ... )
{    
#ifdef _DEBUG
	va_list argptr;        
	va_start( argptr, lpOutputString );            

	WCHAR OutMsg[MAXSTRL];
	WCHAR format[MAXSTRL];

	for(int i=0,j=0;lpOutputString[i] != '\0';i++) 
	{
		format[j++] = lpOutputString[i];
		// If escape character
		if(lpOutputString[i] == '\\')
		{
			i++;
			continue;
		}
		// if not a substitutal character
		if(lpOutputString[i] != '%')
			continue;

		format[j++] = lpOutputString[++i];
		format[j] = '\0';
		switch(lpOutputString[i])
		{
			// string
		case 's':
			{
				char* s = va_arg( argptr, char * );
				swprintf(OutMsg, MAXSTRL,format,s);
				wcscpy_s(format,OutMsg);
				j = wcslen(format);
				wcscat_s(format, L" ");
				break;
			}
			// character
		case 'c':
			{
				char c = (char) va_arg( argptr, int );
				swprintf(OutMsg, MAXSTRL, format,c);
				wcscpy_s(format,OutMsg);
				j = wcslen(format);
				wcscat_s(format, L" ");
				break;
			}
			// integer
		case 'd':
			{
				int d = va_arg( argptr, int );
				swprintf(OutMsg, MAXSTRL, format,d);
				wcscpy_s(format,OutMsg);
				j = wcslen(format);
				wcscat_s(format, L" ");
				break;
			}
		}
	}           
	OutputDebugString(OutMsg);
	va_end( argptr );
#endif
}

