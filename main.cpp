#include <windows.h>
#include <iostream>

__int64 get_fpak_platform_file()
{
	auto delegate = *(__int64*)((uintptr_t)GetModuleHandleA(nullptr) + 0xBFA0C28); 
	if (!delegate) return 0;
	return *(__int64*)(delegate + 24);
}

void bypass_pak_signing()
{
	auto delegate = (uint8_t*)((uintptr_t)GetModuleHandleA(nullptr) + 0xC2E98F8);
	DWORD old_protect;
	VirtualProtect(delegate, 0x20, PAGE_READWRITE, &old_protect);
	memset(delegate, 0, 0x20);
	VirtualProtect(delegate, 0x20, old_protect, &old_protect);

	auto fpak = get_fpak_platform_file();
	if (fpak) {
		auto bsigned_ptr = (uint8_t*)(fpak + 48);
		VirtualProtect(bsigned_ptr, 1, PAGE_READWRITE, &old_protect);
		*bsigned_ptr = 0;
		VirtualProtect(bsigned_ptr, 1, old_protect, &old_protect);
	}
}

void mount_custom_pak(const wchar_t* path, int priority) 
{
	auto fpak = get_fpak_platform_file();
	if (!fpak) return;

	typedef bool(__fastcall* fn_mount)(__int64, const wchar_t*, int, const wchar_t*, bool, bool);
	auto mount = (fn_mount)((uintptr_t)GetModuleHandleA(nullptr) + 0x283E150);
	mount(fpak, path, priority, nullptr, true, false);
}

bool __stdcall DllMain(void* hinstDLL, std::uint32_t dwReason, void* lpReserved) 
{
	if (dwReason != DLL_PROCESS_ATTACH) return true;

	bypass_pak_signing();
	mount_custom_pak(L"C:\\mods\\pakchunk1-Windows_P.pak", 10000);
	mount_custom_pak(L"C:\\mods\\pakchunk2-Windows_P.pak", 10001);
	mount_custom_pak(L"C:\\mods\\pakchunk3-Windows_P.pak", 10002);

	return true;
}