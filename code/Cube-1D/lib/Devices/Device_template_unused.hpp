#pragma once

class Device_Template
{
public:
    // Base constructor
    Device_Template() = default;

    // Virtual destructor ensures proper cleanup when deleting via a base pointer
    virtual ~Device_Template() = default;

    // Common interface methods
    virtual bool init() = 0;
    virtual bool getStatus() = 0;
};
