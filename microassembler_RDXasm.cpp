#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdio.h>

using namespace std;

string codefilename;
string exefilename;
string readfilename;

ifstream codefile;
ofstream exefile;
ofstream readfile;

struct key
{
	string keyword;
	unsigned short int keycode;
};

typedef struct key key;

int getNumber(string c)
{
    long long int result=0;
    unsigned int i=0; // iterator
    unsigned int nums=0;
    while(c[i]==' ')
    {i++;}
    if(c[i]=='0')     // the number is mentioned as binary or hexadecimal
    {
        i++;
        if(c[i]=='x' || c[i]=='X')
        {
            i++;
            while( i<c.length() )
            {
                switch (c[i])
                {
                    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                        {
                            result<<=4;
                            result|=(c[i++]-'0');
                            break;
                        }
                    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                        {
                            result<<=4;
                            result|=(c[i++]-55);
                            break;
                        }
                    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                        {
                            result<<=4;
                            result|=(c[i++]-87);
                            break;
                        }
                    default:
                        {
                        	if(nums==0)
							{
								return -1;
							}
                            return result;
                        }
                }
                nums++;
            }
        }
        else if(c[i]=='b' || c[i]=='B')
        {
            i++;
            while( i<c.length() && (c[i]=='0' || c[i]=='1') )
            {
                result=(result<<1)|(c[i++]-'0');
                nums++;
            }
        }
        else
        {
            goto L;
        }
    }
    else              // the number is decimal
    {
        L :
        if(c[i]=='d' || c[i]=='D')
        {
            i++;
        }
        while(i<c.length() && c[i]>='0' && c[i]<='9')
        {
            result=result*10+c[i++]-'0';
            nums++;
        }
    }
    if(nums==0)
	{
		return -1;
	}
    return result;
}


int main(int argc,char** argv)
{
	if(argc!=3)
	{
		cout<<"error : invalid param"<<endl;
		return 0;
	}

	codefilename = argv[1];

	exefilename = argv[2];
	exefilename+=".RDXuasm";

	cout<<codefilename<<" will be assembled to "<<exefilename<<endl;

	vector<key> keys;

	codefile.open(codefilename);
	exefile.open(exefilename);

    if(!codefile.is_open())
	{
		cout<<"error : code file unable to open"<<endl;
		return 0;
	}

    char temp = 'a';
    while( (!codefile.eof()) && (temp!='{') )
	{
		codefile.get(temp);
	}
	while( (!codefile.eof()) && (temp!='\n') )
	{
		codefile.get(temp);
	}


    string data;
    key tempo;
    int number;
    while(!codefile.eof())
	{
		data = "";
        do
		{
			codefile.get(temp);
			if(temp=='=' || temp=='\n' || codefile.eof() )
			{
				break;
			}
			else
			{
                data += temp;
			}
		}while(1);

		for(unsigned int j=0;j<data.length();)
		{
			if(data[j]==' ' || data[j]=='\n')
			{
                data.erase(j);
			}
			else
			{
				j++;
			}
		}
		if(data.compare("}")==0)
		{
            break;
		}
		tempo.keyword = data;
		data = "";
		do
		{
			codefile.get(temp);
			if(temp=='\n')
			{
				break;
			}
			else
			{
                data += temp;
			}
		}while(1);
		number = getNumber(data);
		if(number == -1)
		{
			cout<<"error : in specifying number constant"<<endl;
			return 0;
		}
		tempo.keycode = number;
		keys.push_back(tempo);
		if(temp=='}')
		{
			break;
		}
	}

	/*while( (!codefile.eof()) && (temp!='\n') )
	{
		codefile.get(temp);
	}*/

	char line[200];
	unsigned int address=0;

	while(!codefile.eof())
	{
        codefile.getline(line,200);
        for(int i=0,j=1;i<200 && j<200 && line[i]!='\0';i++,j++)
		{
			if(line[i]==' ')
			{
                line[i]=line[j++];
			}
		}

		if(line[0]=='\0' || line[0]=='/')
		{
			continue;
		}

        for(int i=0;i<200;i++)
		{
            if( line[i]=='\0')
			{
				address++;
				break;
			}
			else if(line[i]==':')
			{
                tempo.keyword="";
                for(int j=0;j<i;j++)
				{
                    tempo.keyword+=line[j];
				}
				tempo.keycode=address;
				keys.push_back(tempo);
				address++;
				break;
			}
		}
	}

	for(int i=0;i<keys.size();i++)
	{
        cout<<keys[i].keyword<<" - "<<keys[i].keycode<<endl;
	}

	codefile.clear();
	codefile.ignore();

	codefile.seekg(0,codefile.beg);

	while(!codefile.eof() && temp!='}')
	{
		codefile.get(temp);
	}

    string word[3];
    unsigned short int command;

	while(!codefile.eof())
	{
        unsigned int i = 0;
        command = 0;
        word[0]="";
        word[1]="";
        word[2]="";
        bool conti = true;

        do
		{
			codefile.get(temp);
			if(temp!=' ' && temp!='\n' && temp!='/')
			{
				word[i]+=temp;
				conti = false;
			}
			else if(conti==false && (temp==' ' || temp=='\n' || temp=='/'))
			{
				i++;
				conti = true;
			}
			if(temp==':')
			{
                conti=true;
                word[0]="";
                i=0;
                if(i>0)
				{
                    cout<<"error : in giving label"<<endl;
                    cout<<"error : label should be first in its line"<<endl;
				}
			}

		}while(temp!='\n' && temp!='/' && (!codefile.eof()));

		cout<<word[0]<<" ! "<<word[1]<<" ! "<<word[2]<<" "<<i<<endl;

		while(temp!='\n' && (!codefile.eof()))
		{
            codefile.get(temp);
		}


		if(i==0)
		{
			continue;
		}

        if(i==3)
		{
			bool found = false;
            unsigned int i = 0;
            for(;i<keys.size();i++)
			{
                if(keys[i].keyword.compare(word[0])==0)
                {
                	found = true;
                	break;
                }
			}
			if(found == true)
			{
				command = keys[i].keycode;
				command<<=8;
			}
			else
			{
				number = getNumber(word[0]);
				if(number == -1)
				{
					cout<<"error : in specifying number"<<endl;
					break;
				}
				command = number;
                command = (command<<8) | ( (command>>2) & 0x00C0);
			}

			found = false;
            i = 0;
            for(;i<keys.size();i++)
			{
                if(keys[i].keyword.compare(word[2])==0)
                {
                	found = true;
                	break;
                }
			}
			if(found == true)
			{
				command |= keys[i].keycode;
			}
			else
			{
				cout<<"error : no such decoder signal keyword connection specified"<<endl;
				break;
			}

			if(word[1].compare("i")==0)
			{
				command = command | (1<<5);
			}
		}
		else if(i==2)
		{
            bool found = false;
            unsigned int i = 0;
            for(;i<keys.size();i++)
			{
                if(keys[i].keyword.compare(word[0])==0)
                {
                	found = true;
                	break;
                }
			}
			if(found == true)
			{
				command = keys[i].keycode;
				command<<=8;
			}
			else
			{
				cout<<"error : no such decoder signal specified == "<<word[0]<<endl;
				break;
			}

			found = false;
            i = 0;
            for(;i<keys.size();i++)
			{
                if(keys[i].keyword.compare(word[1])==0)
                {
                	found = true;
                	break;
                }
			}
			if(found == true)
			{
				command |= keys[i].keycode;
			}
			else
			{
				cout<<"error : no such decoder signal specified == "<<word[1]<<endl;
				break;
			}

		}
		else
		{
			cout<<"error : incorrect code "<<endl;
			break;
		}

		exefile<<hex<<(command>>12)<<((command>>8)&0x0f)<<" "<<((command>>4)&0x0f)<<(command&0x0f)<<"\n";
		cout<<hex<<(command>>12)<<((command>>8)&0x0f)<<" "<<((command>>4)&0x0f)<<(command&0x0f)<<dec<<"\n";

	}

    return 0;
}
