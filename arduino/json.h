#ifndef JSON_H
#define JSON_H

#include <Arduino.h>
#include "frozen.h"
#include "string.h"

class json_ro_t
{
	public:
		json_ro_t(const std::string& str):str_m(str),tokens_m(NULL)
		{
			if(str_m.c_str()!=NULL)
				tokens_m=parse_json2(str_m.c_str(),str_m.size());
		}

		json_ro_t(const json_ro_t& copy):str_m(copy.str_m),tokens_m(NULL)
		{
			*this=copy;
		}

		json_ro_t& operator=(const json_ro_t& copy)
		{
			if(this!=&copy)
			{
				str_m=copy.str_m;

				if(str_m.c_str()!=NULL)
					tokens_m=parse_json2(str_m.c_str(),str_m.size());
			}

			return *this;
		}

		~json_ro_t()
		{
			free(tokens_m);
			tokens_m=NULL;
		}

		inline std::string operator[](const std::string& key)
		{
			if(key.c_str()==NULL||tokens_m==NULL)
				return "";

			json_token* value=find_json_token(tokens_m,key.c_str());

			if(value==NULL)
				return "";

			return std::string(value->ptr,value->len);
		}

	private:
		std::string str_m;
		json_token* tokens_m;
};

#endif
