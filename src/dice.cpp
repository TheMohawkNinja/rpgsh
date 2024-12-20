#include <climits>
#include <filesystem>
#include <fstream>
#include "../headers/define.h"
#include "../headers/dice.h"
#include "../headers/functions.h"
#include "../headers/output.h"
#include "../headers/text.h"

int Dice::get_value(std::string d, long unsigned int start, std::string terminator, bool allow_sign, bool required)
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
				throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
			}
		}

		try
		{
			std::stoi(value_str);
			return std::stoi(value_str);
		}
		catch(...)
		{
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
		}
	}
	else
	{
		if(start == d.length())
		{
			if(!required) return 0;

			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
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
				throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
			}
		}

		try
		{
			std::stoi(value_str);
			return std::stoi(value_str);
		}
		catch(...)
		{
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
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
Dice::Dice(std::string dice_str)
{
	//
	// Determine if we are working with an explicit constructor or an string
	//

	std::string d(1,DICE_SIGIL);
	if(dice_str.substr(0,2) == (d+"{"))// Explicit constructor
	{
		if(dice_str.find("}") == std::string::npos)
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);

		// Get number of commas in explicit constructor
		unsigned int commas = 0;
		for(const auto& c : dice_str)
			if(c == ',') commas++;

		if(commas && commas != 2)
		{
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
		}
		else if(!commas)//If no commas, assume list
		{
			List = dice_str.substr(2,dice_str.length()-3);
			exit(-1);
		}

		try
		{
			Quantity = std::stoi(dice_str.substr(2,dice_str.find(",")-2));
			dice_str = dice_str.substr(dice_str.find(",")+1,dice_str.length()-(dice_str.find(",")+1));
			Faces = std::stoi(dice_str.substr(0,dice_str.find(",")));
			dice_str = dice_str.substr(dice_str.find(",")+1,dice_str.length()-(dice_str.find(",")+1));
			Modifier = std::stoi(dice_str.substr(0,dice_str.find("}")));
		}
		catch(...)
		{
			throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
		}
	}
	else if(islower(dice_str[0]) && dice_str[1] == '{')
	{
		throw std::runtime_error(E_INVALID_EXPLICIT_CONSTRUCTOR);
	}
	else if(dice_str.substr(0,1) != "d")// Implicit string constructor with explicitly defined Quantity
	{
		Quantity = get_value(dice_str,0,"d",false,true);
		Faces = get_value(dice_str,dice_str.find("d",0) + 1,"",false,true);
		Modifier = get_value(dice_str,dice_str.find(std::to_string(Faces),dice_str.find("d",0)) + std::to_string(Faces).length(),"",true,false);
	}
	else// Implicit string construct with implicit Quantity = 1
	{
		Quantity = 1;
		Faces = get_value(dice_str,dice_str.find("d",0) + 1,"",false,true);
		Modifier = get_value(dice_str,dice_str.find(std::to_string(Faces),dice_str.find("d",0)) + std::to_string(Faces).length(),"",true,false);
	}
}
Dice::Dice(unsigned int _Quantity, unsigned int _Faces, int _Modifier)
{
	Quantity = _Quantity;
	Faces = _Faces;
	Modifier = _Modifier;
	just_show_total = true;
}
Dice::Dice(std::string dice_str, bool _just_show_rolls, bool _just_show_total, bool _is_list, std::string _count_expr, unsigned int _count)
{
	if(!_is_list)
	{
		if(dice_str.substr(0,1) != "d")
		{
			Quantity = get_value(dice_str,0,"d",false,true);
			Faces = get_value(dice_str,dice_str.find("d",0) + 1,"",false,true);
			Modifier = get_value(dice_str,dice_str.find(std::to_string(Faces),dice_str.find("d",0)) + std::to_string(Faces).length(),"",true,false);
		}
		else
		{
			Quantity = 1;
			Faces = get_value(dice_str,dice_str.find("d",0) + 1,"",false,true);
			Modifier = get_value(dice_str,dice_str.find(std::to_string(Faces),dice_str.find("d",0)) + std::to_string(Faces).length(),"",true,false);
		}
	}
	else
	{
		dice_list = dice_str;
	}

	just_show_rolls = _just_show_rolls;
	just_show_total = _just_show_total;
	is_list = _is_list;
	count_expr = _count_expr;
	count = _count;
}
Dice::Dice(unsigned int _Quantity, unsigned int _Faces, int _Modifier, bool _just_show_rolls, bool _just_show_total, bool _is_list, std::string _count_expr, unsigned int _count)
{
	Quantity = _Quantity;
	Faces = _Faces;
	Modifier = _Modifier;
	just_show_rolls = _just_show_rolls;
	just_show_total = _just_show_total;
	is_list = _is_list;
	count_expr = _count_expr;
	count = _count;
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
	catch(...){List += b;}

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
void Dice::roll()
{
	if(!std::filesystem::exists(random_seed_path))
	{
		if(std::filesystem::exists(backup_random_seed_path))
		{
			random_seed_path = backup_random_seed_path;
		}
		else
		{
			output(Error,"Random seed path file doesn't exist at either \"%s\" or \"%s\". If system corruption is not suspected, please report your system's RNG/pRNG file to the RPGSH Github issues page.",random_seed_path,backup_random_seed_path);
		}
	}
	if(Quantity > 0 && Faces > 0)
	{
		for(unsigned int i=0; i<Faces; i++)
		{
			result_quantity.push_back(0);
		}

		if(!just_show_rolls && !just_show_total)
		{
			fprintf(stdout,"Rolling a %s%d%s-sided die %s%d%s time(s) with a modifier of %s%d%s...\n\n",TEXT_WHITE,Faces,TEXT_NORMAL,TEXT_WHITE,Quantity,TEXT_NORMAL,TEXT_WHITE,Modifier,TEXT_NORMAL);
		}

		std::ifstream fs(random_seed_path);
		std::string data, seed;
		unsigned int result;

		while(seed.length() < Quantity)
		{
			std::getline(fs,data);
			seed += data;
		}

		int total = 0;
		std::string color;

		for(unsigned int i=0; i<Quantity; i++)
		{
			std::srand((int)seed[i] * (int)seed[i] - i);
			std::srand(std::rand());//Mitigates apparent roll biasing when Faces%result=0
			result = std::rand() % Faces + 1;
			result_quantity[result-1]++;
			total += result;

			if(count_expr != "")
			{
				if(count_expr == "=" && result == count){total_count++;}
				if(count_expr == "<" && result < count){total_count++;}
				if(count_expr == ">" && result > count){total_count++;}
				if(count_expr == "<=" && result <= count){total_count++;}
				if(count_expr == ">=" && result >= count){total_count++;}
				if(count_expr == "!=" && result != count){total_count++;}
			}

			if(result == 1)
			{
				color = TEXT_RED;
			}
			else if(result == Faces)
			{
				color = TEXT_GREEN;
			}
			else
			{
				color = TEXT_WHITE;
			}

			if(just_show_rolls)
			{
				fprintf(stdout,"%s%d%s\n",color.c_str(),result,TEXT_NORMAL);
			}
			else if(!just_show_total)
			{
				fprintf(stdout,"Roll");
				for(long unsigned int space=std::to_string(Quantity).length(); space>=std::to_string(i+1).length(); space--)
				{
					fprintf(stdout," ");
				}
				fprintf(stdout,"%d:",(i+1));
				for(long unsigned int space=std::to_string(Faces).length(); space>=std::to_string(result).length(); space--)
				{
					fprintf(stdout," ");
				}
				fprintf(stdout,"%s%d%s\n",color.c_str(),result,TEXT_NORMAL);
			}
		}
		if(!just_show_rolls && !just_show_total)
		{
			fprintf(stdout,"\n");
			fprintf(stdout,"Natural total:\t%s%d%s\n",TEXT_WHITE,total,TEXT_NORMAL);

			if(Modifier != 0)
			{
				fprintf(stdout,"Modifier:\t%s%d%s\n",TEXT_WHITE,Modifier,TEXT_NORMAL);
				total += Modifier;
			}
			fprintf(stdout,"Total:\t\t%s%d%s\n",TEXT_WHITE,total,TEXT_NORMAL);

			if(count_expr != "")
			{
				fprintf(stdout,"\n");
				fprintf(stdout,"Rolls %s%d:\t%s%d%s\n",count_expr.c_str(),count,TEXT_WHITE,total_count,TEXT_NORMAL);
			}
		}
		else if(just_show_total)
		{
			total += Modifier;
			fprintf(stdout,"%d\n",total);
		}
		fs.close();
	}
	else if(is_list)//TODO Reimplement with list member variable
	{
		std::string list_line = "";
		std::string list_path = "";
		std::vector<std::string> values;
		std::ifstream fs;

		if(dice_list.substr(0,1) != "/" && dice_list.substr(0,2) != "./" && dice_list.substr(0,3) != "../")//If not specifying path, assume it's something in the dice-lists folder
		{
			list_path = dice_lists_dir+dice_list;
		}
		else//Otherwise, user is specifying a path outside of the dice-lists folder, so just go with what the user inputted
		{
			list_path = dice_list;
		}

		if(std::filesystem::exists(list_path))
		{
			fs.open(list_path);
			if(!fs.good())
			{
				output(Error,"Unable to open dice list \"%s\".",list_path.c_str());
			}
		}
		else
		{
			output(Error,"Dice list \"%s\" does not exist.",list_path.c_str());
			exit(-1);
		}

		while(std::getline(fs,list_line))//Get values from dice list, making sure to ignore blank space and comments
		{
			if(list_line != "" && list_line[0] != COMMENT)
			{
				values.push_back(list_line);
				list_line = "";
			}
		}
		fs.close();

		fs.open(random_seed_path);
		std::string data, seed;
		int result;
		do
		{
			std::getline(fs,data);
		}while((int)data[0] == 0); //Prevents crashing on first seed when (int)seed[0] == 0
		fs.close();

		seed += data;
		std::srand((int)seed[0] * (int)seed[seed.length()-1]);
		std::srand(std::rand());//Mitigates apparent roll biasing when Faces%result=0
		result = std::rand() % values.size();

		if(!just_show_total && !just_show_rolls)
		{
			fprintf(stdout,"[Roll result: %s%d%s]\n",TEXT_WHITE,result,TEXT_NORMAL);
		}
		if(!just_show_rolls)
		{
			fprintf(stdout,"%s\n",values[result].c_str());
		}
		else
		{
			fprintf(stdout,"%d\n",result);
		}
	}
}
void Dice::test()
{
	std::string plus = TEXT_WHITE;
	plus += "+";
	Quantity = 100000;
	Faces = 20;
	roll();
	fprintf(stdout,"\n");
	fprintf(stdout,"\t┌───────────────────────────────────────┐\n");
	fprintf(stdout,"\t│ %s%s%sTEST RESULTS:%s                         │\n",TEXT_ITALIC,TEXT_BOLD,TEXT_YELLOW,TEXT_NORMAL);
	fprintf(stdout,"\t│                                       │\n");
	for(unsigned int i=0; i<Faces; i++)
	{
		float percent = ((float)result_quantity[i]/((float)Quantity/(float)Faces))*100-100;
		fprintf(stdout,"\t│ # of %d's: %s%d (%s%s%.2f%%%s from perfect) │\n",(i+1),(i<9)?" ":"",result_quantity[i],(percent>=0)?plus.c_str():"",TEXT_WHITE,percent,TEXT_NORMAL);
	}
	fprintf(stdout,"\t└───────────────────────────────────────┘\n");
}
