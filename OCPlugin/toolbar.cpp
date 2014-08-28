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

// globals

HookList			g_hookList;
OC_TOPWINDOW		g_ocTopWindow;
OC_IFACES			g_ocIfaces;
OC_CONTACTINFOWINDOW g_ocContactInfoWindow;
CResourceManager	g_resMgr;
OC_DIRECTUIWINDOW	g_ocDUIWindow;
CActionDispatcher	g_actDisp;

void AddToolbarButtons(OC_TOOLBARBUTTON* ptbb, UINT numButtons, 
					   char* szXmlRes, char* szStyle, size_t cbXmlBufSz, 
					   size_t cbStyleBufSz)
{	
	UINT uLastRcImg = RESOURCE_ID_BASE;
	UINT uImgId;
	DWORD dwBytePos = (ptbb->uPosition == TB_INSERT_FIRST ? TB_INSERT_FIRST_BYTE : ptbb->uPosition);

	// Build up XML resource

	char tmpStyle[MAXSTRL];
	char tmpRes[MAXSTRL];

	for (UINT i = 0; i < numButtons; i++)
	{
		sprintf_s(tmpRes,"<Button id=atom(%s) AccRole=57 Class=\"TransparentButton\" Layout=flowlayout(0,2,0,2)"
			" Active=MouseandKeyboard|NoSyncFocus Padding=rect(5,4,5,4)>\n"
			" <element class=\"ToolbarIcon\" ID=Atom(%s)/> </Button> \n", 
			ptbb[i].szButtonId, ptbb[i].szTButtonId);

		uImgId = (ptbb[i].fExtBitmap ? uLastRcImg : ptbb[i].uResID);

		sprintf_s(tmpStyle, "\nbutton[ID=atom(%s)]\n"
			"{\n"
			"AccName:\"%s\"; \n"		// button name
			"AccRole: 57; \n"								// see Oleacc.h for MSAA UI roles
			"AccDesc:\"%s\"; \n"
			"AccDefAction:rcstr(20068); \n"				// def action for buttons
			"ShortcutString:\"%s\"; \n"
			"}\n element[id=atom(%s)] { content:rcimg(%d); }", ptbb[i].szButtonId, 
			ptbb[i].szButtonId, ptbb[i].szTooltip, ptbb[i].szTooltip, 
			ptbb[i].szTButtonId, uImgId);

		if (i == 0)
		{
			strcpy_s(szXmlRes, cbXmlBufSz, tmpRes);
			strcpy_s(szStyle, cbStyleBufSz, tmpStyle);
		} else {
			strcat_s (szXmlRes, cbXmlBufSz, tmpRes);
			strcat_s (szStyle, cbStyleBufSz, tmpStyle);
		}

		if (ptbb[i].fExtBitmap)
		{
			// Perfectly safe to call resource functions since Win32 resource management function
			// hooking is not active yet

			RESOURCEINFO ri;
			ri.uResId		= ptbb[i].uResID;
			ri.hrsrc		= FindResource(ptbb[i].hModule, MAKEINTRESOURCE(ptbb[i].uResID), 
				ptbb[i].wszResType);
			ri.dwSize		= SizeofResource(ptbb[i].hModule, ri.hrsrc);
			ri.hResData		= LoadResource(ptbb[i].hModule, ri.hrsrc);
			ri.pvData		= LockResource(ri.hResData);
			g_resMgr.RegisterNewResource(OCRES_BITMAPS, uLastRcImg, ri);
			uLastRcImg++;	
		}	
		g_actDisp.RegisterAction(ptbb[i].wszButtonId, ptbb[i].pfnAct);
	}
	g_resMgr.RegisterResource(OCRES_XMLSTYLE, OCRES_CONTACT_LIST, szStyle, strlen(szStyle), RR_INSERT,
		OCRES_STYLE_INSERT_POS);
	g_resMgr.RegisterResource(OCRES_XMLUI, OCRES_CONTACT_LIST, szXmlRes, strlen(szXmlRes), RR_INSERT, dwBytePos);

}
