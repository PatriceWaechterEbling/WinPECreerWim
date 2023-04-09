// WinPECreerWim.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//
	// See also: https://docs.microsoft.com/de-de/cpp/c-runtime-library/reference/vsnprintf-s-vsnprintf-s-vsnprintf-s-l-vsnwprintf-s-vsnwprintf-s-l?view=msvc-160
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN	
#define titre "Assistant de capture OS en WIM"
#define WINVER  0x0600 //requiere  windows ver 6.xx et +

#if WINVER <0x0403 
#define TTM_SETTITLE 0x420
#define CCM_SETWINDOWTHEME 0x200B
#define TTS_BALLOON	0x40
#endif
#ifdef WINVER
#pragma warning(disable:4996)
#endif // WINVER

#define IDM_TIMER1 0x01000
#define WM_TRAY_ICONE WM_USER + 101
#include <windows.h> //entetes requises par l'application
#include <windowsx.h>
#include <wingdi.h>
#include <commctrl.h>
#include <Winuser.h>
#include <Commdlg.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <stdlib.h>
#include <strsafe.h>
#include <ShellAPI.h>
#include <io.h>	
#include <Shobjidl.h>
#include <Objbase.h>
#include <tCHAR.h>
#include "resource.h"

#pragma comment (lib,"comctl32.lib") 
#pragma comment (lib,"kernel32.lib") 
#pragma comment (lib,"user32.lib") 
#pragma comment (lib,"gdi32.lib") 
#pragma comment (lib,"winspool.lib") 
#pragma comment (lib,"comdlg32.lib") 
#pragma comment (lib,"advapi32.lib") 
#pragma comment (lib,"shell32.lib") 
#pragma comment (lib,"ole32.lib") 
#pragma comment (lib,"oleaut32.lib") 
#pragma comment (lib,"uuid.lib") 
#pragma comment (lib,"odbc32.lib") 
#pragma comment (lib,"odbccp32.lib") 
#pragma comment (lib,"shlwapi.lib") 

BROWSEINFO bi;
ITEMIDLIST *il;
OPENFILENAME ofn;
INITCOMMONCONTROLSEX iccex; 
SYSTEMTIME st;
HWND hWnd;
HMODULE hMainMod;
CHAR szPath[MAX_PATH];
CHAR appPath[MAX_PATH];
CHAR buffer[MAX_PATH];
CHAR Result[MAX_PATH];
CHAR szFile[MAX_PATH];
CHAR jours[7][10] = {"dimanche", "lundi","mardi","mercredi","jeudi","vendredi","samedi"};
CHAR mois[12][10] = {"janvier", "fevrier","mars", "avril", "mai", "juin","juillet","aout","septembre", "octobre", "novembre", "decembre"};
CHAR statut[2][12] = { "Desactivee","Activee" };
CHAR date[0x08];
CHAR nom[0x30];
CHAR Description[0x30];
DWORD version=MAKEWORD(21,1);
CHAR edition[0x16];
INT type=0;
TCHAR volumeName[MAX_PATH + 1];
TCHAR fileSysName[MAX_PATH + 1];
TCHAR driveType[MAX_PATH];
const DWORD RUN_ACTION_SHELLEX_FAILED = 0xFFFFFFFFFFFFFFFF;
const DWORD RUN_ACTION_SUCCESSFUL = 0x400;
const DWORD RUN_ACTION_CANCELLED = 0xC000013A;
unsigned __int64 i64NumberOfBytesUsed;
BYTE ConfigWim[8] = {1,1,1,1,0,0,1};
CHAR* ExtraireNomImage(CHAR* Chemin);
CHAR* GetDate();
VOID Horrodatage(CHAR* dataTexte);
VOID Horrodatage();
VOID SauverFichier();
VOID ExplorerDossiers(LPCTSTR Titre);
LPSTR ExecuteCommandeDos(LPSTR csExeName, LPSTR csArguments);
VOID GotoXT(COORD coordonnes, TCHAR Texte);
VOID GotoXT(INT X, INT Y, TCHAR Texte);
BOOL WINAPI SetConsoleIcon(HICON hIcon);
VOID ExchangeColors(CHAR* Message, INT CouleurMessage, CHAR* Notification, INT CouleurNotification);
DWORD GetVolumeInfo(LPCTSTR pDriveLetter, LPTSTR pDriveInfoBuffer, DWORD nDriveInfoSize);
VOID GetDiskSpaces(LPCTSTR pDriveLetter, LPTSTR pSpaceInfoBuffer, DWORD nSpaceInfoBufferSize);
CHAR* HarmoniseNom(CHAR* texte);
CHAR* CreerDescription(CHAR* texte);
INT MsgBox(CHAR* lpszText, CHAR* lpszCaption, DWORD dwStyle, INT lpszIcon);
VOID CreerRequeteDISM(CHAR* Cible, CHAR* Source, CHAR* Scratch, CHAR* Nom, CHAR* Descriptions);
VOID ExecuterRequeteDISM(CHAR* Tempon);

INT main(){
	hWnd = GetConsoleWindow();
	InitCommonControls();
	CreateStatusWindow(WS_CHILD | WS_VISIBLE, "(C)opyright Patrice Waechter-Ebling 2022-2023", hWnd, 6000);
	RECT rc;
	SetConsoleTitle(titre);
	hMainMod = GetModuleHandle(0);
	HICON ico = LoadIcon(0, IDI_EXCLAMATION);
	GetLocalTime(&st);
	SetConsoleIcon(ico);
	GetLocalTime(&st);
	GetClientRect(hWnd,&rc);
	DrawIcon(GetDC(hWnd), rc.right - 33, 0, LoadIcon(hMainMod, (LPCSTR)0x66));
	Horrodatage((CHAR*)"Bienvenue a ce prototype destine a aider les techniciens pour la capture d'images WIM afin de les rendre deployable via WDS!\n");
	printf("Bienvenue en ce %s,%d %s %d\n",jours[st.wDayOfWeek],st.wDay,mois[st.wMonth-1],st.wYear);
	HRESULT hrCoInitializeEx = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	DWORD drives = GetLogicalDrives();
	INT i = 0;
	TCHAR driveLetter[16];
	TCHAR driveInfo[MAX_PATH + 1];
	TCHAR spaceInfo[MAX_PATH + 1];
	DWORD dwGetVolInf = 0;
	int mnu = 0;
	Horrodatage((CHAR*)"Analyse des lecteurs possibles\n");
//	HWND hLecteur=CreateWindow("HDD",titre,,320,240,360,240)
	for (i = 0; i < 26; i++){
		if ((drives & (1 << i)) != 0){
			mnu++;
			_stprintf_s(driveLetter, sizeof(driveLetter) / sizeof(TCHAR), TEXT("%c:\\"), TEXT('A') + i);
			std::cout << mnu;ExchangeColors(driveLetter, 0x0D, (CHAR*)" = ", 0x0f); 
			GetDiskSpaces(driveLetter, spaceInfo, MAX_PATH + 1);
			dwGetVolInf = GetVolumeInfo(driveLetter, driveInfo, MAX_PATH + 1);
			ExchangeColors(spaceInfo, 0x0C, (CHAR*) " ", 0x0B);
			std::cout<< std::endl;
		}
	}

	Horrodatage((CHAR*)"Selectionnez le repertoire ou le lecteur a imager");
	ExplorerDossiers("Selectionnez le repertoire ou le lecteur a imager");
	sprintf(szFile, "Capture_WDS_%s.wim", nom);
	SauverFichier();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN|FOREGROUND_INTENSITY);
	CreerRequeteDISM(ofn.lpstrFile, Result,szPath,nom,Description);
	CoUninitialize();
	return 0x01;
}
VOID Horrodatage(CHAR* dataTexte) {
	CHAR temp[0xFF];
	GetLocalTime(&st);
	sprintf(temp, "%4d-%.2d-%.2d %.2d:%.2d:%.2d ", st.wYear, st.wMonth - 1, st.wDay, st.wHour, st.wMinute, st.wSecond);
	ExchangeColors(dataTexte, 0x0F, temp, 0x0E);
}
VOID Horrodatage() {
	GetLocalTime(&st);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0E);
	printf("\n%4d-%.2d-%.2d %.2d:%.2d:%.2d ", st.wYear, st.wMonth - 1, st.wDay, st.wHour, st.wMinute, st.wSecond);
}
VOID SauverFichier() {
	GetCurrentDirectory(0x103, szPath);
//	SHGetSpecialFolderPath(hWnd, szPath, CSIDL_PERSONAL, FALSE); // dossier mes documents desactive pour cause OneDrive 
	RECT rc;
	GetClientRect(ofn.hwndOwner, &rc);
	DrawIcon(GetDC(ofn.hwndOwner), rc.right - 33, 1, LoadIcon(hMainMod, (LPCSTR)IDI_ICON3));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile =szFile;
	ofn.nMaxFile = 0x104;
	ofn.lpstrInitialDir = szPath;
	ofn.lpstrFilter = "Fichiers Images\0*.wim\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = "Enregister l'image sous";
	ofn.lpstrFileTitle =(LPSTR) nom;
	ofn.nMaxFileTitle = 0x104;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT| OFN_EXPLORER| OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn) == TRUE) {
		HANDLE hf = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL); //acces au fichier
		Horrodatage();
		ExchangeColors(ofn.lpstrFile,0x0F,(CHAR*)" Destination de l'image:\n",0x0C);
		std::cout << std::endl;
	}else {
		MsgBox( (CHAR*)"Aucun nom de fichier n'a ete selectionne\nAbandon du programme", NULL, MB_ICONERROR | MB_OK,IDI_ICON3); PostQuitMessage(-6);
	}
}
VOID ExplorerDossiers(LPCTSTR Titre) {
	SetDlgItemText(hWnd, 0x1770, "En attente de l'utilisateur");
	bi.hwndOwner = hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = &buffer[0];
	bi.lpszTitle = Titre;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_STATUSTEXT | BIF_EDITBOX;
	bi.lpfn = NULL;
	il = SHBrowseForFolder(&bi);
	if (il == NULL) {MsgBox((CHAR*)"Aucun dossier ni lecteur n'a ete selectionne\nAbandon du programme", NULL,  MB_OK,IDI_ICON4); PostQuitMessage(-6);}
	if (SHGetPathFromIDList(il, &Result[0])) {
		strcpy(szPath, Result);
		strcpy(nom, ExtraireNomImage(Result));
		sprintf(buffer, "%s [%s]",titre ,nom);
		SetConsoleTitle(buffer);
		ExchangeColors(Result, 0x0B, (CHAR*)"Chemin de traitement: ", 0x0A);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		if (i64NumberOfBytesUsed > 1024) {SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED  | FOREGROUND_INTENSITY);printf(" CHARge a traiter % I64uTo", i64NumberOfBytesUsed / 1024);}
		else{SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);printf(" CHARge a traiter % I64uGo", i64NumberOfBytesUsed);}
		Horrodatage();printf("\t Parametres WIM");
		Horrodatage();ExchangeColors(nom, 0x0B, (CHAR*)"Le nom sera: ", 0x0A);
		strcpy(Description, CreerDescription(nom));
		Horrodatage();ExchangeColors(Description, 0x0B, (CHAR*)"La description sera: ", 0x0A);
		Horrodatage();ExchangeColors((CHAR*)"Maximum", 0x0B, (CHAR*)"Le niveau de compression sera: ", 0x0A);
		Horrodatage(); ExchangeColors(statut[ConfigWim[1]], 0x0B, (CHAR*)"La verification sera: ", 0x0A);
		Horrodatage(); ExchangeColors(statut[ConfigWim[0]], 0x0B, (CHAR*)"L'INTegrite sera: ", 0x0A);
		Horrodatage(); ExchangeColors(statut[ConfigWim[2]], 0x0B, (CHAR*)"Compatibilite WDS sera: ", 0x0A);
		Horrodatage(); ExchangeColors(statut[ConfigWim[4]], 0x0B, (CHAR*)"WIMBoot sera: ", 0x0A);
	}
}
VOID GotoXT(COORD coordonnes,TCHAR Texte){
	SetConsoleCursorPosition(
	GetStdHandle(STD_OUTPUT_HANDLE), coordonnes);
	printf((CHAR*)Texte);
}
VOID GotoXT(INT X,INT Y, TCHAR Texte) {
	COORD coordonnes;
	coordonnes.X = X;
	coordonnes.Y = Y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordonnes);
	printf((CHAR*)Texte);
}
LPSTR ExecuteCommandeDos(LPSTR csExeName, LPSTR csArguments) {
	CHAR tempEx[0xFE];
	sprintf((CHAR*)"cmd -c %s %s", csExeName, csArguments);
	SECURITY_ATTRIBUTES secattr;
	ZeroMemory(&secattr, sizeof(secattr));
	secattr.nLength = sizeof(secattr);
	secattr.bInheritHandle = TRUE;
	HANDLE rPipe, wPipe;
	CreatePipe(&rPipe, &wPipe, &secattr, 0);
	STARTUPINFO sInfo;
	ZeroMemory(&sInfo, sizeof(sInfo));
	PROCESS_INFORMATION pInfo;
	ZeroMemory(&pInfo, sizeof(pInfo));
	sInfo.cb = sizeof(sInfo);
	sInfo.dwFlags = STARTF_USESTDHANDLES;
	sInfo.hStdInput = NULL;
	sInfo.hStdOutput = wPipe;
	sInfo.hStdError = wPipe;
	CreateProcess(0, (LPSTR)tempEx, 0, 0, TRUE, NORMAL_PRIORITY_CLASS | CREATE_FORCEDOS/*CREATE_NO_WINDOW*/, 0, 0, &sInfo, &pInfo);
	CloseHandle(wPipe);
	CHAR buf[100];
	DWORD reDword;
	Horrodatage(tempEx);
	CHAR m_csOutput[0xff];
	BOOL res;
	do { res = ::ReadFile(rPipe, buf, 100, &reDword, 0);strcat(m_csOutput, buf);} while (res);
	Horrodatage(m_csOutput);
	return m_csOutput;
}
CHAR* ExtraireNomImage(CHAR* Chemin) {
	if (strlen(Chemin) < 4) {
		if (Chemin[2] == '\\') { //mode racine disque dur
			CHAR buff[0x40];
			TCHAR driveInfo[MAX_PATH + 1];
			sprintf(buff, "On est sur la racine du lecteur %.1s\nVoulez vous continuer?", &Chemin[0]);
			MsgBox(buff ,(CHAR*) "Option Capture Lecteur activee", MB_YESNO ,IDI_ICON5);
			GetVolumeInfo(Chemin, driveInfo, MAX_PATH + 1);
			sprintf(nom, "%s_%s", HarmoniseNom(volumeName), GetDate());
			Horrodatage();GetDiskSpaces(Chemin, driveInfo, MAX_PATH + 1);
		}
	}else{
		sprintf(nom, "%s_%s", PathFindFileNameA(Chemin), GetDate());
	}
	return nom;
}
CHAR* GetDate() {
	GetLocalTime(&st);
	sprintf(date, "%4d%.2d%.2d", st.wYear, st.wMonth - 1, st.wDay);
	return date;
}
BOOL WINAPI SetConsoleIcon(HICON hIcon){
	typedef BOOL(WINAPI* PSetConsoleIcon)(HICON);
	static PSetConsoleIcon pSetConsoleIcon = NULL;
	if (pSetConsoleIcon == NULL)	pSetConsoleIcon = (PSetConsoleIcon)GetProcAddress(GetModuleHandle("kernel32"), "SetConsoleIcon");
	if (pSetConsoleIcon == NULL)	return FALSE;
	return pSetConsoleIcon(hIcon);
}
VOID ExchangeColors(CHAR* Message, INT CouleurMessage,CHAR* Notification, INT CouleurNotification) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, CouleurNotification);
	printf(Notification);
	SetConsoleTextAttribute(hConsole, CouleurMessage);
	printf(Message);
}
DWORD GetVolumeInfo(LPCTSTR pDriveLetter, LPTSTR pDriveInfoBuffer, DWORD nDriveInfoSize){
	DWORD serialNumber = 0;
	DWORD maxCompLength = 0;
	DWORD fileSysFlags = 0;
	DWORD lastError = 0;
	BOOL bGetVolInf = FALSE;
	UINT iDrvType = 0;
	typedef enum tagGetVolumeInfoResult{RESULTS_SUCCESS = 0,RESULTS_GETVOLUMEINFORMATION_FAILED = 1	};
	size_t size = sizeof(driveType) / sizeof(TCHAR);
	bGetVolInf = GetVolumeInformation(pDriveLetter, volumeName, sizeof(volumeName) / sizeof(TCHAR),	&serialNumber, &maxCompLength, &fileSysFlags, fileSysName, sizeof(fileSysName) / sizeof(TCHAR));
	if (bGetVolInf == 0){
		lastError = GetLastError();
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		_stprintf_s(pDriveInfoBuffer, nDriveInfoSize, TEXT(" ** Erreur %d lors de la requete d'information sur le lecteur %.1s **"), lastError,pDriveLetter);
		return RESULTS_GETVOLUMEINFORMATION_FAILED;
	}
	iDrvType = GetDriveType(pDriveLetter);
	switch (iDrvType){
		case DRIVE_UNKNOWN:_stprintf_s(driveType, size, TEXT("%s"), TEXT("inconnu"));break;
		case DRIVE_NO_ROOT_DIR:_stprintf_s(driveType, size, TEXT("%s"), TEXT("Chemin?"));break;
		case DRIVE_REMOVABLE:_stprintf_s(driveType, size, TEXT("%s"), TEXT("disque dur externe"));break;
		case DRIVE_FIXED:_stprintf_s(driveType, size, TEXT("%s"), TEXT("disque dur interne"));break;
		case DRIVE_REMOTE:_stprintf_s(driveType, size, TEXT("%s"), TEXT("lecteur  reseau"));break;
		case DRIVE_CDROM:_stprintf_s(driveType, size, TEXT("%s"), TEXT("CD-ROM"));break;
		case DRIVE_RAMDISK:_stprintf_s(driveType, size, TEXT("%s"), TEXT("RAM-Disk"));break;
		default:_stprintf_s(driveType, size, TEXT("%s"), TEXT("indefini"));break;
	}
	if (_tcslen(volumeName) == 0){_stprintf_s(volumeName, sizeof(volumeName) / sizeof(TCHAR), TEXT("%s"), TEXT("Nom indefini"));}
	ExchangeColors(volumeName, 0x02, (CHAR*)" ", 0x0);
	ExchangeColors(driveType, 0x03, (CHAR*)" ", 0x0);
	ExchangeColors(fileSysName, 0x04, (CHAR*)" ", 0x0);
	return RESULTS_SUCCESS;
}
VOID GetDiskSpaces(LPCTSTR pDriveLetter, LPTSTR pSpaceInfoBuffer, DWORD nSpaceInfoBufferSize){
	unsigned __int64 i64TotalNumberOfBytes,i64TotalNumberOfFreeBytes,i64FreeBytesAvailableToCaller;
	BOOL bGetDiskFreeSpaceEx = FALSE;
	bGetDiskFreeSpaceEx = GetDiskFreeSpaceEx(pDriveLetter, (PULARGE_INTEGER)&i64FreeBytesAvailableToCaller, (PULARGE_INTEGER)&i64TotalNumberOfBytes, (PULARGE_INTEGER)&i64TotalNumberOfFreeBytes);
	if (bGetDiskFreeSpaceEx == TRUE){
		_stprintf_s(pSpaceInfoBuffer, nSpaceInfoBufferSize, TEXT(" Disponibilite: %I64u/%I64uGo"), i64TotalNumberOfFreeBytes / (1024 * 1024 * 1024), i64TotalNumberOfBytes / (1024 * 1024 * 1024));
		i64NumberOfBytesUsed = (i64TotalNumberOfBytes - i64TotalNumberOfFreeBytes) / (static_cast<unsigned long long>(1024 * 1024) * 1024);
	}
}
CHAR* HarmoniseNom(CHAR* texte) { 
	std::string str = texte;
	std::size_t found = str.find_first_of(" ");
	while (found != std::string::npos){str[found] = '_';found = str.find_first_of(" ", found + 1);}
	return(CHAR*)str.c_str();
}
CHAR* CreerDescription(CHAR* texte){
	std::string str = texte;
	std::size_t found = str.find_first_of("_");
	while (found != std::string::npos){str[found] = ' ';found = str.find_first_of("_", found + 1);}
	return(CHAR*)str.c_str();
}
INT MsgBox( CHAR* lpszText, CHAR* lpszCaption, DWORD dwStyle, INT lpszIcon) {
	MSGBOXPARAMS lpmbp;
	lpmbp.hInstance = hMainMod;
	lpmbp.cbSize = sizeof(MSGBOXPARAMS);
	lpmbp.hwndOwner = hWnd;
	lpmbp.dwLanguageId = MAKELANGID(0x0800, 0x0800); //par defaut celui du systeme
	lpmbp.lpszText = lpszText;
	if (lpszCaption != NULL) {
		lpmbp.lpszCaption = lpszCaption;
	}else{
		lpmbp.lpszCaption =titre;
	}
	lpmbp.dwStyle = dwStyle | 0x00000080L;
	if (lpszIcon == NULL) {
		lpmbp.lpszIcon = (LPCTSTR)IDI_ICON2;
	}else {
		lpmbp.lpszIcon = (LPCTSTR)lpszIcon;
	}
	lpmbp.lpfnMsgBoxCallback = 0;
	return  MessageBoxIndirect(&lpmbp);
}
VOID CreerRequeteDISM(CHAR* Cible,CHAR* Source, CHAR* Scratch, CHAR* Nom, CHAR* Descriptions) {
	CHAR Tempon[0x1000]{};
	if (strlen(Description) == 0) { strcpy(Descriptions, "Windows"); }
	sprintf(Tempon, "@dism /capture-image /imagefile:\"%s\" /capturedir:\"%s\" /ScratchDir:\"%s\" /name:\"%s\" /Description:\"%s\" /compress:maximum /checkintegrity /verify",
	Cible,Source, Scratch,Nom,Descriptions);
	if (strlen(Source) < 4) {
		if (Source[0] == 'C' || Source[0] == 'c') {
			strcat(Tempon, " /online");
			if (MsgBox((char*)"Voulez vous la rendre bootable?", (char*)titre, MB_YESNO | MB_DEFBUTTON2, IDI_ICON4) == 0x06) {
				strcat(Tempon, " /bootable");
			}
		}
	}
	std::cout << Tempon << std::endl;
	/system(Tempon);

	//ExecuterRequeteDISM(Tempon);
}
VOID ExecuterRequeteDISM(CHAR* Tempon){
	SHELLEXECUTEINFO sei = { 0 };
	DWORD exitCode = 0;
	ZeroMemory(&sei, sizeof(sei));
	sei.cbSize = sizeof(SHELLEXECUTEINFO);
	sei.fMask = SEE_MASK_NOCLOSEPROCESS;
	sei.hwnd = NULL;
	sei.lpVerb = NULL;
	sei.lpFile = Tempon;
	sei.lpParameters = NULL;
	sei.lpDirectory = NULL;
	sei.nShow = SW_SHOW;
	sei.hInstApp = NULL;
	if (ShellExecuteEx(&sei) == TRUE){
		WaitForSingleObject(sei.hProcess, INFINITE);
		GetExitCodeProcess(sei.hProcess, &exitCode);
		CloseHandle(sei.hProcess);
		return ;
	}
	return ;
}
