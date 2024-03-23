#pragma once

#include <map>
#include <string>

template <class T>
class RPGSH_OBJ
{
	private:
		std::map<std::string, T> Properties;

	public:

	T& operator [] (const std::string s)
	{
		return Properties[s];
	}

	using iterator = typename std::map<std::string, T>::const_iterator;

	typename std::map<std::string, T>::const_iterator begin() const
	{
		return Properties.begin();
	}
	typename std::map<std::string, T>::const_iterator end() const
	{
		return Properties.end();
	}

	RPGSH_OBJ(){}
	RPGSH_OBJ(std::string s, T var)
	{
		Properties[s] = var;
	}
	RPGSH_OBJ(std::map<std::string, T> map)
	{
		for(auto const& [key, value] : map)
		{
			Properties[key] = value;
		}
	}

	void clear()
	{
		Properties.clear();
	}
	int size()
	{
		return Properties.size();
	}
};
