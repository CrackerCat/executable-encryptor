#include "StubConf.h"
#include"aplib.h"
#pragma comment(lib,"aplib.lib")

#pragma comment(linker,"/merge:.data=.text")
#pragma comment(linker,"/merge:.rdata=.text")
#pragma comment(linker, "/section:.text,RWE")

#define HASH_MessageBoxA      0x1E380A6A
#define HASH_LoadLibraryExA   0xC0D83287
#define HASH_ExitProcess      0x4FD18963
#define HASH_GetModuleHandleA 0xF4E2F2B2
#define HASH_VirtualAlloc     0x1EDE5967
#define HASH_VirtualProtect   0xEF64A41E
#define HASH_CopyMemory       0xA437DE59
#define HASH_GetProcAddress   0xBBAFDF85
#define HASH_lstrcmpA         0x7EAD1F70
#define HASH_CreateWindowExA  0x1FDAF55B
#define HASH_GetWindowTextA	  0x457BF544
#define HASH_DefWindowProcA   0x22E85CA4
#define HASH_UpdateWindow     0x9BB5D8DC
#define HASH_ShowWindow       0xDD8B5FB8
#define HASH_GetDlgItem       0x5D0CB479
#define HASH_TranslateMessage 0xE09980A2
#define HASH_DispatchMessageA 0x7A1506C2
#define HASH_RegisterClassA	 0x0BC05E32
#define HASH_GetMessageA	0x6106044B
#define HASH_PostQuitMessage 0xcaa94781

int GetFunAddrByHash(int nHashDigest);

#define declareFunc(name) decltype(name) *My_##name

#define DefineFuncPtr(name,base)My_##name = (decltype(name)*)GetFunAddrByHash(HASH_##name,base)

declareFunc(LoadLibraryExA);
declareFunc(MessageBoxA);
declareFunc(GetModuleHandleA);
declareFunc(VirtualAlloc);
declareFunc(VirtualProtect);
declareFunc(CreateWindowExA);
declareFunc(GetWindowTextA);
declareFunc(DefWindowProcA);	
declareFunc(UpdateWindow);
declareFunc(ShowWindow);
declareFunc(GetDlgItem);
declareFunc(TranslateMessage);
declareFunc(DispatchMessageA);
declareFunc(RegisterClassA);
declareFunc(GetMessageA);
declareFunc(PostQuitMessage);
declareFunc(ExitProcess);
declareFunc(GetProcAddress);
extern "C" _declspec(dllexport) StubConf g_conf = { 0 };

int WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
	return TRUE;

}
void myMemset(void * src, unsigned int sze){
	__asm{
		mov edi, src;/\]
		mov ecx, sze;
		mov al, 0;
		rep stosb;
	}
}

bool myStrcmp(char* a, char* b){
	bool flag = 0;
	__asm{
		xor eax, eax;
		mov esi, a;
		mov edi, b;
	sscmpss:
		lodsb;
		scasb;
		
		jne falsecmp;

		test al, al;
		jne sscmpss;
		je endeb;
		
	falsecmp:
		mov flag, 1;
	endeb:
	}
	return flag;
}
void GetHash(char*strFunName)
{
	UINT32 nDigest = 0;
	while (*strFunName)
	{
		nDigest = ((nDigest << 25) | (nDigest >> 7));
		nDigest = nDigest + *strFunName;
		*strFunName++;
	}
	//	printf("%s:%X", strFunName, nDigest);
	return;
}


bool Hash_CmpString(char *strFunName, int nHash)
{
	unsigned int nDigest = 0;
	while (*strFunName)
	{
		nDigest = ((nDigest << 25) | (nDigest >> 7));
		nDigest = nDigest + *strFunName;
		strFunName++;
	}
	return nHash == nDigest ? true : false;
}

int GetFunAddrByHash(int nHashDigest, HMODULE hModule)
{

	IMAGE_DOS_HEADER *pDOS = (IMAGE_DOS_HEADER*)hModule;
	IMAGE_NT_HEADERS *pNt = (IMAGE_NT_HEADERS *)((DWORD)hModule + pDOS->e_lfanew);
	IMAGE_DATA_DIRECTORY *pDataDir = (IMAGE_DATA_DIRECTORY *)&pNt->OptionalHeader.DataDirectory[0];
	IMAGE_EXPORT_DIRECTORY *pExport = (IMAGE_EXPORT_DIRECTORY *)((DWORD)hModule + pDataDir->VirtualAddress);

	// ��ȡ��������ϸ��Ϣ
	PDWORD pAddrOfFun = (PDWORD)(pExport->AddressOfFunctions + (DWORD)hModule);
	PDWORD pAddrOfNames = (PDWORD)(pExport->AddressOfNames + (DWORD)hModule);
	PWORD  pAddrOfOrdinals = (PWORD)(pExport->AddressOfNameOrdinals + (DWORD)hModule);

	// 4. �����Ժ��������Һ�����ַ������ѭ����ȡENT�еĺ���������Ϊ���Ժ�����
	//    Ϊ��׼����˲������޺�����������������봫��ֵ�Աȣ�����ƥ��������EAT
	//    ����ָ�������Ϊ��������ȡ�����ֵַ��
	DWORD dwFunAddr;
	for (DWORD i = 0; i < pExport->NumberOfNames; i++)
	{
		PCHAR lpFunName = (PCHAR)(pAddrOfNames[i] + (DWORD)hModule);
		if (Hash_CmpString(lpFunName, nHashDigest))
		{
			dwFunAddr = pAddrOfFun[pAddrOfOrdinals[i]] + (DWORD)hModule;
			break;
		}
		if (i == pExport->NumberOfNames - 1)
			return 0;
	}

	return dwFunAddr;
}
//void MemZero(PBYTE lpBuff, int nSize)
//{
//	__asm {
//		mov edi, lpBuff;
//			xor eax, eax;
//			mov ecx, nSize;
//			cld;
//			rep stosb;
//	}
//}


void getApi()
{

	char szUser32[] = { 'u', 's', 'e', 'r', '3', '2', '.', 'd', 'l', 'l', '\0' };
	char szbuf[] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd', '\0' };
	char szKer32[] = { 'K', 'e', 'r', 'n', 'e', 'l', '3', '2', '.', 'd', 'l', 'l', '\0' };
	// 2. ��ȡ�ؼ�ģ���ַ kernelbase ����kernel32
	HMODULE hKernelMoudle = 0;
	HMODULE hker32 = 0;
	_asm
	{
		push esi;
		mov esi, dword ptr fs : [0x30];   // esi = PEB�ĵ�ַ
		mov esi, [esi + 0x0C];            // esi = ָ��PEB_LDR_DATA�ṹ��ָ��
		mov esi, [esi + 0x1C];            // esi = ģ������ָ��InInit...List
		mov esi, [esi];                  // esi = ���������еĵڶ�����Ŀ
		mov esi, [esi + 0x08];           // 
		mov hKernelMoudle, esi;
		pop esi;
	}

	DefineFuncPtr(LoadLibraryExA, (HMODULE)hKernelMoudle);
	DefineFuncPtr(VirtualAlloc, (HMODULE)hKernelMoudle);
	DefineFuncPtr(VirtualProtect, (HMODULE)hKernelMoudle);

	HMODULE hUser32 = My_LoadLibraryExA(szUser32, 0, 0);
	DefineFuncPtr(MessageBoxA, hUser32);
	DefineFuncPtr(GetModuleHandleA, hKernelMoudle);
	DefineFuncPtr(GetWindowTextA, hUser32);
	DefineFuncPtr(DefWindowProcA, hUser32);
	DefineFuncPtr(UpdateWindow, hUser32);
	DefineFuncPtr(ShowWindow, hUser32);
	DefineFuncPtr(GetDlgItem, hUser32);
	DefineFuncPtr(CreateWindowExA, hUser32);
	DefineFuncPtr(TranslateMessage,hUser32);
	DefineFuncPtr(DispatchMessageA,hUser32);
	DefineFuncPtr(RegisterClassA, hUser32);
	DefineFuncPtr(GetMessageA, hUser32);
	DefineFuncPtr(PostQuitMessage, hUser32);
	DefineFuncPtr(ExitProcess, hUser32);
	DefineFuncPtr(GetProcAddress, hKernelMoudle);
	//My_MessageBoxA(0, szbuf, 0, 0);

	//hker32 = My_GetModuleHandleA(szKer32);

	//DWORD BaseAddr = (DWORD)My_GetModuleHandleA(0);

}
#define BUTTON_1 10001
#define EDIT 10002
LRESULT CALLBACK WndProc(
	HWND hWnd,
	UINT uMessage,
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch (uMessage)
	{
	case WM_CREATE:
	{LPCREATESTRUCT pct = (LPCREATESTRUCT)lParam;
	pct->hInstance;
	//3 ��ȡʵ���������3��ֱ�Ӷ���ȫ�ֱ�����ֵ��

	//1һ�㴰����ʲô�ؼ������Ǿ������ﴴ��
	My_CreateWindowExA(
		0,
		"button",
		"yes",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		100, 100, 80, 30,
		hWnd,
		(HMENU)BUTTON_1,//������OVERLAPPED��POPUP��ʱ���ǲ˵��������ڴ���
		//�Ӵ��ڵ�ʱ�������Ӵ��ڵ�ID
		0,
		NULL
		);

	My_CreateWindowExA(
		0,
		"edit",
		"",
		WS_CHILD | WS_VISIBLE,
		100, 140, 200, 30,
		hWnd,
		(HMENU)EDIT,//������OVERLAPPED��POPUP��ʱ���ǲ˵��������ڴ���
		//�Ӵ��ڵ�ʱ�������Ӵ��ڵ�ID
		0,
		NULL
		);
	break;
	}
	case WM_COMMAND:
	{
		DWORD dwId = LOWORD(wParam);
		switch (dwId)
	case BUTTON_1:
		{
			char buf[100];
			myMemset(buf, 100);
			//char* buf = (char*)My_VirtualAlloc(0,100,MEM_COMMIT,PAGE_READWRITE);
			HWND hTextWnd = My_GetDlgItem(hWnd, EDIT);
			My_GetWindowTextA(hTextWnd, buf, 100);
			if (myStrcmp(buf, (char*)g_conf.key2))
				My_MessageBoxA(0, "wrong", 0, 0);
			else{
				My_PostQuitMessage(0);
				My_ShowWindow(hWnd, SW_HIDE);
			}
		}
	}
	break;
	}
	return My_DefWindowProcA(hWnd, uMessage, wParam, lParam);//ϵͳĬ�ϴ�����Ϣ
}
//��ȡDOSͷ
IMAGE_DOS_HEADER* getDosHeader(_In_  char* pFileData) {
	return (IMAGE_DOS_HEADER *)pFileData;
}

// ��ȡNTͷ
IMAGE_NT_HEADERS* getNtHeader(_In_  char* pFileData) {
	return (IMAGE_NT_HEADERS*)(getDosHeader(pFileData)->e_lfanew + (SIZE_T)pFileData);
}

//��ȡ�ļ�ͷ
IMAGE_FILE_HEADER* getFileHeader(_In_  char* pFileData) {
	return &getNtHeader(pFileData)->FileHeader;
}

//��ȡ��չͷ
IMAGE_OPTIONAL_HEADER* getOptionHeader(_In_  char* pFileData) {
	return &getNtHeader(pFileData)->OptionalHeader;
}

// ��ȡָ�����ֵ�����ͷ
IMAGE_SECTION_HEADER* getSection(_In_ char* pFileData,
	_In_  const char* scnName)//��ȡָ�����ֵ�����
{
	// ��ȡ���θ�ʽ
	DWORD dwScnCount = getFileHeader(pFileData)->NumberOfSections;
	// ��ȡ��һ������
	IMAGE_SECTION_HEADER* pScn = IMAGE_FIRST_SECTION(getNtHeader(pFileData));
	char buff[10] = { 0 };
	// ��������
	for (DWORD i = 0; i < dwScnCount; ++i) {
		memcpy_s(buff, 8, (char*)pScn[i].Name, 8);
		// �ж��Ƿ�����ͬ������
		if (strcmp(buff, scnName) == 0)
			return pScn + i;
	}
	return NULL;
}



//_declspec(dllexport) DWORD g_oep = 0x12345678;


typedef void*(WINAPI*FnGetProcAddress)(HMODULE, const char*);
FnGetProcAddress pFnGetProcAddress;

typedef HMODULE(WINAPI*FnLoadLibraryA)(const char*);
FnLoadLibraryA pFnLoadLibraryA;

typedef DWORD(WINAPI*FnMessageBoxA)(HWND, char*, char*, DWORD);
FnMessageBoxA pFnMessageBoxA;


void doSDKprompt(){
	HINSTANCE	_hInstance = My_GetModuleHandleA(0);
	//1 ���һ�������ࣨ���ڻص�������������
	//typedef struct tagWNDCLASSW {
	//	UINT        style;
	//	WNDPROC     lpfnWndProc;
	//	int         cbClsExtra;
	//	int         cbWndExtra;
	//	HINSTANCE   hInstance;
	//	HICON       hIcon;
	//	HCURSOR     hCursor;
	//	HBRUSH      hbrBackground; 
	//	LPCWSTR     lpszMenuName;
	//	LPCWSTR     lpszClassName;
	//} WNDCLASSW, *PWNDCLASSW, NEAR *NPWNDCLASSW, FAR *LPWNDCLASSW;
	WNDCLASSA wc;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = WndProc;//���ڻص����������Դ�������Ϣ��
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = _hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_BACKGROUND;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ("Hello 15PB");
	//2 ע�����������
	My_RegisterClassA(&wc);
	//3 ���ݴ�����������������
	HWND hWnd = My_CreateWindowExA(
		0,
		("Hello 15PB"),    //ע��õĴ�������
		("Fisrt Window"),  //������
		WS_OVERLAPPEDWINDOW, //���ڷ��
		0, 0, 800, 400,         //���Ͻ�����ʹ��ڴ�С
		NULL,                //�����ھ��
		NULL,                //�˵����
		_hInstance,           //ʵ��������˴��������ĸ�����
		NULL                 //��Я���˲������뵽���ڻص�������
		);
	My_UpdateWindow(hWnd);
	My_ShowWindow(hWnd, SW_SHOW);

	//5 дһ����Ϣѭ��
	MSG msg = { 0, 0, 0, 0 };
	//GetMessage����ȡһ��WM_QUITE��Ϣ��ʱ�򣬾ͻ᷵��FALSE
	while (My_GetMessageA(&msg, NULL, NULL, NULL))
	{
		My_TranslateMessage(&msg);
		My_DispatchMessageA(&msg);
	}
}

void fixReloc(){
	StubConf* fixed;
	DWORD imgbase = 0;
	byte* relocPtr = 0;
	_asm{
		call getPC;
	getPC:
		pop eax;
		and eax, 0xFFFF0000;
		mov imgbase, eax;
		lea ebx, g_conf;
		and ebx, 0xFFFF;
		add eax, ebx;
		mov fixed, eax;
	}
	//relocPtr = fixed->selfReloc;

}

void DecryptTEA(unsigned int *firstChunk, unsigned int *secondChunk, unsigned int* key)
{
	unsigned int  sum = 0;
	unsigned int  y = *firstChunk;
	unsigned int  z = *secondChunk;
	unsigned int  delta = 0x9e3779b9;

	sum = delta << 3; //32�����㣬������2��5�η���16�����㣬������2��4�η���8�����㣬������2��3�η�

	for (int i = 0; i < 8; i++) //8������
	{
		z -= (y << 4) + key[2] ^ y + sum ^ (y >> 5) + key[3];
		y -= (z << 4) + key[0] ^ z + sum ^ (z >> 5) + key[1];
		sum -= delta;
	}

	*firstChunk = y;
	*secondChunk = z;
}
void DecryptBuffer(char* buffer, int size, unsigned int* key)
{
	char *p = buffer;

	int leftSize = size;

	while (p < buffer + size &&
		leftSize >= sizeof(unsigned int) * 2)
	{
		DecryptTEA((unsigned int *)p, (unsigned int *)(p + sizeof(unsigned int)), key);
		p += sizeof(unsigned int) * 2;

		leftSize -= sizeof(unsigned int) * 2;
	}
}



int ApDecode(char * pData, int nSize)
{
	unsigned int m_nSpaceSize = NULL;
	m_nSpaceSize = aP_workmem_size(nSize);               //���㹤���ռ��С
	char *	m_pWorkSpace = (char*)My_VirtualAlloc(0, m_nSpaceSize, MEM_COMMIT, PAGE_READWRITE);       //���빤���ռ�
	
	size_t orig_size = aPsafe_get_orig_size(pData);     //��ѹ�����ݵĴ�С
	char *data = (char*)My_VirtualAlloc(0, orig_size, 0x1000, 0x4);;

	int outlength = aPsafe_depack(pData, nSize, data, orig_size);
	__asm{
		mov edi, pData;
		mov esi, data;
		mov ecx, orig_size;
		rep movsb;
	}
	return 0;
}
void fixReloc(PIMAGE_BASE_RELOCATION pRel, DWORD oldimgbase, DWORD newimgbase){

	while (pRel->SizeOfBlock)
	{
		typedef struct {
			WORD offset : 12;
			WORD type : 4;
		}TypeOffset;

		DWORD dwCount = (pRel->SizeOfBlock - 8) / 2;
		TypeOffset* pTypeOfs =
			(TypeOffset*)(pRel + 1);									//find size

		DWORD oldMem = 0;
		for (DWORD i = 0; i<dwCount; ++i)
		{
			if (pTypeOfs[i].type == 3)									//type must  = 3
			{

				DWORD* pFixAddr = (DWORD*)(pRel->VirtualAddress + pTypeOfs[i].offset + newimgbase);
				//get addr(base + Section RVA + offset)
				My_VirtualProtect(pFixAddr, 4, PAGE_READWRITE, &oldMem);
				*pFixAddr = *pFixAddr + newimgbase - oldimgbase;
				My_VirtualProtect(pFixAddr, 4, oldMem, &oldMem);
			}
		}


		pRel = (IMAGE_BASE_RELOCATION*)
			((DWORD)pRel + pRel->SizeOfBlock);					//next reloc table

	}
}

void fixExEReloc(int imgbase){
	if (imgbase == g_conf.oldimgbase)
		return;
	PIMAGE_BASE_RELOCATION relocPtr = (PIMAGE_BASE_RELOCATION)(imgbase + g_conf.relocDir.VirtualAddress);

	fixReloc(relocPtr, g_conf.oldimgbase, imgbase);
}

void Mymemcpy(void*dst, void *src, int size)
{
	int dwsize = size;
	__asm
	{
		mov edi, dst;
		mov esi, src;
		mov ecx, dwsize;
		rep movsb;
	}
}
void fixIAT(DWORD offset, DWORD imgbase){
	PIMAGE_IMPORT_DESCRIPTOR pImport = PIMAGE_IMPORT_DESCRIPTOR(offset + imgbase);
	char code[50] = {
		0xEB, 0x05, 0x01, 0xC0, 0x01, 0xC9, 0xE8, 0x60, 0x9C, 0xE8, 0x00, 0x00, 0x00, 0x00, 0x58, 0x83,
		0xE8, 0x0C, 0x89, 0x40, 0x17, 0x9D, 0x61, 0xFF, 0x25, 0x78, 0x56, 0x34, 0x12
	};
	while (pImport->FirstThunk)				//get every table for different dlls
	{


		char* dllName = (char*)(pImport->Name + imgbase);
		PIMAGE_THUNK_DATA firstThunk = (PIMAGE_THUNK_DATA)(pImport->OriginalFirstThunk + imgbase);
		HMODULE dllHandle = My_LoadLibraryExA(dllName, 0, 0);
		DWORD* funcAddr = (DWORD*)(pImport->FirstThunk + imgbase);
		while (firstThunk->u1.AddressOfData)									//get each function
		{

			DWORD oldMem;
			My_VirtualProtect(funcAddr, 4, PAGE_READWRITE, &oldMem);
			if (!(firstThunk->u1.AddressOfData & 0x80000000))
			{
				PIMAGE_IMPORT_BY_NAME byname = (PIMAGE_IMPORT_BY_NAME)(firstThunk->u1.AddressOfData + imgbase);

				char* funcName = (char*)&byname->Name;
				DWORD realFuncAddr=(DWORD)My_GetProcAddress(dllHandle, funcName);
				byte* myIAT = (byte*)My_VirtualAlloc(0, 50, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				Mymemcpy(myIAT, code, 50);											
				*(DWORD*)(myIAT+2) = realFuncAddr;									//input func addr
				*funcAddr = (DWORD)myIAT;
																					//encrypt IAT
			}
			else
			{
				*funcAddr = (DWORD)My_GetProcAddress(dllHandle, (LPCSTR)&firstThunk->u1.AddressOfData);
			}
			My_VirtualProtect(funcAddr, 4, oldMem, &oldMem);
			funcAddr++;
			firstThunk++;
		}


		pImport++;
	}
	
	
}

void transferIAT(DWORD funcoffset, DWORD* IATaddr){

	
}
void decryptAll(DWORD imgbase){
	DWORD*	encryptAddr = g_conf.startAddr;
	DWORD*	sizeArray = g_conf.size;
	while (*encryptAddr)
	{
		DecryptBuffer((char*)*encryptAddr + imgbase, *sizeArray, g_conf.key2);
		ApDecode((char*)*encryptAddr + imgbase, *sizeArray);
		encryptAddr++;
		sizeArray++;
	}
}
extern "C" _declspec(dllexport) void _declspec(naked) start()
{

	getApi(); // ��ȡAPI
	doSDKprompt();
	int imgbase;
	_asm{
		call getPC;
	getPC:
		pop eax;
		and eax, 0xFFFF0000;
		mov imgbase, eax;
	}
	/*while (*encryptAddr)
	{
		encryptAddr = g_conf.startAddr;
		sizeArray = g_conf.size;
		DecryptBuffer((char*)*encryptAddr + imgbase, *sizeArray, g_conf.key2);
		ApDecode((char*)*encryptAddr + imgbase, *sizeArray);
		encryptAddr++;
		sizeArray++;
	}*/
	decryptAll(imgbase);
	fixExEReloc(imgbase);
	fixIAT(g_conf.importDir.VirtualAddress, imgbase);
	g_conf.oep += imgbase;
	__asm{
		add esp, 0x100;
		pop ebp;
	}
	_asm jmp g_conf.oep;
	
}
