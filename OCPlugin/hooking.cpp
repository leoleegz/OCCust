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
#include "OCPlugin.h"

void AttachHook (const HOOK_DESCRIPTOR& hd, HookList& hl)
{
    HMODULE hMod = GetModuleHandle (hd.szTargetModule);
    FARPROC hProc = GetProcAddress (hMod, hd.szTargetFunction);
    _ASSERTE (hProc);

    NktFunctionWrapper fwFunction (hProc, hd.nccCallingCnv, hd.cbParam);
	NktFunctionWrapper fwHandler (hd.pvHookHandler, cdecl_, sizeof (NktHandlerParams*));
		
	ApiHookPtr hook = new NktApiHook (fwFunction, NktLocalFunction(fwHandler), hd.iFlags);
    hl.push_back (hook);

    FreeLibrary (hMod);
}

// ---------------------------------------------------------------------------
void AttachHookArray (const HOOK_DESCRIPTOR hda[], HookList& hl)
{
    for (int i = 0; i < NUM_HOOK_DESCRIPTORS; i++)
        AttachHook (hda[i], hl);	
}



