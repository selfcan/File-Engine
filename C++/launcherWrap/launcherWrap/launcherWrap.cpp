﻿#define _CRT_SECURE_NO_WARNINGS

#include <corecrt_io.h>
#include <direct.h>
#include <Windows.h>
#include <ShlObj.h>
#include "resource.h"
#include <TlHelp32.h>
#include <iostream>
#include <string>
#include "zip/zip.h"
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "User32.lib")

//#define TEST

#ifndef TEST
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#endif

constexpr auto* g_file_engine_zip_name = "File-Engine.zip";

char g_close_signal_file[1000];
char g_file_engine_exe_path[1000];
char g_file_engine_working_dir[1000];
char g_update_signal_file[1000];
char g_new_file_engine_exe_path[1000];
#ifdef TEST
int g_check_time_count = 10;
#else
int g_check_time_count = 50;
#endif

constexpr auto* const g_proc_name = TEXT("File-Engine-x64.exe");

int g_restart_count = 0;
std::time_t g_restart_time = std::time(nullptr);
bool g_is_restart_on_release_file = false;

bool is_close_exist();
bool find_process(const WCHAR* procName);
void restart_file_engine(bool);
bool release_resources();
void extract_zip();
bool is_file_exist(const char* file_path);
void release_all();
bool is_dir_not_exist(const char* path);
void update();
void init_path();

int main()
{
	if (find_process(g_proc_name))
	{
		return 0;
	}
	init_path();

	std::cout << "file-engine-x64.exe path :  " << g_file_engine_exe_path << std::endl;
	std::cout << "file-engine working dir: " << g_file_engine_working_dir << std::endl;
	std::cout << "new file-engine-x64.exe path: " << g_new_file_engine_exe_path << std::endl;
	std::cout << "update signal file: " << g_update_signal_file << std::endl;
	std::cout << "close signal file : " << g_close_signal_file << std::endl;
	auto loop_count = 0;
	if (is_dir_not_exist(g_file_engine_working_dir))
	{
		if (_mkdir(g_file_engine_working_dir))
		{
			std::string msg;
			msg.append("Create dir ").append(g_file_engine_working_dir).append(" failed");
			MessageBoxA(nullptr, msg.c_str(), "Error", MB_OK);
			return 0;
		}
	}
	restart_file_engine(true);
	while (!is_close_exist())
	{
		loop_count++;
		if (loop_count > g_check_time_count)
		{
			if (!find_process(g_proc_name))
			{
				std::cout << "File-Engine process not exist" << std::endl;
				if (is_close_exist())
				{
					// 再次检测，防止已经关闭后重启
					break;
				}
				restart_file_engine(false);
			}
			loop_count = 0;
		}
		Sleep(100);
	}
	return 0;
}

inline void init_path()
{
	char current_dir[1000];
	GetModuleFileNameA(nullptr, current_dir, sizeof current_dir);
	std::string tmp_current_dir(current_dir);
	strcpy_s(current_dir, tmp_current_dir.substr(0, tmp_current_dir.find_last_of('\\')).c_str());

	std::string file_engine_exe_dir_string(current_dir);
	file_engine_exe_dir_string += "\\data\\";
	strcpy_s(g_file_engine_working_dir, file_engine_exe_dir_string.c_str());

	file_engine_exe_dir_string += "File-Engine-x64.exe";
	strcpy_s(g_file_engine_exe_path, file_engine_exe_dir_string.c_str());

	std::string file_engine_directory(g_file_engine_working_dir);
	file_engine_directory += "tmp\\closeDaemon";
	strcpy_s(g_close_signal_file, file_engine_directory.c_str());

	std::string new_file_engine_path(g_file_engine_working_dir);
	new_file_engine_path += "tmp\\File-Engine-x64.exe";
	strcpy_s(g_new_file_engine_exe_path, new_file_engine_path.c_str());

	std::string update_signal_file(g_file_engine_working_dir);
	update_signal_file += "user\\update";
	strcpy_s(g_update_signal_file, update_signal_file.c_str());
}

/**
 * 释放所有文件
 */
void release_all()
{
	if (release_resources())
	{
		extract_zip();
		remove(g_file_engine_zip_name);
	}
}

/**
 * 释放File-Engine.zip文件到当前文件夹
 */
bool release_resources()
{
	const HRSRC hRsrc = FindResourceA(nullptr, MAKEINTRESOURCEA(IDR_ZIP2), "ZIP");
	if (nullptr == hRsrc)
	{
		return false;
	}
	const DWORD size = SizeofResource(nullptr, hRsrc);
	if (0 == size)
	{
		return false;
	}
	const HGLOBAL hGlobal = LoadResource(nullptr, hRsrc);
	if (nullptr == hGlobal)
	{
		return false;
	}
	const LPVOID pBuffer = LockResource(hGlobal);
	if (nullptr == pBuffer)
	{
		return false;
	}
	FILE* fp;
	if (fopen_s(&fp, g_file_engine_zip_name, "wb"))
	{
		return false;
	}
	if (nullptr != fp)
	{
		if (size != fwrite(pBuffer, 1, size, fp))
		{
			fclose(fp);
			return false;
		}
		fclose(fp);
		return true;
	}
	return false;
}

/**
 * 解压File-Engine.zip
 */
inline void extract_zip()
{
	zip_extract(g_file_engine_zip_name, g_file_engine_working_dir, nullptr, nullptr);
}

/**
 * 重启程序
 */
void restart_file_engine(bool isIgnoreCloseFile)
{
	if (isIgnoreCloseFile)
	{
		remove(g_close_signal_file);
	}
	else
	{
		if (is_close_exist())
		{
			return;
		}
	}
	if (is_file_exist(g_update_signal_file))
	{
		update();
	}
	std::time_t tmp_restart_time = std::time(nullptr);
	// 这次重启距离上次时间超过了10分钟，视为正常重启
	const std::time_t tmp = tmp_restart_time - g_restart_time;
	if (tmp > 600)
	{
		g_restart_count = 0;
		g_is_restart_on_release_file = false;
	}
	if (g_is_restart_on_release_file && g_restart_count >= 1)
	{
		MessageBoxA(nullptr, "Launch failed after 3 retries", "Error", MB_OK);
		exit(-1);
	}
	if (g_restart_count > 3 || !is_file_exist(g_file_engine_exe_path))
	{
		release_all();
		g_is_restart_on_release_file = true;
		g_restart_count = 0;
	}
	g_restart_count++;
	ShellExecuteA(nullptr, "open", g_file_engine_exe_path, nullptr, g_file_engine_working_dir, SW_SHOWNORMAL);
}

/**
 * 更新File-Engine
 */
void update()
{
	CopyFileA(g_new_file_engine_exe_path, g_file_engine_exe_path, false);
	remove(g_update_signal_file);
}

/**
 * 检查关闭标志是否存在
 */
bool is_close_exist()
{
	return is_file_exist(g_close_signal_file);
}

bool is_dir_not_exist(const char* path)
{
	return ENOENT == _access(path, 0);
}

bool is_file_exist(const char* file_path)
{
	FILE* fp = nullptr;
	fopen_s(&fp, file_path, "rb");
	if (fp != nullptr){
		fclose(fp);
		return true;
	}
	return false;
}

/**
 * 查找File-Engine进程是否存在
 */
bool find_process(const WCHAR* procName)
{
	PROCESSENTRY32 pe;
	DWORD id = 0;
	auto* const hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe))
		return 0;
	while (true)
	{
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE)
			break;
		if (wcscmp(pe.szExeFile, procName) == 0)
		{
			id = pe.th32ProcessID;
			break;
		}
	}
	CloseHandle(hSnapshot);
	return id;
}
