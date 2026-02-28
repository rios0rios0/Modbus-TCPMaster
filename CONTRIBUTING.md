# Contributing

Contributions are welcome. By participating, you agree to maintain a respectful and constructive environment.

For coding standards, testing patterns, architecture guidelines, commit conventions, and all
development practices, refer to the **[Development Guide](https://github.com/rios0rios0/guide/wiki)**.

## Prerequisites

- [Visual Studio](https://visualstudio.microsoft.com/) 2017+ with the **C++/CLI** workload and **.NET desktop development** components
- [.NET Framework](https://dotnet.microsoft.com/en-us/download/dotnet-framework) 4.0+ (for `System::Net::Sockets`)
- A Modbus TCP slave device (e.g., Yaskawa MP2300S) or a Modbus TCP simulator for testing

## Development Workflow

1. Fork and clone the repository
2. Create a branch: `git checkout -b feat/my-change`
3. Open the solution in Visual Studio:
   ```
   Open ModbusTCPMaster.sln in Visual Studio
   ```
4. Build the solution:
   ```
   Ctrl+Shift+B (or Build > Build Solution)
   ```
5. Alternatively, build from the Developer Command Prompt:
   ```bash
   msbuild ModbusTCPMaster.sln /p:Configuration=Debug /p:Platform=Win32
   ```
6. Run the console test application from the output directory:
   ```bash
   Debug\ModbusTCPMaster.exe
   ```
7. Commit following the [commit conventions](https://github.com/rios0rios0/guide/wiki/Life-Cycle/Git-Flow)
8. Open a pull request against `main`
