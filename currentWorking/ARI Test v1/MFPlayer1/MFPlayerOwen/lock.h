#pragma once


class SharedLock : public ISharedLock, CritSec
{
public:
    SharedLock() : m_cRef(1)
    {
    }

    // IUnknown methods
    STDMETHODIMP_(ULONG) AddRef()
    {
            return InterlockedIncrement(&m_cRef);
    }
    STDMETHODIMP_(ULONG) Release()
    {
        ULONG uCount = InterlockedDecrement(&m_cRef);
        if (uCount == 0)
        {
            delete this;
        }
        return uCount;
    }
    STDMETHODIMP QueryInterface(REFIID iid, void** ppv)
    {
        if (!ppv)
        {
            return E_POINTER;
        }
        if (iid == IID_IUnknown)
        {
            *ppv = static_cast<IUnknown*>(this);
        }
        else if (iid == __uuidof(ISharedLock))
        {
            *ppv = static_cast<ISharedLock*>(this);
        }
        else
        {
            *ppv = NULL;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }

    STDMETHODIMP Lock()
    {
        CritSec::Lock();
        return S_OK;
    }

    STDMETHODIMP Unlock()
    {
        CritSec::Unlock();
        return S_OK;
    }

private:
    volatile long m_cRef;        // Reference count.
};


