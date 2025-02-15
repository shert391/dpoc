#pragma once

struct provconst {
	path				 driverPath;
	path				 symbolicPath;
	size_t				 sizeBinary;
	const unsigned char* pBinary;
};

class prov {
protected:
	HANDLE m_hDevice = 0;

private:
	provconst m_info_;

public:
	explicit prov (provconst info) : m_info_(info) {}

	void drvload (IN const path& drvPath);

private:
	void drvLoadPrv ();
	void drvUnloadPrv ();
	void prvDrvCreate ();

	virtual void read (IN void* pFrom, OUT void* pDst, IN size_t size)				   = 0;
	virtual void write (IN void* pFrom, IN void* pDst, IN size_t size, OUT void* pOld) = 0;
};