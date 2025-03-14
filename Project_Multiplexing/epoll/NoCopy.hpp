#pragma once

class NoCopy{
public:
    NoCopy(){}
    NoCopy(const NoCopy& t) = delete;
    void operator=(const NoCopy& t) = delete;

};