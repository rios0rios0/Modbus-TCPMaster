#include "stdafx.h"
#include <array>
#include <iostream>
#include "MP2300SController.h"

using namespace std;

using DINT = signed int;

// Designed By Hoang Van Luu 
// Skype: katllu
// Mobile: 0909.886.483
// E-mail: hoangluu.automation@gmail.com
//
// Adapted By Felipe Rios da Silva Cordeiro (rios0rios0)
// E-mail: felipe.rios.silva@outlook.com
int main() {
	DINT num = 0;
	WORD startAddress = 0;
	auto values = gcnew cli::array<DINT>(4);
	//Supondo que o Slave está rodando na mesma máquina
	System::String^ IP = "127.0.0.1";
	//Porta padrão do ModbusTCP
	WORD Port = 502;
	//Conexão com o Slave
	MP2300SController^ objMP2300SController = gcnew MP2300SController(IP, Port);
	objMP2300SController->Connect();
	//Entrada do usuário
	while (true) {
		cout << "Enter values[0]: ";
		cin >> num;
		values[0] = num;
		cout << "Enter values[1]: ";
		cin >> num;
		values[1] = num;
		cout << "Enter values[2]: ";
		cin >> num;
		values[2] = num;
		cout << "Enter values[3]: ";
		cin >> num;
		values[3] = num;
		//Finalização da escrita
		objMP2300SController->WriteML(startAddress, values);
		printf("\nWrite data is successfully!\n");
	}
	//Desconexão e finalização
	objMP2300SController->Disconnect();
	return 0;
}