#include "string.h"
#include "json.h"
#include "frozen.c"

int main()
{
	json_ro_t test("{\"o\":[40,50,255],\"b\":[15]}}");

	while(true)
	{
		printf("%s\n",test["o"].c_str());
		printf("%s\n",test["o[0]"].c_str());
		printf("%s\n",test["o[1]"].c_str());
		printf("%s\n",test["o[2]"].c_str());
	}

	/*std::string t0("1234");
	std::string t1="abcd";

	printf("%s\n",t0.c_str());
	printf("%s\n",t1.c_str());

	t0.swap(t1);

	std::string t2(t1);

	printf("%s\n",t2.c_str());
	t2=t0;
	printf("%s\n",t2.c_str());

	t2.resize(2);
	printf("%s\n",t2.c_str());

	t2.resize(20);
	printf("%s\n",t2.c_str());

	t2.clear();
	printf("%s\n",t2.c_str());

	std::string t3=t0+t1;
	printf("%s\n",t3.c_str());

	t3+=t1;
	printf("%s\n",t3.c_str());

	t3+='c';
	printf("%s\n",t3.c_str());

	std::string t4=t3+'c';
	printf("%s\n",t4.c_str());

	t4=t3.substr(7,4);
	printf("%s\n",t4.c_str());*/

	return 0;
}