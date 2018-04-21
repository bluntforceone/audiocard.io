/****************************************************************************************************
 * Copyright 2018 Conrad Jones                                                                      *
 *                                                                                                  *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software    *
 * and associated documentation files (the "Software"), to deal in the Software without             *
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,       *
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the    *
 * Software is furnished to do so, subject to the following conditions:                             *
 *                                                                                                  *
 * The above copyright notice and this permission notice shall be included in all copies or         *
 * substantial portions of the Software.                                                            *
 *                                                                                                  *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING    *
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND       *
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,     *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.          *
 *                                                                                                  *
 ***************************************************************************************************/

#ifndef AUDIOCARD_IO_COM_H
#define AUDIOCARD_IO_COM_H

#include <Windows.h>

namespace acom {

template <typename T>
struct ICom {
public:
    T* obj;
    explicit ICom(T* obj_ = nullptr)
    {
        this->obj = obj_;
    }
    explicit ICom(IUnknown* obj_)
    {
        this->obj = nullptr;
        HRESULT hr = obj_->QueryInterface(__uuidof(T), (void**)&this->obj);
    }
    explicit ICom(ICom<T>&& mov)
    {
        this->obj = mov.obj;
        mov.obj = nullptr;
    }
    explicit ICom(const ICom<T>& cpy)
    {
        this->obj = cpy.obj;
        if (this->obj != nullptr) {
            this->obj->AddRef();
        }
    }
    explicit ICom(REFCLSID clsid)
    {
        obj = nullptr;
        HRESULT hr = CoCreateInstance(clsid,
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(T),
            (void**)&obj);
    }

    ~ICom()
    {
        if (this->obj) {
            this->obj->Release();
            this->obj = nullptr;
        }
    }

    void create(REFCLSID clsid)
    {
        if (this->obj != nullptr) {
            this->obj->Release();
            this->obj = nullptr;
        }
        HRESULT hr = CoCreateInstance(clsid,
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(T),
            (void**)&this->obj);
    }

    ICom<T>& operator=(T* inf)
    {
        if (this->obj != nullptr) {
            this->obj->Release();
        }
        this->obj = inf;
        return *this;
    }

    ICom<T>& operator=(ICom<T>&& other)
    {
        this->obj = std::move(other.obj);
        return *this;
    }

    ICom<T>& operator=(ICom<T>& other)
    {
        this->obj = other.obj;
        if (this->obj != nullptr) {
            this->obj->AddRef();
        }
        return *this;
    }

    T* operator->()
    {
        return this->obj;
    }
    T** operator&()
    {
        return &this->obj;
    }

    template <typename CLASS>
    static ICom<T>&& createInstance()
    {
        T* pInterface;
        HRESULT hr = CoCreateInstance(__uuidof(CLASS),
            nullptr,
            CLSCTX_INPROC_SERVER,
            __uuidof(T),
            (void**)&pInterface);
        return ICom<T>(pInterface);
    }
};
}

#endif //AUDIOCARD_IO_COM_H
