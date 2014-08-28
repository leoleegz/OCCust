#include "OCPlugin.h"

// ---------------------------------------------------------------------------

PCREATETEXTSERVICES Real_CreateTextServices = NULL;


HRESULT WINAPI Routed_CreateTextServices(IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk)
{
	//return Real_CreateTextServices(punkOuter, pITextHost, ppUnk);
	return Real_CreateTextServices(punkOuter, pITextHost, ppUnk);
}

void Handle_GetProcAddress(NktHandlerParams * hp)
{
	HMODULE hModule = *(HMODULE*)PARAMETER_INDEX(0);
	LPCWSTR lpProcName = *(LPCWSTR*)PARAMETER_INDEX(1);

	if (HIWORD(lpProcName))
	{
		if( wcscmp(lpProcName, TEXT("CreateTextServices")) == 0 )
		{
			HMODULE hMSFT = LoadLibrary(TEXT("msftedit.dll"));

			if( hModule == hMSFT )
			{
				Real_CreateTextServices = (PCREATETEXTSERVICES)WINAPI_RETVAL;
				hp->iHook->SetReturnValue(&hp->context, (INT_PTR) Routed_CreateTextServices);
			}
		}
	}
	return;
}
