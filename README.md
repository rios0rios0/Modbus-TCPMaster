<h1 align="center">Modbus TCP Master</h1>
<p align="center">
    <a href="https://github.com/rios0rios0/modbus-tcpmaster/releases/latest">
        <img src="https://img.shields.io/github/release/rios0rios0/modbus-tcpmaster.svg?style=for-the-badge&logo=github" alt="Latest Release"/></a>
    <a href="https://github.com/rios0rios0/modbus-tcpmaster/blob/main/LICENSE">
        <img src="https://img.shields.io/github/license/rios0rios0/modbus-tcpmaster.svg?style=for-the-badge&logo=github" alt="License"/></a>
</p>

A Modbus TCP master implementation for controlling Yaskawa MP2300S motion controllers, built as a C++/CLI dynamic link library with Visual Studio. The project implements the Modbus TCP protocol to write multiple registers (function code 16) to servo drive controllers over a standard TCP socket connection on port 502.

## Features

- **Modbus TCP protocol implementation** -- constructs and sends compliant Modbus Application Protocol (MBAP) frames with transaction ID, protocol ID, unit ID, and function code headers
- **Write Multiple Registers (FC 16)** -- implements Modbus function code 16 for writing arrays of 32-bit signed integers to consecutive holding registers on the slave device
- **MP2300S controller interface** -- dedicated `MP2300SController` class with `Connect`, `Disconnect`, and `WriteML` methods for Yaskawa MP2300S motion controller communication
- **Error validation** -- parses Modbus exception responses and maps error codes (0x01 through 0x0B) to descriptive messages including Illegal Function, Illegal Data Address, Slave Device Busy, Gateway Path Unavailable, and others
- **Byte-order conversion** -- handles the big-endian to little-endian byte swapping required by the Modbus protocol for 32-bit DINT values across register pairs
- **Interactive console application** -- includes a `Main.cpp` test harness that connects to a controller and accepts user input for writing 4 register values in a loop

## Technologies

- C++/CLI (Visual C++ with .NET managed extensions)
- .NET Framework (`System::Net::Sockets` for TCP communication)
- Visual Studio 2017 solution (`.sln` / `.vcxproj`)
- Modbus TCP protocol (port 502)

## Project Structure

```
modbus-tcpmaster/
├── MP2300SController.h        # Controller class declaration (Connect, Disconnect, WriteML, Read, Write)
├── MP2300SController.cpp      # Full Modbus TCP protocol implementation with frame construction and validation
├── Main.cpp                   # Interactive console test application for writing register values
├── stdafx.h                   # Precompiled header
├── stdafx.cpp                 # Precompiled header source
├── AssemblyInfo.cpp           # .NET assembly metadata (version 1.0.*)
├── resource.h                 # Resource definitions
├── app.rc                     # Application resource script
├── app.ico                    # Application icon
├── app.aps                    # Visual Studio resource editor cache
├── ModbusTCPMaster.sln        # Visual Studio solution file
├── ModbusTCPMaster.vcxproj    # Visual Studio project file
├── ModbusTCPMaster.vcxproj.filters  # Source file filters
├── ModbusTCPMaster.vcxproj.user     # User-specific project settings
├── LICENSE
└── README.md
```

## Installation

### Prerequisites

- Visual Studio 2017 or later with the **C++/CLI** workload and **.NET desktop development** components installed

### Building

1. Open `ModbusTCPMaster.sln` in Visual Studio
2. Select the desired configuration (Debug or Release)
3. Build the solution (`Ctrl+Shift+B`)
4. The compiled DLL and test executable will be in the output directory

## Usage

### As a library

Reference the compiled DLL in your .NET or C++/CLI project and use the `MP2300SController` class:

```cpp
// Create controller instance targeting the MP2300S at a specific IP
MP2300SController^ controller = gcnew MP2300SController("192.168.1.100", 502);

// Establish TCP connection
controller->Connect();

// Write 4 integer values to registers starting at address 0
auto values = gcnew cli::array<DINT>(4);
values[0] = 1000;  // Register 0-1
values[1] = 2000;  // Register 2-3
values[2] = 3000;  // Register 4-5
values[3] = 4000;  // Register 6-7
controller->WriteML(0, values);

// Disconnect when done
controller->Disconnect();
```

### As a standalone console application

Run the compiled executable to interactively write register values:

```
Enter values[0]: 1000
Enter values[1]: 2000
Enter values[2]: 3000
Enter values[3]: 4000

Write data is successfully!
```

### Modbus protocol details

- **Default port:** 502
- **Slave address:** 1
- **Function code:** 16 (Write Multiple Registers)
- **Buffer sizes:** 2048 bytes for both read and write
- **Data format:** 32-bit signed integers (DINT), split across two 16-bit Modbus registers each

## Contributing

Contributions are welcome. Please open an issue or submit a pull request.

## License

This project is licensed under the terms specified in the [LICENSE](LICENSE) file.
