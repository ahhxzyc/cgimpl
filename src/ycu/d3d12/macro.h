#pragma once

#define CHECK_HR(X)                                                  \
    do {                                                                       \
        const HRESULT hr = X;                                         \
        if(!SUCCEEDED(hr))                                            \
        {                                                                      \
            std::cout <<                                           \
                    std::string("errcode = ")                                  \
                    + std::to_string(hr) + "."                        \
                    + std::system_category().message(hr);            \
        }                                                                      \
    } while(false)