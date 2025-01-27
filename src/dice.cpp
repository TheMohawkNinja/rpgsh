#include <climits>
#include <filesystem>
#include <fstream>
#include "../headers/define.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/text.h"

int Dice::getValue(std::string d, long unsigned int start, std::string terminator, bool allow_sign, bool required)
{
	std::string value_str = "";
	if(terminator != "")
	{
		for(int i=start; d.substr(i,1)!=terminator; i++)
		{
			try
			{
				if(!allow_sign)
				{
					if(d.substr(i,1) == "+" || d.substr(i,1) == "-")
						break;

					std::stoi(d.substr(i,1));
					value_str += d.substr(i,1);
				}
				else
				{
					if(d.substr(i,1) != "+" && d.substr(i,1) != "-")
						std::stoi(d.substr(i,1));

					value_str += d.substr(i,1);
				}
			}
			catch(...)
			{
				throw;
			}
		}

		try
		{
			std::stoi(value_str);
			return std::stoi(value_str);
		}
		catch(...)
		{
			throw;
		}
	}
	else
	{
		if(start == d.length())
		{
			if(!required) return 0;
			throw;
		}
		for(long unsigned int i=start; i<d.length(); i++)
		{
			try
			{
				if(!allow_sign)
				{
					if(d.substr(i,1) == "+" || d.substr(i,1) == "-")
						break;
					std::stoi(d.substr(i,1));
					value_str += d.substr(i,1);
				}
				else
				{
					if(d.substr(i,1) != "+" && d.substr(i,1) != "-")
						std::stoi(d.substr(i,1));
					value_str += d.substr(i,1);
				}
			}
			catch(...)
			{
				throw;
			}
		}

		try
		{
			std::stoi(value_str);
			return std::stoi(value_str);
		}
		catch(...)
		{
			throw;
		}
	}
}

Dice::Dice(){}
Dice::Dice(const Dice& b)
{
	Quantity = b.Quantity;
	Faces = b.Faces;
	Modifier = b.Modifier;
	List = b.List;
}
Dice::Dice(std::string str)
{
	//
	// Determine if we are working with an explicit constructor or an string
	//

	std::string d(1,DICE_SIGIL);
	if(left(str,2) == (d+"{"))// Explicit constructor
	{
		if(findu(str,'}') == std::string::npos)
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

		// Get number of commas in explicit constructor
		unsigned int commas = countu(str,',');

		if(commas && commas != 2)
		{
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
		}
		else if(!commas)//If no commas, assume list
		{
			List = str.substr(2,str.length()-3);
			exit(0);
		}

		try
		{
			Quantity = std::stoi(str.substr(2,findu(str,',')-2));
			str = str.substr(findu(str,',')+1,str.length()-(findu(str,',')+1));
			Faces = std::stoi(left(str,findu(str,',')));
			str = str.substr(findu(str,',')+1,str.length()-(findu(str,',')+1));
			Modifier = std::stoi(left(str,findu(str,',')));
		}
		catch(std::runtime_error& e)
		{
			throw e.what();
		}
	}
	else if(islower(str[0]) && str[1] == '{')
	{
		throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
	}
	else if(str[0] != 'd')// Implicit string constructor with explicitly defined Quantity
	{
		try
		{
			Quantity = getValue(str,0,"d",false,true);
			Faces = getValue(str,findu(str,'d')+1,"",false,true);
			Modifier = getValue(str,findu(str,std::to_string(Faces),findu(str,'d'))+std::to_string(Faces).length(),"",true,false);
		}
		catch(...)
		{
			List = str;
		}
	}
	else// Implicit string construct with implicit Quantity = 1
	{
		try
		{
			Quantity = 1;
			Faces = getValue(str,findu(str,'d')+1,"",false,true);
			Modifier = getValue(str,findu(str,std::to_string(Faces),findu(str,'d'))+std::to_string(Faces).length(),"",true,false);
		}
		catch(...)
		{
			List = str;
		}
	}
}
Dice::Dice(unsigned int _Quantity, unsigned int _Faces, int _Modifier)
{
	Quantity = _Quantity;
	Faces = _Faces;
	Modifier = _Modifier;
}

Dice::operator std::string() const
{
	if(List == "")
		return std::string(1,DICE_SIGIL)+"{"+
		       std::to_string(Quantity)+","+
		       std::to_string(Faces)+","+
		       std::to_string(Modifier)+"}";
	else
		return std::string(1,DICE_SIGIL)+"{"+List+"}";
}
Dice::operator bool() const
{
	return Quantity > 0;
}
Dice& Dice::operator = ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator = (const std::string b)
{
	try
	{
		Dice rhs = Dice(b);
		Quantity = rhs.Quantity;
		Faces = rhs.Faces;
		Modifier = rhs.Modifier;
		List = rhs.List;
	}
	catch(...)
	{
		throw std::runtime_error(E_INVALID_OPERATION);
	}

	return *this;
}
Dice& Dice::operator = (const Var b)
{
	Dice rhs = b.Value;
	return *this;
}
Dice& Dice::operator = (const Dice b)
{
	Quantity = b.Quantity;
	Faces = b.Faces;
	Modifier = b.Modifier;
	List = b.List;

	return *this;
}
Dice& Dice::operator = ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator = ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator += (const int b)
{
	Modifier += b;
	return *this;
}
Dice& Dice::operator += (const std::string b)
{
	try{*this += Dice(b);}
	catch(...){List = mergeQuotes(List + b);}

	return *this;
}
Dice& Dice::operator += (const Var b)
{
	try{*this += std::stoi(b.Value);}
	catch(...){*this += b.Value;}

	return *this;
}
Dice& Dice::operator += (const Dice b)
{
	if(Faces == b.Faces)
		Quantity += b.Quantity;
	else
		throw std::runtime_error(E_INVALID_OPERATION);

	return *this;
}
Dice& Dice::operator += ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator += ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator -= (const int b)
{
	Modifier -= b;
	return *this;
}
Dice& Dice::operator -= ([[maybe_unused]] const std::string b)
{
	try{*this -= Dice(b);}
	catch(...){throw std::runtime_error(E_INVALID_OPERATION);}

	return *this;
}
Dice& Dice::operator -= (const Var b)
{
	try{*this -= std::stoi(b.Value);}
	catch(...){*this -= b.Value;}

	return *this;
}
Dice& Dice::operator -= (const Dice b)
{
	if(Faces == b.Faces)
		Quantity -= b.Quantity;
	else
		throw std::runtime_error(E_INVALID_OPERATION);

	return *this;
}
Dice& Dice::operator -= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator -= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator *= (const int b)
{
	Quantity *= b;
	return *this;
}
Dice& Dice::operator *= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator *= (const Var b)
{
	try{*this *= std::stoi(b.Value);}
	catch(...){*this *= b.Value;}

	return *this;
}
Dice& Dice::operator *= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator *= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator *= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator /= (const int b)
{
	Quantity /= b;
	return *this;
}
Dice& Dice::operator /= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator /= (const Var b)
{
	try{*this /= std::stoi(b.Value);}
	catch(...){*this /= b.Value;}

	return *this;
}
Dice& Dice::operator /= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator /= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator /= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator ^= ([[maybe_unused]] const int b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator ^= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator ^= (const Var b)
{
	try{*this ^= std::stoi(b.Value);}
	catch(...){*this ^= b.Value;}

	return *this;
}
Dice& Dice::operator ^= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator ^= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator ^= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator %= (const int b)
{
	Quantity %= b;
	return *this;
}
Dice& Dice::operator %= ([[maybe_unused]] const std::string b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator %= (const Var b)
{
	try{*this %= std::stoi(b.Value);}
	catch(...){*this %= b.Value;}

	return *this;
}
Dice& Dice::operator %= ([[maybe_unused]] const Dice b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator %= ([[maybe_unused]] const Wallet b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice& Dice::operator %= ([[maybe_unused]] const Currency b)
{
	throw std::runtime_error(E_INVALID_OPERATION);
}
Dice Dice::operator + (const int b)
{
	return *this += b;
}
Dice Dice::operator + (const std::string b)
{
	return *this += b;
}
Dice Dice::operator + (const Var b)
{
	return *this += b;
}
Dice Dice::operator + (const Dice b)
{
	return *this += b;
}
Dice Dice::operator + (const Wallet b)
{
	return *this += b;
}
Dice Dice::operator + (const Currency b)
{
	return *this += b;
}
Dice Dice::operator - (const int b)
{
	return *this -= b;
}
Dice Dice::operator - (const std::string b)
{
	return *this -= b;
}
Dice Dice::operator - (const Var b)
{
	return *this -= b;
}
Dice Dice::operator - (const Dice b)
{
	return *this -= b;
}
Dice Dice::operator - (const Wallet b)
{
	return *this -= b;
}
Dice Dice::operator - (const Currency b)
{
	return *this -= b;
}
Dice Dice::operator * (const int b)
{
	return *this *= b;
}
Dice Dice::operator * (const std::string b)
{
	return *this *= b;
}
Dice Dice::operator * (const Var b)
{
	return *this *= b;
}
Dice Dice::operator * (const Dice b)
{
	return *this *= b;
}
Dice Dice::operator * (const Wallet b)
{
	return *this *= b;
}
Dice Dice::operator * (const Currency b)
{
	return *this *= b;
}
Dice Dice::operator / (const int b)
{
	return *this /= b;
}
Dice Dice::operator / (const std::string b)
{
	return *this /= b;
}
Dice Dice::operator / (const Var b)
{
	return *this /= b;
}
Dice Dice::operator / (const Dice b)
{
	return *this /= b;
}
Dice Dice::operator / (const Wallet b)
{
	return *this /= b;
}
Dice Dice::operator / (const Currency b)
{
	return *this /= b;
}
Dice Dice::operator ^ (const int b)
{
	return *this ^= b;
}
Dice Dice::operator ^ (const std::string b)
{
	return *this ^= b;
}
Dice Dice::operator ^ (const Var b)
{
	return *this ^= b;
}
Dice Dice::operator ^ (const Dice b)
{
	return *this ^= b;
}
Dice Dice::operator ^ (const Wallet b)
{
	return *this ^= b;
}
Dice Dice::operator ^ (const Currency b)
{
	return *this ^= b;
}
Dice Dice::operator % (const int b)
{
	return *this %= b;
}
Dice Dice::operator % (const std::string b)
{
	return *this %= b;
}
Dice Dice::operator % (const Var b)
{
	return *this %= b;
}
Dice Dice::operator % (const Dice b)
{
	return *this %= b;
}
Dice Dice::operator % (const Wallet b)
{
	return *this %= b;
}
Dice Dice::operator % (const Currency b)
{
	return *this %= b;
}
bool Dice::operator == ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Dice::operator == (const std::string& b) const
{
	try{return *this == Dice(b);}
	catch(...){return false;}
}
bool Dice::operator == (const Var& b) const
{
	try{return *this == b.Value;}
	catch(...){return false;}
}
bool Dice::operator == (const Dice& b) const
{
	return (Quantity == b.Quantity &&
		Faces == b.Faces &&
		Modifier == b.Modifier &&
		List == b.List);
}
bool Dice::operator == ([[maybe_unused]] const Wallet& b) const
{
	return false;
}
bool Dice::operator == ([[maybe_unused]] const Currency& b) const
{
	return false;
}
bool Dice::operator < ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Dice::operator < (const std::string& b) const
{
	try{return *this < Dice(b);}
	catch(...){return false;}
}
bool Dice::operator < (const Var& b) const
{
	try{return *this < b.Value;}
	catch(...){return false;}
}
bool Dice::operator < (const Dice& b) const
{
	return Quantity < b.Quantity;
}
bool Dice::operator < ([[maybe_unused]] const Wallet& b) const
{
	return false;
}
bool Dice::operator < ([[maybe_unused]] const Currency& b) const
{
	return false;
}
bool Dice::operator > ([[maybe_unused]] const int& b) const
{
	return false;
}
bool Dice::operator > (const std::string& b) const
{
	try{return *this > Dice(b);}
	catch(...){return false;}
}
bool Dice::operator > (const Var& b) const
{
	try{return *this > b.Value;}
	catch(...){return false;}
}
bool Dice::operator > (const Dice& b) const
{
	return Quantity > b.Quantity;
}
bool Dice::operator > ([[maybe_unused]] const Wallet& b) const
{
	return false;
}
bool Dice::operator > ([[maybe_unused]] const Currency& b) const
{
	return false;
}
bool Dice::operator <= (const int& b) const
{
	return (*this < b || *this == b);
}
bool Dice::operator <= (const std::string& b) const
{
	try{return *this <= Dice(b);}
	catch(...){return (*this < b || *this == b);}
}
bool Dice::operator <= (const Var& b) const
{
	try{return *this <= b.Value;}
	catch(...){return (*this < b || *this == b);}
}
bool Dice::operator <= (const Dice& b) const
{
	return Quantity <= b.Quantity;
}
bool Dice::operator <= (const Wallet& b) const
{
	return (*this < b || *this == b);
}
bool Dice::operator <= (const Currency& b) const
{
	return (*this < b || *this == b);
}
bool Dice::operator >= (const int& b) const
{
	return (*this > b || *this == b);
}
bool Dice::operator >= (const std::string& b) const
{
	try{return *this >= Dice(b);}
	catch(...){return (*this > b || *this == b);}
}
bool Dice::operator >= (const Var& b) const
{
	try{return *this >= b.Value;}
	catch(...){return (*this > b || *this == b);}
}
bool Dice::operator >= (const Dice& b) const
{
	return Quantity >= b.Quantity;
}
bool Dice::operator >= (const Wallet& b) const
{
	return (*this > b || *this == b);
}
bool Dice::operator >= (const Currency& b) const
{
	return (*this > b || *this == b);
}
bool Dice::operator != (const int& b) const
{
	return !(*this == b);
}
bool Dice::operator != (const std::string& b) const
{
	return !(*this == b);
}
bool Dice::operator != (const Var& b) const
{
	return !(*this == b);
}
bool Dice::operator != (const Dice& b) const
{
	return !(*this == b);
}
bool Dice::operator != (const Wallet& b) const
{
	return !(*this == b);
}
bool Dice::operator != (const Currency& b) const
{
	return !(*this == b);
}
bool Dice::operator && (const int b)
{
	return bool(*this) && (b != 0);
}
bool Dice::operator && (const std::string b)
{
	return bool(*this) && (b != "" && stringcasecmp(b,"false"));
}
bool Dice::operator && (const Var b)
{
	return bool(*this) && bool(b);
}
bool Dice::operator && (const Dice b)
{
	return bool(*this) && bool(b);
}
bool Dice::operator && (const Wallet b)
{
	return bool(*this) && bool(b);
}
bool Dice::operator && (const Currency b)
{
	return bool(*this) && bool(b);
}
bool Dice::operator || (const int b)
{
	return bool(*this) || (b != 0);
}
bool Dice::operator || (const std::string b)
{
	return bool(*this) || (b != "" && stringcasecmp(b,"false"));
}
bool Dice::operator || (const Var b)
{
	return bool(*this) || bool(b);
}
bool Dice::operator || (const Dice b)
{
	return bool(*this) || bool(b);
}
bool Dice::operator || (const Wallet b)
{
	return bool(*this) || bool(b);
}
bool Dice::operator || (const Currency b)
{
	return bool(*this) || bool(b);
}
Dice& Dice::operator ++ (int)
{
	Modifier++;
	return *this;
}
Dice& Dice::operator -- (int)
{
	Modifier--;
	return *this;
}

std::string Dice::dice() const
{
	std::string ret = "";
	if(Quantity)
	{
		ret += std::to_string(Quantity);
	}
	ret += "d"+std::to_string(Faces);
	if(Modifier)
	{
		ret += (Modifier>0) ? "+" : "";
		ret += Modifier;
	}
	return ret;
}
const char* Dice::c_str() const
{
	return std::string(*this).c_str();
}
RollResults Dice::roll()
{
	RollResults results;
	const char* e_prng_path;

	if(std::filesystem::exists(prng_path))
		e_prng_path = prng_path;
	else if(std::filesystem::exists(backup_prng_path))
		e_prng_path = backup_prng_path;
	else
		throw std::runtime_error(E_NO_PRNG_FILE);

	if(List != "")
	{
		std::string list_line = "";
		std::ifstream fs;

		//Check if we are using explicitly define list or built-in list
		if(List[0] != '/' && left(List,2) != "./" && left(List,3) != "../")
			List = dice_lists_dir+List;

		if(!std::filesystem::exists(List)) throw std::runtime_error(E_FILE_NOT_FOUND);
		fs.open(List);
		if(!fs.good()) throw std::runtime_error(E_BAD_FS);

		std::vector<std::string> list_values;
		std::string line;
		while(std::getline(fs,line))
		{
			if(line != "" && findu(line,COMMENT) == std::string::npos)
				list_values.push_back(line);
			else if(line != "")
				list_values.push_back(left(line,findu(line,COMMENT)));
		}
		fs.close();

		fs.open(e_prng_path);
		std::string data, seed;
		do
		{
			std::getline(fs,data);
		}while((int)data[0] == 0);//Prevents crashing on first seed when (int)seed[0] == 0
		fs.close();

		seed += data;
		std::srand((int)seed[0] * (int)seed[seed.length() - 1]);
		std::srand(std::rand());//Mitigates apparent roll biasing when Faces%result=0
		long pick = std::rand() % list_values.size();
		results.push_back({std::to_string(pick),list_values[pick]});

		return results;
	}
	else if(Quantity > 0 && Faces > 0)
	{
		std::ifstream fs(e_prng_path);
		std::string data, seed;

		while(seed.length() < Quantity)
		{
			std::getline(fs,data);
			seed += data;
		}

		unsigned int natural = 0;

		for(unsigned int i=0; i<Quantity; i++)
		{
			std::srand((int)seed[i] * (int)seed[i] - i);
			std::srand(std::rand());//Mitigates apparent roll biasing when Faces%result=0
			natural = std::rand() % Faces + 1;

			results.push_back({std::to_string(natural),std::to_string(natural+Modifier)});
		}

		fs.close();

		return results;
	}

	return results; //Supress -Wreturn-type
}
