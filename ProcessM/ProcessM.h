// ProcessM.cpp : Defines the entry point for the console application.
//
#pragma once

#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <memory>

using namespace std;

/*Stany procesów, przy wyswietlaniu ich pokazują się
odpowiednio numery kolejnisci 0, 1, 2, ...*/

enum State { NOWY, GOTOWY, ZAKONCZONY, AKTYWNY, OCZEKUJACY };
int GroupNumber = 0;

/*Nazdorca Id - on nadaje procesom Id w sposów unikatowy
i nie powtarzalny, działą na zasadzie tablicy zajętych numerów
oraz miejsca gdzie numer nie jest zajety*/

struct OverseerID {
	int LimitID = 100;
	std::vector<int>FreeIDList;
	int FreeID;


	OverseerID() {

		for (int i = 0; i < LimitID; i++) {
			FreeIDList.push_back(1);

		}
	}

	int GetFreeID() {

		for (int i = 0; i < LimitID; i++) {
			if (FreeIDList[i] == 1) {
				FreeID = i;
				FreeIDList[i] = 0;
				return FreeID;

			}
		}

	};

	void ClearID(int ID) {
		FreeIDList[ID] = 1;
	}

};

/*Musimy miec 1 obiekt globalny dla wszytskich Id*/
OverseerID IDs;

/*Blok Kontrony procesu*/
//Jesli potrzebujecie jeszcze jakies POLA, GETERY, SETERY piszcie
struct PCB {


	int ProcessID;
	State ProcessState;
	string ProcessName;
	int ProcessGroup;

	//

	int A;
	int B;
	int C;

	//

	int CommandCounter;
	int Tau;
	int Timmer;

	// Konstruktor tworzący nowy PCB

	PCB(string processname, int processgroup) {

		ProcessID = IDs.GetFreeID();
		ProcessState = NOWY;
		ProcessName = processname;
		ProcessGroup = processgroup;

		A, B, C = 0;

		CommandCounter = 0;
		Tau = 0;
		Timmer = 0;


	};

	//Metody - gdyz nie bedziecie mieli dostępu bezpośredio do pól

	int GetID() {

		return ProcessID;

	};
	string GetName() {

		return ProcessName;

	};
	State GetState() {

		return ProcessState;

	};
	void SetState(State state) {

		ProcessState = state;

	};
	void PrintPCBInformations() {};
	int GetCommandCounter() {

		return CommandCounter;

	};
	void SetCommandCounter(int commandcounter) {

		CommandCounter = commandcounter;

	};
	int GetTau() {

		return Tau;

	};
	void SetTau(int tau) {

		Tau = tau;

	};
	int GetTimmer() {

		return Timmer;

	};
	void SetTimmer(int timmer) {

		Timmer = timmer;

	};

};


/*Obiekt pozwalający na wsadzenie mi grup do listy (zrobienie listy grup)*/

struct Group {

	int ProcessGroup;
	Group() {
		ProcessGroup = GroupNumber;
		GroupNumber++; // uproszcony nadzorca
	}
	/*Jedna grupa posaida swój nr, nadawany przez nadzorce
	oraz listę procesów nalezącej do tej grupy*/
	list<std::shared_ptr<PCB>>ProcessList;

};

/*Globalna lista grup na której bedzie działac zarządanie procesami*/

list<Group>ProcessGroupsList;

/*Dla procesora lista nowych procesów*/

std::vector<shared_ptr<PCB>> procesy_otrzymane; 


/*Tworzenie nowego procesu i dodawanie go do istniejacej grupy,
Id nadaje nadzorca*/
void NewProcess(string ProcessName, int ProcessGroup) {

	std::shared_ptr<PCB> New = std::make_shared<PCB>(ProcessName, ProcessGroup);
	for (auto it = ProcessGroupsList.begin(); it != ProcessGroupsList.end(); ++it) {
		if (ProcessGroup == it->ProcessGroup) it->ProcessList.push_back(New);
		//else NewProcessGroup(ProcessName); // nie wiem czy wyrzucać bląd gdy podasz grupę ktora nie stnieje czy odrazu utworzyć taką grupę
	}
	procesy_otrzymane.push_back(New); // funkcja niebezpieczna potrzebna dla procesora !!!
};

/*Usuwanie procesu znjąc tylko jego Id*/
void DeleteProcess(int ProcessID) {
	for (auto it = ProcessGroupsList.begin(); it != ProcessGroupsList.end(); ++it) {
		it->ProcessList.remove_if([ProcessID](std::shared_ptr<PCB> n) { return n->GetID() == ProcessID; });
	}// co zrobić gdy usuniemy ostatni proces z danje grupy, usunąc odrazu grupę ?
};

/*Usuwanie calej grupybprocesów, bez względu na to czy jakiś jest w środku*/
void DeleteProcessGroup(int ProcessGroup) {

	ProcessGroupsList.remove_if([ProcessGroup](Group n) {return n.ProcessGroup == ProcessGroup; });

};

/*Tworzenie pierwszego procesu przy tworzeniu grupy tzw. procesu bezczynnego*/
std::shared_ptr<PCB> FirstProcess(int ProcessGroup) {

	std::shared_ptr<PCB>first = std::make_shared<PCB>("bezczynny", ProcessGroup); // nie wiej ajaki tu stan, sprawdz jak mieli w stystemie z c++

	return first;
};

/*Tworzenie nowej grupy procesów i odd razu rozpoczynanie w niej
procesu bezczynnego, koniecznego do funkcjonowania grupy*/
void NewProcessGroup(string ProcessName) {//??????????????????
	Group NewGroup; // numer powinien byc adany przez nadzorcę
	NewGroup.ProcessList.push_back(FirstProcess(NewGroup.ProcessGroup));
	ProcessGroupsList.push_back(NewGroup);
};

/*Ustawianie Stanu tylko po Id bez znajomosci grupy*/
void SetStateID(int ProcessID, State state) {

	for (auto &v : ProcessGroupsList) {
		for (auto &x : v.ProcessList) {
			x->SetState(state);
		}
	}

};

/*Zwracanie Stanu tylko po Id bez znajomosci grupy*/
State GetStateID(int ProcessID) {

	for (auto &v : ProcessGroupsList) {
		for (auto &x : v.ProcessList) {
			if (x->GetID() == ProcessID) return x->GetState();
		}
	}

};
/*Zwracanie wskażnika do procesu, który pozwala wywołac bezpośrednio metody PCB*/
std::shared_ptr<PCB> GetPCB(int ProcessID) {


	for (auto &v : ProcessGroupsList) {
		for (auto &x : v.ProcessList) {
			if (x->GetID() == ProcessID) return x;
		}
	}

};

/*Wypisywanie przykłądowych info o całej grupie procesów
(czyli o wszytskich procesach znajdującyzch sie w niej)*/
void PrintGroupInformation(int ProcessGroup) {
	for (auto it = ProcessGroupsList.begin(); it != ProcessGroupsList.end(); ++it) {
		if (ProcessGroup == it->ProcessGroup) {
			for (auto x : it->ProcessList) {
				std::cout << "Nazwa: " << x->GetName() << std::endl;
				std::cout << "Id: " << x->GetID() << std::endl;
				std::cout << "Stan: " << x->GetState() << std::endl;

			}
		}
	}

};

/*WYpisanie info o Liscie grup procesów*/
void PrintProcessListInformation() {

	for (auto &v : ProcessGroupsList) {

		std::cout << "Numer grupy:" << v.ProcessGroup << std::endl;

	}

};


