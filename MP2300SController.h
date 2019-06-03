#pragma once
using namespace System::Net;
using namespace System::Net::Sockets;

using BYTE = unsigned char;
using WORD = unsigned short;
using INT = signed short;
using DINT = signed int;

// Designed By Hoang Van Luu 
// Skype: katllu
// Mobile: 0909.886.483
// E-mail: hoangluu.automation@gmail.com
public ref class MP2300SController
{
private:
	static constexpr DINT READ_BUFFER_SIZE = 2048;
	static constexpr DINT WRITE_BUFFER_SIZE = 2048;

	array<BYTE>^ bufferReceiver = nullptr;
	array<BYTE>^ bufferSender = nullptr;

	Socket^ mSocket = nullptr;

	System::String^ ip = "127.0.0.1";
	WORD port = 502;

public:
	MP2300SController();

	MP2300SController(System::String^ ip, WORD port);

	void Connect();

	void Disconnect();

	void WriteML(WORD startAddress, array<DINT>^ ints);

	array<BYTE>^ WriteMultipleRegistersMsg(WORD id, BYTE slaveAddress, WORD startAddress, BYTE function, array<BYTE>^ values);

	array<BYTE>^ ToByteArray(DINT value);

	array<BYTE>^ ToByteArray(array<DINT>^ value);

	void Write(array<BYTE>^ frame);

	array<BYTE>^ Read();

	void CheckValidate(array<BYTE>^ messageReceived);

};