#include "functionwrapper.h"

///
/// Helpers
///
#define CP_OBJECT(r) \
	_fnc = r._fnc; \
	_cc = r._cc; \
	_pmSize = r._pmSize;

///
/// Constructors/Destructors
///
NktFunctionWrapper::NktFunctionWrapper()
{
	_pmSize = 0;
	_fnc = NULL;
	_cc = bad_cc_;
}
NktFunctionWrapper::NktFunctionWrapper(const void* fnc, int cc, USHORT pmSize)
{
	_fnc = fnc;
	_cc = static_cast<NktCallingConvention>(cc);
	_pmSize = pmSize;
}
NktFunctionWrapper::NktFunctionWrapper(const NktFunctionWrapper& cp)
{ 
	CP_OBJECT(cp);
}
NktFunctionWrapper& NktFunctionWrapper::operator=(const NktFunctionWrapper& cp)
{
	CP_OBJECT(cp);
	return *this;
}

///
/// Getters
///
const void* NktFunctionWrapper::GetAddress() const
{
	return _fnc;
}
int NktFunctionWrapper::GetCallConvention() const
{
	return _cc;
}
USHORT NktFunctionWrapper::GetParamSize() const
{
	return _pmSize;
}
USHORT NktFunctionWrapper::GetCallerCleanSize() const
{
	USHORT ret = 0;
	switch(_cc)
	{
		//case thiscall_com_:
		case safecall_:
		case stdcall_:
		case syscall_:
		case thiscall_ms_:
		case fastcall_:
			//ret = 0;
			break;
		case cdecl_:
		case thiscall_:
		case pascal_:
			ret = _pmSize;
			break;
		default:
			NODEFAULT;
	}

	//FIXME {
#pragma message("FIXME: clean size adjusted to REGISTER_SIZE")
	if (ret % REGISTER_SIZE)
	{
		ret /= REGISTER_SIZE;
		ret *= REGISTER_SIZE + 1;
	}
	//FIXME }

	return ret;
}
USHORT NktFunctionWrapper::GetCalleeCleanSize() const
{
	USHORT ret = 0;
	switch(_cc)
	{
		//case thiscall_com_:
		case safecall_:
		case stdcall_:
		case syscall_:
			ret = _pmSize;
			break;
		case thiscall_ms_:
			ret = _pmSize - REGISTER_SIZE;
			break;
		case fastcall_:
			ret = (_pmSize > (REGISTER_SIZE << 1))? _pmSize-(REGISTER_SIZE << 1) : 0;
			break;
		case cdecl_:
		case thiscall_:
		case pascal_:
			//ret = 0;
			break;
		default:
			NODEFAULT;
	}

	//FIXME {
#pragma message("FIXME: clean size adjusted to REGISTER_SIZE")
	if (ret % REGISTER_SIZE)
	{
		ret /= REGISTER_SIZE;
		ret *= REGISTER_SIZE + 1;
	}
	//FIXME }

	return ret;
}
USHORT NktFunctionWrapper::GetStackParamSize() const
{
	USHORT ret = _pmSize;
	int dec = (_cc == fastcall_)? 2 : (_cc == thiscall_ms_)? 1 : 0;
	while (dec > 0 && ret >= REGISTER_SIZE)
	{
		ret -= REGISTER_SIZE;
		--dec;
	}
	return ret;
}

///
/// FunctionWrapper::isValid
///
bool NktFunctionWrapper::isValid() const
{
	return (_fnc != NULL) && (_cc != bad_cc_);
}