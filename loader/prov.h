#pragma once

struct prconst {
    const wchar_t* szDriverPath;

    union { // пути до объекта устройства драйвера в пространстве имён диспетчера объектов NT
        const wchar_t* szSymObjPath; // путь через символическую ссылку
        const wchar_t* szDevicePath; // прямой путь
    };

    const size_t sizeBinary;
    const unsigned char* pBinary;
};

class prov {
protected:
    HANDLE m_hDevice = 0;

private:
    prconst m_info_;

public:
    explicit prov(prconst info) : m_info_(info) {}

    void drvload(const wchar_t* szDrvPath);

private:
    void prvDrvLoad();
    void prvDrvUnload();
    void prvDrvCreate();

    virtual void read(_In_ void* pFrom, _Out_ void* pDst, _In_ size_t size) = 0;
    virtual void write(_In_ void* pFrom, _In_ void* pDst, _In_ size_t size, _Out_ void* pOld) = 0;
};