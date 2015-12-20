#ifndef STRING_H
#define STRING_H

#include <Arduino.h>

namespace std
{
	class string
	{
		public:
			string(const char* str=NULL):str_m(str)
			{}

			string(const String& str):str_m(str)
			{}

			string(const char* str,const uint32_t size):str_m(str)
			{
				reserve(size);
			}

			inline uint32_t size() const
			{
				return str_m.length();
			}

			inline void clear()
			{
				*this="";
			}

			inline void erase(const uint32_t start,const uint32_t size)
			{
				str_m.remove(start,size);
			}

			inline void reserve(const uint32_t size)
			{
				str_m.reserve(size);
			}

			inline const char* c_str() const
			{
				return str_m.c_str();
			}

			inline std::string substr(const uint32_t start,const uint32_t size)
			{
				return std::string(str_m.substring(start,size));
			}

			inline char& operator[](uint32_t ii)
			{
				return str_m[ii];
			}

			inline char operator[](const uint32_t ii) const
			{
				 return str_m[ii];
			}

			inline void print(HardwareSerial& serial) const
			{
				serial.print(str_m);
			}

			inline void println(HardwareSerial& serial) const
			{
				serial.println(str_m);
			}

			friend inline std::string operator+(const std::string& lhs,const std::string& rhs)
			{
				return lhs.str_m+rhs.str_m;
			}

			friend inline std::string& operator+=(std::string& lhs,const std::string& rhs)
			{
				lhs.str_m+=rhs.str_m;
				return lhs;
			}

			friend inline std::string operator+(const std::string& lhs,const char rhs)
			{
				return lhs.str_m+rhs;
			}

			friend inline std::string& operator+=(std::string& lhs,const char rhs)
			{
				lhs.str_m+=rhs;
				return lhs;
			}

		private:
			String str_m;
	};

	inline int16_t stoi(const std::string& str)
	{
		if(str.c_str()==NULL)
			return 0;

		return atoi(str.c_str());
	}

	inline int32_t stol(const std::string& str)
	{
		if(str.c_str()==NULL)
			return 0;

		return atol(str.c_str());
	}

	inline uint32_t stoul(const std::string& str)
	{
	   return (uint32_t)stol(str);
	}

	inline float stof(const std::string& str)
	{
		if(str.c_str()==NULL)
			return 0;

		return atof(str.c_str());
	}

	inline std::string to_string(const uint32_t value)
	{
		char buffer[100];
		memset(buffer,0,100);
		snprintf(buffer,100,"%lu",value);
		return std::string(buffer);
	}

	inline std::string to_string(const int32_t value)
	{
		char buffer[100];
		memset(buffer,0,100);
		snprintf(buffer,100,"%ld",value);
		return std::string(buffer);
	}

	inline std::string to_string(const float value)
	{
		char buffer[100];
		memset(buffer,0,100);
		snprintf(buffer,100,"%f",value);
		return std::string(buffer);
	}
}

#endif
