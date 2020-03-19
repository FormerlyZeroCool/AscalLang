
#include <iostream>
#include <string>

template <class t>
class setting {
private:
	t dat;
	//Currently unused
	std::string name;
	//Currently unused
	std::string message;
	//Used as key in settings hashtable
	std::string command;

public:
	setting(){}

	setting(std::string name,std::string command,t dat):
		dat(dat),name(name),command(command){};

	setting(std::string name,std::string command,std::string message,t dat):
		dat(dat),name(name),command(command),message(message){};

	bool operator==(const t &dat)
	{
		return this->dat == dat;
	}
	bool operator==(const setting<t> &o)
	{
		return this->dat == o.dat && o.command.compare(this->command) == 0 && o.name.compare(this->name) == 0
				&& o.message.compare(this->message) == 0;
	}
	bool operator=(t odat)
	{
		return this->dat = odat;
	}
	t& operator*()
	{
		return dat;
	}
	t getSetting()
	{
		return this->dat;
	}
	std::string getName()
	{
		return name;
	}
	std::string getMessage()
	{
		return message;
	}
	std::string getCommand()
	{
		return command;
	}
	void setMessage(std::string m)
	{
		this->message = m;
	}
	void setName(std::string  name)
	{
		this->name = name;
	}
	void setCommand(std::string com)
	{
		this->command = com;
	}
};
//class cmdSetting
