#include "CLI.hpp"

CLI::CLI(unsigned long baud)
{
    SERIAL_INTERFACE.begin(baud);
}

void CLI::addCommand(char cmdID, CommandCallback callback)
{
    // Prevent overflow of the command array
    if (_commandCount < MAX_COMMANDS)
    {
        _commandList[_commandCount].id = cmdID;
        _commandList[_commandCount].callback = callback;
        _commandCount++;
    }
    else
    {
        SERIAL_INTERFACE.println("Max command limit reached!");
    }
}

void CLI::run()
{
    // Read all available characters
    while (SERIAL_INTERFACE.available())
    {
        char c = SERIAL_INTERFACE.read();

        // SERIAL_INTERFACE.write(c);

        // If it's a newline (end of command), handle the buffer
        if (c == '\n' || c == '\r')
        {
            // Null-terminate the buffer and process
            _rxBuffer[_rxIndex] = '\0';
            handleBuffer();
            _rxIndex = 0;
        }
        else
        {
            // Add the character to the buffer (with overflow check)
            if (_rxIndex < (MAX_BUFFER_SIZE - 1))
            {
                _rxBuffer[_rxIndex++] = c;
            }
            else
            {
                // Buffer overflow case, reset it
                _rxIndex = 0;
                SERIAL_INTERFACE.println("Buffer overflow!");
            }
        }
    }
}

void CLI::handleBuffer()
{
    // If no characters, do nothing
    if (_rxIndex == 0)
        return;

    // The first character in _rxBuffer is the command ID
    char cmdID = _rxBuffer[0];

    // The rest of the string (after the first character) is the command argument
    const char *arg = (_rxIndex > 1) ? &_rxBuffer[1] : "";

    // Search for a matching command
    for (int i = 0; i < _commandCount; i++)
    {
        if (_commandList[i].id == cmdID)
        {
            // If found, call its callback with the argument
            _commandList[i].callback(arg);
            return;
        }
    }

    // If no match, print an error
    SERIAL_INTERFACE.print("Unknown command: ");
    SERIAL_INTERFACE.println(cmdID);
}
