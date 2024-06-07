#include <cstring>
#include <iostream>
#include <fstream>

#include "../include/tdata.h"

int tkey_comparer::operator()(
	tkey const &lhs,
	tkey const &rhs) const
{
	if (lhs != rhs)
	{
		return lhs < rhs ? -1 : 1;
	}
	return 0;
}

tvalue::tvalue():
        _count_detailes(0),
        _name("")
{ }

tvalue::tvalue(
	uint64_t count_detailes,
	std::string const &name):
        _count_detailes(count_detailes),
        _name(name)
{ }


// tvalue::~tvalue()
// {
// 	delete[] _name;
// }

// tvalue::tvalue(
// 	tvalue const &other):
// 		_count_detailes(other._count_detailes),
// 		_name(new char[strlen(other._name) + 1])
// {
// 	strcpy(_name, other._name);
// }

// tvalue tvalue::operator=(
// 	tvalue const &other)
// {
// 	if (this != &other)
// 	{
// 		delete[] _name;
// 		_name = (new char[strlen(other._name) + 1]);
// 		strcpy(_name, other._name);
// 	}
	
// 	return *this;
// }

// tvalue::tvalue(
// 	tvalue &&other) noexcept:
// 		_count_detailes(other._count_detailes),
// 		_name(other._name)
// {
// 	other._name = nullptr;
// }

// tvalue tvalue::operator=(
// 	tvalue &&other) noexcept
// {
// 	if (this != &other)
// 	{
// 		_count_detailes = other._count_detailes;
// 		_name = other._name;
		
// 		other._name = nullptr;
// 	}
	
// 	return *this;
// }

ram_tdata::ram_tdata(
	tvalue const &value):
		value(value)
{ }

ram_tdata::ram_tdata(
	tvalue &&value):
		value(std::move(value))
{ }


file_tdata::file_tdata(
	long file_pos):
		_file_pos(file_pos)
{ }

void file_tdata::serialize(
	std::string const &path,
	tkey const &key,
	tvalue const &value,
	bool update_flag)
{
	std::ofstream data_stream(path, std::ios::app | std::ios::binary);
	
    if (!data_stream.is_open())
    {
        throw std::ios::failure("Cannot open the file");
    }
	
	// TODO UPDATE WITH BIGGER DATA
	
	if (_file_pos == -1 || update_flag)
	{
		_file_pos = data_stream.tellp();
	}
	else
	{
		data_stream.seekp(_file_pos, std::ios::beg);
	}
	
	size_t login_len = key.size();
	size_t name_len = value._name.size();
	
    data_stream.write(reinterpret_cast<char const *>(&login_len), sizeof(size_t));
    data_stream.write(key.c_str(), sizeof(char) * login_len);
    data_stream.write(reinterpret_cast<char const *>(&value._count_detailes), sizeof(int64_t));
    data_stream.write(reinterpret_cast<char const *>(&name_len), sizeof(size_t));
    data_stream.write(value._name.c_str(), sizeof(char) * name_len);
	data_stream.flush();
	
	if (data_stream.fail())
	{
		throw std::ios::failure("An error occured while serializing data");
	}
}

tvalue file_tdata::deserialize(
	std::string const &path) const
{
	std::ifstream data_stream(path, std::ios::binary);
    if (!data_stream.is_open())
    {
        throw std::ios::failure("Cannot open the file");
    }
	if (_file_pos == 1)
	{
		throw std::logic_error("Invalid pointer to data");
	}

    data_stream.seekg(_file_pos, std::ios::beg);
	
	tvalue value;
	size_t login_len, name_len;
	
	data_stream.read(reinterpret_cast<char *>(&login_len), sizeof(size_t));
	data_stream.seekg(login_len, std::ios::cur);
    data_stream.read(reinterpret_cast<char *>(&value._count_detailes), sizeof(size_t));
	data_stream.read(reinterpret_cast<char *>(&name_len), sizeof(size_t));
	
	value._name.resize(name_len);
	for (auto &ch : value._name)
	{
		data_stream.read(&ch, sizeof(char));
	}
	
	if (data_stream.fail())
	{
		throw std::ios::failure("An error occured while deserializing data");
	}
	
    return value;
}
