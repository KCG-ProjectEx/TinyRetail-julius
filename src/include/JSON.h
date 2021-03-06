#ifndef JSON_H__
#define JSON_H__

#include <string>

using namespace std;

class CJSON
{
public:
	CJSON();
	~CJSON();
	int push(const string& id, const string& value);
	int push(const string& id, const int& value);	
	string pop();
	int init();
	string data;
};

#endif