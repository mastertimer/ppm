#include <iostream>
#include <string>
#include <vector>

using i64 = long long;

std::wstring command_decomposition(const std::wstring& cmd, std::vector<std::wstring>& parameters)
{
    std::wstring command_name;
	i64 i0 = cmd.size();
	// пропуск первых пробелов
	for (i64 i = 0; i < (i64)cmd.size(); i++)
		if ((cmd[i] != L' ') && (cmd[i] != L'\t'))
		{
			i0 = i;
			break;
		}
	// вычленение command_name
	for (i64 i = i0; i < (i64)cmd.size(); i++)
		if ((cmd[i] == L' ') || (cmd[i] == L'\t'))
		{
			command_name = cmd.substr(i0, i - i0);
			break;
		}
	if (command_name.empty()) command_name = cmd.substr(i0, cmd.size() - i0);
	// вычленение параметров
	int rez = 0; // 0 - пробелы 1 - набор символов 2 - строка "safasf asf"
	int start_p = 0;
	for (i64 i = i0 + command_name.size(); i < (i64)cmd.size(); i++)
	{
		wchar_t c = cmd[i];
		if (rez == 0)
		{
			if ((c == L' ') || (c == L'\t')) continue;
			start_p = i;
			rez = (c == L'"') ? 2 : 1;
			continue;
		}
		if (rez == 1) if ((c != L' ') && (c != L'\t')) continue;
		if (rez == 2) if (c != L'"') continue;
		parameters.push_back(cmd.substr(start_p, i - start_p + (rez == 2)));
		rez = 0;
	}
	if (rez == 1) parameters.push_back(cmd.substr(start_p, cmd.size() - start_p));
	return command_name;
}

int main()
{
	setlocale(LC_ALL, "RU");
    for (;;)
    {
        std::wcout << L">";
        std::wstring cmd;
		std::wcin >> cmd;
		std::vector<std::wstring> parameters;
		std::wstring command_name = command_decomposition(cmd, parameters);
		if (command_name == L"a")
		{

		}
		else
			std::wcout << L"команда не найдена\n";
	}
}
