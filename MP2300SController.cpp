#include "stdafx.h"
#include "MP2300SController.h"

MP2300SController::MP2300SController()
{

}

MP2300SController::MP2300SController(System::String^ ip, WORD port)
	: MP2300SController()
{
	this->ip = ip;
	this->port = port;
}

void MP2300SController::Connect()
{
	this->mSocket = gcnew Socket(AddressFamily::InterNetwork, SocketType::Stream, ProtocolType::Tcp);
	this->bufferReceiver = gcnew array<BYTE>(READ_BUFFER_SIZE);
	this->bufferSender = gcnew array<BYTE>(WRITE_BUFFER_SIZE);
	this->mSocket->SendBufferSize = READ_BUFFER_SIZE;
	this->mSocket->ReceiveBufferSize = WRITE_BUFFER_SIZE;
	IPEndPoint^ server = gcnew IPEndPoint(IPAddress::Parse(this->ip), this->port);
	this->mSocket->Connect(server);
}

void MP2300SController::Disconnect()
{
	if (this->mSocket == nullptr)
	{
		return;
	}
	if (this->mSocket->Connected)
	{
		this->mSocket->Close();
	}
}

void MP2300SController::WriteML(WORD startAddress, array<DINT>^ ints)
{
	const BYTE slaveAddress = 1;
	const BYTE function = 16;
	WORD id = function;
	array<BYTE>^ values = ToByteArray(ints);

	array<BYTE>^ frame = WriteMultipleRegistersMsg(id, slaveAddress, startAddress, function, values);
	this->Write(frame);
	System::Threading::Thread::Sleep(100);
	array<BYTE>^ buffReceiver = this->Read();
	DINT SizeByte = buffReceiver[8];
	array<BYTE>^ byteMsg = nullptr;

	if (function != buffReceiver[7])
	{
		array<BYTE>^ errorbytes = gcnew array<BYTE>(3);
		for (int i = 0; i < errorbytes->Length; i++)
		{
			errorbytes[i] = buffReceiver[i + 6];
		}

		this->CheckValidate(errorbytes);

		byteMsg = gcnew array<BYTE>(9);
		for (int i = 0; i < byteMsg->Length; i++)
		{
			byteMsg[i] = buffReceiver[i + 9];
		}
	}
	else
	{
		byteMsg = gcnew array<BYTE>(READ_BUFFER_SIZE);
		for (int i = 0; i < byteMsg->Length; i++)
		{
			byteMsg[i] = buffReceiver[i];
		}
	}
}

array<BYTE>^ MP2300SController::WriteMultipleRegistersMsg(WORD id, BYTE slaveAddress, WORD startAddress, BYTE function, array<BYTE>^ values)
{
	DINT byteCount = values->Length;
	array<BYTE>^ frame = gcnew array<BYTE>(13 + byteCount);
	frame[0] = safe_cast<BYTE>(id >> 8);
	frame[1] = safe_cast<BYTE>(id);
	frame[2] = 0;
	frame[3] = 0;
	frame[4] = 0;
	frame[5] = safe_cast<BYTE>(7 + byteCount);
	frame[6] = slaveAddress;
	frame[7] = function;
	frame[8] = safe_cast<BYTE>(startAddress >> 8);
	frame[9] = safe_cast<BYTE>(startAddress);
	WORD amount = safe_cast<BYTE>(byteCount / 2);
	frame[10] = safe_cast<BYTE>(amount >> 8);
	frame[11] = safe_cast<BYTE>(amount);
	frame[12] = safe_cast<BYTE>(byteCount);
	for (int i = 0; i < values->Length; i++)
	{
		frame[i + 13] = values[i];
	}
	return frame;
}

array<BYTE>^ MP2300SController::ToByteArray(DINT value)
{

	array<BYTE>^ bytes = gcnew array<BYTE>(4);
	bytes[1] = value & 0xFF;
	bytes[0] = (value >> 8) & 0xFF;
	bytes[3] = (value >> 16) & 0xFF;
	bytes[2] = (value >> 24) & 0xFF;
	short temp;
	for each (BYTE var in bytes)
	{
		temp = safe_cast<short>(var);
	}
	return bytes;
}

array<BYTE>^ MP2300SController::ToByteArray(array<DINT>^ value)
{
	int length = value->Length;
	array<BYTE>^ result = gcnew array<BYTE>(4 * length);
	for (int i = 0; i < length; i++)
	{
		array<BYTE>^ temp = this->ToByteArray(value[i]);
		if (temp->Length != 4) continue;
		for (int j = 0; j < temp->Length; j++)
		{
			result[i * 4 + j] = temp[j];
		}
	}
	return result;
}

void MP2300SController::Write(array<BYTE>^ frame)
{
	this->mSocket->Send(frame, frame->Length, SocketFlags::None);
}

array<BYTE>^ MP2300SController::Read()
{
	NetworkStream^ ns = gcnew NetworkStream(this->mSocket);
	if (ns->CanRead)
	{
		INT rs = this->mSocket->Receive(this->bufferReceiver, this->bufferReceiver->Length, SocketFlags::None);
	}
	return this->bufferReceiver;
}

void MP2300SController::CheckValidate(array<BYTE>^ messageReceived)
{
	switch (messageReceived[1])
	{

	case 129: // Hex: 81
	case 130: // Hex: 82
	case 131: // Hex: 83
	case 132: // Hex: 83
	case 133: // Hex: 84
	case 134: // Hex: 86
	case 143: // Hex: 8F
	case 144: // hex: 90
		switch (messageReceived[2])
		{
		case 1:
			throw gcnew System::NotImplementedException("01/0x01: Illegal Function.");
		case 2:
			throw gcnew System::NotImplementedException("02/0x02: Illegal Data Address.");
		case 3:
			throw gcnew System::NotImplementedException("03/0x03: Illegal Data Value.");
		case 4:
			throw gcnew System::NotImplementedException("04/0x04: Failure In Associated Device.");
		case 5:
			throw gcnew System::NotImplementedException("05/0x05: Acknowledge.");
		case 6:
			throw gcnew System::NotImplementedException("06/0x06: Slave Device Busy.");
		case 7:
			throw gcnew System::NotImplementedException("07/0x07: NAK – Negative Acknowledgements.");
		case 8:
			throw gcnew System::NotImplementedException("08/0x08: Memory Parity Error.");
		case 10:
			throw gcnew System::NotImplementedException("10/0x0A: Gateway Path Unavailable.");
		case 11:
			throw gcnew System::NotImplementedException("11/0x0B: Gateway Target Device Failed to respond.");
		default:
			break;
		}
		break;
	}
}