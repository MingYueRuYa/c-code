#ifndef task_scheduler_h
#define task_scheduler_h

#include "commonpack.h"
#include "json.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>

using json = nlohmann::json;

struct Config
{
	wstring app_name = L"";
	wstring app_path = L"";
	wstring app_desc = L"";
	wstring app_param = L"";
	wstring app_start_dir = L"";
	std::vector<pair<int, int>> start_time = { };
};

Config config;

bool DeleteTaskScheduler(int argc, char*argv[])
{
	string param = argv[1];
	if (param == "-d")
	{
		bool result = XIBAO::TaskScheduler::DeleteTaskScheduler(config.app_name);
		if (result)
		{
			std::cout << XIBAO::StringHelper::to_string(wstring(L"ɾ���ƻ�����ɹ�:")+config.app_name) << std::endl;
		}
	}
	return 0;
}


bool InitConfig()
{
	// ��ȡ��ǰ������Ŀ¼
	return true;
}

bool ReadConfig()
{
	std::ifstream input("config.json");
	if (input.is_open())
	{
    json j;
    input >> j;
		string str;
		if (j.contains("app_name"))
		{
      str = j["app_name"].get<string>();
      config.app_name = XIBAO::StringHelper::to_wstring(str);
		}

		if (j.contains("app_path"))
		{
      str = j["app_path"].get<string>();
      config.app_path = XIBAO::StringHelper::to_wstring(str);
		}


		if (j.contains("app_desc"))
		{
			str = j["app_desc"].get<string>();
			config.app_desc = XIBAO::StringHelper::to_wstring(str);
		}

		if (j.contains("app_param"))
		{
			str = j["app_param"].get<string>();
			config.app_param = XIBAO::StringHelper::to_wstring(str);
		}
		
		if (j.contains("app_start_dir"))
		{
			str = j["app_start_dir"].get<string>();
			config.app_start_dir = XIBAO::StringHelper::to_wstring(str);
		}

		if (j.contains("start_time"))
		{
			json o = j["start_time"];
			for (json::iterator it = o.begin(); it != o.end(); ++it) {
				int hour = (*it)["hour"].get<int>();
				int min = (*it)["min"].get<int>();
				config.start_time.push_back({hour, min});
			}
		}
	}
	else
	{
		cout << XIBAO::StringHelper::to_string(L"û���ҵ�����config.json�ļ�") << endl;
		return false;
	}

	return true;
}

bool CreateTaskScheduler()
{
	// 1����ȡ��ǰ��·������װ�ɾ���·��
	// 2��json��ʽ����
	bool result = XIBAO::TaskScheduler::DeleteTaskScheduler(config.app_name);
	if (result)
	{
		std::cout << XIBAO::StringHelper::to_string(wstring(L"ɾ��ԭ���ļƻ�����ɹ�:")+config.app_name) << std::endl;
	}

	XIBAO::TaskScheduler task_scheduler(config.app_path,
		config.app_name,
		config.app_desc, config.app_start_dir,
		config.app_param, config.start_time);

	std::cout << "------------------------------------" << std::endl;
	std::cout <<  XIBAO::StringHelper::to_string(L"��������:") << XIBAO::StringHelper::to_string(task_scheduler.GetTaskName()) << std::endl;
	std::cout << XIBAO::StringHelper::to_string(L"app·��:") << XIBAO::StringHelper::to_string(task_scheduler.GetAppPath()) << std::endl;
	std::cout << XIBAO::StringHelper::to_string(L"����:") << XIBAO::StringHelper::to_string(task_scheduler.GetParameter()) << std::endl;
	std::cout << XIBAO::StringHelper::to_string(L"����·��:") << XIBAO::StringHelper::to_string(task_scheduler.GetWorkDir()) << std::endl;
	std::cout << XIBAO::StringHelper::to_string(L"��������:") << XIBAO::StringHelper::to_string(task_scheduler.GetTaskDescription()) << std::endl;
	auto time_vec = task_scheduler.GetTimeVec();
	string time_str = "";
	for (auto& time_pair : time_vec)
	{
		time_str += std::to_string(time_pair.first) + ":" + std::to_string(time_pair.second) + "	";
	}
	std::cout << XIBAO::StringHelper::to_string(L"ִ��ʱ��:") << time_str << std::endl;
	std::cout << "------------------------------------" << std::endl;

	if (task_scheduler.CreateTaskScheduler())
	{

		std::cout << XIBAO::StringHelper::to_string(wstring(L"�����µ�����ɹ�:")+config.app_name) << std::endl;
	}

	return true;
}

#endif // task_scheduler_h