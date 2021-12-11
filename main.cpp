#include <iostream>
#include <chrono>

#include "arithmetic_coding.h"
#include "compression.h"

std::wstring test_file = L"e:\\ppm\\data\\t110521.txt";

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

void test_arithmetic_coding(std::vector<std::wstring>& parameters)
{
	std::wcout << L"файл: " << test_file << std::endl;
	std::vector<uchar> data, data2;
	_bit_vector res;
	if (!load_file(test_file, data))
	{
		std::wcout << L"ошибка загрузки!";
		return;
	}
	std::wcout << L"размер:            " << std::to_wstring(data.size()) << std::endl;
	double com1 = 0.0, com2 = 0.0;
	double com = information(data, &com1, &com2);
	std::wcout << L"информация:        " << double_to_wstring(com, 1) << L" (" + double_to_wstring(com1, 1) << L" + " <<
		double_to_wstring(com2, 1) + L")" << std::endl;
	std::wcout << L"идеал:             " << double_to_wstring(size_arithmetic_coding(data), 1) << std::endl;
	std::wcout << L"идеал0:            " + double_to_wstring(size_arithmetic_coding(data, 0.01), 1) << std::endl;

	i64 n = 1;
	if (!parameters.empty()) n = std::stoi(parameters[0]);
	if (n < 1) n = 1;
	if (n > 1) std::wcout << std::to_wstring(n) << L" испытаний" << std::endl;

	i64 mindt = 1000000000;
	i64 maxdt = 0;
	i64 summdt = 0;

	for (i64 i = 0; i < n; i++)
	{
		auto tt = std::chrono::high_resolution_clock::now();
		arithmetic_coding(data, res);
		std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - tt;
		i64 dtt = dt.count() / 1000;
		if (dtt < mindt) mindt = dtt;
		if (dtt > maxdt) maxdt = dtt;
		summdt += dtt;
	}


	double v = res.size() / 8.0;
	std::wcout << L"arithmetic_coding: " << double_to_wstring(v, 1) << std::endl;
	std::wcout << L"разница:           " << double_to_wstring(v - com, 1) << std::endl;
	std::wcout << L"среднее время, мксек:      " << std::to_wstring(summdt / n) << std::endl;
	std::wcout << L"минимальное время, мксек:  " << std::to_wstring(mindt) << std::endl;
	std::wcout << L"максимальное время, мксек: " << std::to_wstring(maxdt) << std::endl;

	mindt = 1000000000;
	maxdt = 0;
	summdt = 0;

	for (i64 i = 0; i < n; i++)
	{
		auto tt = std::chrono::high_resolution_clock::now();
		arithmetic_decoding(res, data2);
		std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - tt;
		i64 dtt = dt.count() / 1000;
		if (dtt < mindt) mindt = dtt;
		if (dtt > maxdt) maxdt = dtt;
		summdt += dtt;
	}
	if (data == data2)
		std::wcout << L"расжатие норма" << std::endl;
	else
		std::wcout << L"!!ошибка!! расжатый файл не равен исходному!" << std::endl;
	std::wcout << L"среднее время, мксек:      " << std::to_wstring(summdt / n) << std::endl;
	std::wcout << L"минимальное время, мксек:  " << std::to_wstring(mindt) << std::endl;
	std::wcout << L"максимальное время, мксек: " << std::to_wstring(maxdt) << std::endl;
}

void test_ppm(std::vector<std::wstring>& parameters)
{
	std::wcout << L"файл: " << test_file << std::endl;
	std::vector<uchar> data, data2, res;
	if (!load_file(test_file, data))
	{
		std::wcout << L"ошибка загрузки!" << std::endl;
		return;
	}
	std::wcout << L"размер: " << std::to_wstring(data.size()) << std::endl;

	i64 n = 0;
	if (!parameters.empty()) n = std::stoi(parameters[0]);
	if (n < 0) n = 0;
	std::wcout << L"порядок = " << std::to_wstring(n) << std::endl;

	i64 mindt = 1000000000;
	i64 maxdt = 0;
	i64 summdt = 0;

	{
		auto tt = std::chrono::high_resolution_clock::now();
		ppm(data, res, n);
		std::chrono::nanoseconds dt = std::chrono::high_resolution_clock::now() - tt;
		i64 dtt = dt.count() / 1000;
		if (dtt < mindt) mindt = dtt;
		if (dtt > maxdt) maxdt = dtt;
		summdt += dtt;
	}

	double v = res.size();
	std::wcout << L"ppm:    " << double_to_wstring(v, 0) << std::endl;
	std::wcout << L"время, мксек:  " << std::to_wstring(mindt) << std::endl;
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
			test_arithmetic_coding(parameters);
		else if (command_name == L"ppm")
			test_ppm(parameters);
		else
			std::wcout << L"команда не найдена\n";
	}
}
