// dllmain.h : Declaration of module class.

class CFFMPEGInteropModule : public ATL::CAtlDllModuleT< CFFMPEGInteropModule >
{
public :
	DECLARE_LIBID(LIBID_FFMPEGInteropLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_FFMPEGINTEROP, "{4900312A-A476-487F-8E8C-A848293DDB44}")
};

extern class CFFMPEGInteropModule _AtlModule;
