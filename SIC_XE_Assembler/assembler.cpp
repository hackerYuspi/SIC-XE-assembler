// SIC/XE Assembler with control section by Bindiya and Prajwalita
#include<iostream>
#include<stdio.h>
#include<map>
#include<fstream>
#include<sstream>
#include<vector>
#include<algorithm>
using namespace std;

//Map used to store the Format of instruction and Opcode.
map<string, int> format, opcode;

//Vector used to store the symtab.							
vector<map<string,int> > symtab;

//Vector Used to store the ProgramName, baseAddress, extdefs, extrefs.							
vector<string> programName, baseAddress, extdefs, extrefs;

//Map used to store the regcode.		
map<char, int> regcode;	

//Vector used to store the size of each control section.									
vector<int> sizeOfCsect;

									
struct  statement{											
    int address;
    string label;
    string instruction;
    string operand;
    int objectCode;
    int e;
};

//Vector used to store the all parameters of statement of instructions.
vector<vector<statement> >   Array;					

void initOpTab()
{											
    ifstream file;
    file.open("INS.txt");
    char ins[8];   
    int frmt;  
    int opc;    
    int c;
    string word;

    while(file >> ins)
    {

	stringstream str;
	file >> word;
	str << word;
	str >> hex >> frmt; 
	
	file >> word;
	str << word;
	str >> hex >> opc;

        format[ins] = frmt;
        opcode[ins]= opc;
  
    }
    file.close();

}

void setCsect(vector<string> &list,int count, int &i, int nCsect)
{	
    	Array[nCsect-1].push_back(*(new statement()));
	if(i!=0)
	{
        	if(Array[nCsect-1][i-1].instruction!="RESW" && Array[nCsect-1][i-1].instruction!="RESB" && Array[nCsect-1][i-1].instruction!="BYTE" && Array[nCsect-1][i-1].instruction!="WORD")
			if(Array[nCsect-1][i-1].e==1)
                		Array[nCsect-1][i].address = Array[nCsect-1][i-1].address + 4;
            		else
                		Array[nCsect-1][i].address = Array[nCsect-1][i-1].address + format[Array[nCsect-1][i-1].instruction];
        	else
		{
            	if(Array[nCsect-1][i-1].instruction=="RESW")
                	Array[nCsect-1][i].address = Array[nCsect-1][i-1].address + (3*stoi(Array[nCsect-1][i-1].operand) );
            	else if(Array[nCsect-1][i-1].instruction=="RESB")
                	Array[nCsect-1][i].address = Array[nCsect-1][i-1].address + (stoi(Array[nCsect-1][i-1].operand) );
            	else if(Array[nCsect-1][i-1].instruction=="WORD")
                	Array[nCsect-1][i].address = Array[nCsect-1][i-1].address + 3;
            	else if(Array[nCsect-1][i-1].instruction=="BYTE")
                	Array[nCsect-1][i].address = Array[nCsect-1][i-1].address + 1;
        	}
	}
	else
		Array[nCsect-1][i].address = 0;
    	if(count == 4)
	{
        	if(symtab[nCsect-1].find(list[0])==symtab[nCsect-1].end())
        		Array[nCsect-1][i].label = list[0];
        	else cout<<"Error : Duplicate Labels used\n";
        	if(list[1][0]=='+')
		{
            		Array[nCsect-1][i].instruction = list[1].substr(1);
            		Array[nCsect-1][i].e=1;
        	}
		else
		{
            		Array[nCsect-1][i].e=0;
            		Array[nCsect-1][i].instruction = list[1];
        	}
        	Array[nCsect-1][i].operand = list[2];
		symtab[nCsect-1][Array[nCsect-1][i].label]=Array[nCsect-1][i].address;
    	}
   	else if(count == 3)
	{
        	Array[nCsect-1][i].label = "\0";
        	if(list[0][0]=='+')
		{
            		Array[nCsect-1][i].instruction = list[0].substr(1);
            		Array[nCsect-1][i].e=1;
        	}
		else
		{
            		Array[nCsect-1][i].e=0;
            		Array[nCsect-1][i].instruction = list[0];
        	}
        	Array[nCsect-1][i].operand = list[1];
    	}
    	else if(count == 2)
	{
        	Array[nCsect-1][i].label = "\0";
        	Array[nCsect-1][i].instruction = list[0];
        	Array[nCsect-1][i].operand = "\0";
    	}
    	else
        	cout<<"Error invalid Instruction "<<list[0]<<endl;     
    	i++;
}

int firstPass(char* filename)
{	
	cout<<"\n****************Start**************";
	cout<<"\n***************Pass 1**************";								
    	ifstream inFile(filename);										
    	int i = 0,nCsect = 0;											
    	string str;
    	while(getline(inFile,str))
    	{	
		istringstream iss(str);
        	vector<string> list;
        	int count = 0;
        	do
		{
            		string sub;
            		iss>>sub;
            		list.push_back(sub);
            		count++;
        	}while(iss);
        	
		
		if(list[0][0]=='.')											
            		continue;
        	if(list[0] == "END")										
            		continue;
		if(list[1] == "CSECT")
            		sizeOfCsect.push_back(i);
		if(list[1]=="START"|| list[1] == "CSECT")
		{					
            		Array.push_back(*(new vector<statement>));
        		programName.push_back(list[0]);
			i=0; 
			nCsect++;
            		extdefs.push_back("");
            		extrefs.push_back("");
            		baseAddress.push_back("\0");
			continue;
		}
        	if(list[0]=="BASE")
		{										
            		baseAddress[nCsect-1] = list[0];
           	 	continue;       
        	}
		if(list[0]=="EXTDEF")
		{										
			extdefs[nCsect-1] = (list[1]);
			continue;		
        	}
		if(list[0]=="EXTREF")
		{										
			extrefs[nCsect-1] = (list[1]);
			continue;
		}
        	symtab.push_back(*(new map<string,int>()));
		setCsect(list,count,i, nCsect);		
    	}
    	sizeOfCsect.push_back(i);
	
	
    	return nCsect;
}

void secondpass(int nCsect)
{	
	cout<<"\n***************Pass 2**************";
									
    	for(int i=0;i<nCsect;i++)
	{
        	for(int j=0;j<Array[i].size();j++)
		{
            		int temp = 0;
            		if(Array[i][j].instruction=="WORD" || Array[i][j].instruction=="BYTE")
			{	
                		if(Array[i][j].operand[0]=='X')
				{											         
		   			string sub1 = Array[i][j].operand.substr(2,Array[i][j].operand.length() - 3);
                    			Array[i][j].objectCode = stoi(sub1,nullptr, 16);
                		}
                		else if(Array[i][j].operand[0]=='C')
				{										
                    			string sub1 = Array[i][j].operand.substr(2,Array[i][j].operand.length() - 3);
                    			for(int i=0;i<sub1.length();i++)
					{
                        			temp += (int)sub1[i];
                        			if(i!=sub1.length()-1)
                        		    		temp = temp<<8;
                    			}
                    			Array[i][j].objectCode = temp;
                		}
                		else
                    		Array[i][j].objectCode = stoi(Array[i][j].operand);
            		}
            		else if(Array[i][j].instruction!="RESW" && Array[i][j].instruction!="RESB")
			{	
                		if(format[Array[i][j].instruction] == 3 && Array[i][j].e==0){			
                    		temp = opcode[Array[i][j].instruction];
                    		if(Array[i][j].instruction=="RSUB")
				{
                        		temp += 3;
                        		temp = temp<<16;
                        		Array[i][j].objectCode = temp;
                        		continue;
                    		}
                    		if(Array[i][j].operand[0]=='#')
				{											
                        		temp += 1;
                        		temp  = temp<<16;
                        		string sub1 = Array[i][j].operand.substr(1,Array[i][j].operand.length() - 1);
                        		if(symtab[i].find(sub1) != symtab[i].end())
                            		temp += symtab[i][sub1];
                        	else
                            		temp += stoi(sub1);
                        	Array[i][j].objectCode = temp;
                        	continue;
                    		}
                    		else if(Array[i][j].operand[0]=='@')
				{											
                        		temp += 2;
                        		Array[i][j].operand = Array[i][j].operand.substr(1,Array[i][j].operand.length() - 1);
                    		}
                    		else
				{
                        		temp += 3;
                    		}               
                    		temp = temp<<4;
                    		int byte3 = 0;
                    		if(Array[i][j].operand[-2] == ',')
				{											
                        		byte3 = 8;
                        		Array[i][j].operand = Array[i][j].operand.substr(0,Array[i][j].operand.length()-2);
                    		}
                    		if(baseAddress[i]=="\0" && Array[i][j+1].address - symtab[i][Array[i][j].operand] <= 0xFFF )
				{
                        		byte3 += 2;															
                        		temp += byte3;														
                        		if(symtab[i][Array[i][j].operand] - Array[i][j+1].address < 0)
                            		temp++;
                        		temp = temp<<12;
                        		temp += symtab[i][Array[i][j].operand] - Array[i][j+1].address;
                    		}
                    		else if(baseAddress[i]!="\0")
				{												 
                        		byte3 += 4;
                        		temp += byte3;
                        		temp = temp<<12;
                        		temp += symtab[i][Array[i][j].operand] - symtab[i][baseAddress[i]];                   
                    		}
                    		else
                        		cout<<"Out of Bound! Use base relative or format 4 for"<<Array[i][j].address<<"\t"<<Array[i][j].label<<"\t"<<Array[i][j].instruction<<"\t"<<Array[i][j].operand<<" "<<endl;
                    		Array[i][j].objectCode = temp;
                	}
                	else if(format[Array[i][j].instruction] == 3 && Array[i][j].e==1)
			{			//incase of format 4
                    		temp = opcode[Array[i][j].instruction];
                    		if(Array[i][j].operand[0]=='#')
				{
                        		temp += 1;
                        		temp  = temp<<24;
                        		string sub1 = Array[i][j].operand.substr(1,Array[i][j].operand.length() - 1);
                        		if(symtab[i].find(sub1) != symtab[i].end())
                            			temp += symtab[i][sub1];
                        		else
                            			temp += stoi(sub1);
                        		Array[i][j].objectCode = temp;
                        		continue;
                    		}
                    		else if(Array[i][j].operand[0]=='@')
				{
                        		temp += 2;
                        		Array[i][j].operand = Array[i][j].operand.substr(1,Array[i][j].operand.length() - 1);
                    		}
                    		else	temp += 3;
                    		temp = temp<<4;
                    		if(Array[i][j].operand[Array[i][j].operand.length()-2] == ',')
				{
                        		temp += 9;
                        		Array[i][j].operand = Array[i][j].operand.substr(0,Array[i][j].operand.length()-2);
                    		}
                    		else	temp += 1;
                    		temp = temp<<20;
                    		temp += symtab[i][Array[i][j].operand];
                    		Array[i][j].objectCode = temp;
                	}	
                	else if(format[Array[i][j].instruction] == 2)
			{										
                    		temp = opcode[Array[i][j].instruction];
                    		temp = temp<<4;
                    		temp += regcode[Array[i][j].operand[0]];
                    		temp = temp<<4;
                    		temp += regcode[Array[i][j].operand[2]];
                    		Array[i][j].objectCode = temp;
                	}
                	else	Array[i][j].objectCode = opcode[Array[i][j].instruction];			
            	}
      		else	Array[i][j].objectCode = stoi(Array[i][j].operand);
        }
    }
}


string emptyBlocks(int a, int b = 6)
{										
    string ans = "";
    while(a++<b)
        ans += " ";
    return ans;
}

string intToString(int n, int x = 6){										
    stringstream stream;
    stream<<hex<<n;
    string result( stream.str() );
    for(int y=0;y<result.length();y++)	result[y] = toupper(result[y]);
    while(result.length()<x)	result = "0" + result;
    return result;
}



void printObjectProgram(int nCsect)
{										
    ofstream outfile;
    outfile.open("output.txt");
    for(int i=0;i<nCsect;i++){											
        int sectionSize = Array[i].back().address;
        if(Array[i].back().instruction!="RESW" && Array[i].back().instruction!="RESB" && Array[i].back().instruction!="BYTE" && Array[i].back().instruction!="WORD")
            if(Array[i].back().e==1)
                sectionSize += 4;
            else
                sectionSize += format[Array[i].back().instruction];
        else{
            if(Array[i].back().instruction=="RESW")
                sectionSize += (3*Array[i].back().objectCode);
            else if(Array[i].back().instruction=="RESB")
                sectionSize += (Array[i].back().objectCode);
            else if(Array[i].back().instruction=="WORD")
                sectionSize += 3;
            else if(Array[i].back().instruction=="BYTE")
                sectionSize += 1;
        }
        outfile<<"H"<<programName[i];										
        outfile<<emptyBlocks(programName[i].length())<<intToString(0);
        outfile<<hex<<intToString(sectionSize)<<dec<<"\n";
        stringstream test(extdefs[i]);
        string segment;
        vector<string> seglist;											
        while(std::getline(test, segment, ','))
            seglist.push_back(segment);
        if(seglist.size()!=0)
        	outfile<<"D";
        for(int k=0;k<seglist.size();k++)
            outfile<<seglist[k]<<emptyBlocks(seglist[k].length())<<intToString(symtab[i][seglist[k]]);
        if(seglist.size()!=0)
        	outfile<<"\n";
        stringstream test1(extrefs[i]);										
        vector<string> references;
        while(std::getline(test1, segment, ','))
            references.push_back(segment);
        if(references.size()!=0)   
        	outfile<<"R";
        for(int k=0;k<references.size();k++)
            outfile<<references[k]<<emptyBlocks(references[k].length());
        if(references.size()!=0)   outfile<<endl;
        map<int, string> modmap;
        int stat = 0;
        int limit = 0;
        for(;stat < sizeOfCsect[i];){										
            outfile<<"T"<<intToString(Array[i][stat].address);
            int dummy = stat,y;
           for(y= 0;y<30;){
                if(dummy>=sizeOfCsect[i])	break;
                if(find(references.begin(), references.end(), Array[i][dummy].operand) != references.end())
				    modmap[Array[i][dummy].address] = Array[i][dummy].operand;
				else if(Array[i][dummy].e==1)
					modmap[Array[i][dummy].address] = Array[i][dummy].operand;
                if(Array[i][dummy].instruction=="RESW" || Array[i][dummy].instruction=="RESB"){
                    while(Array[i][dummy].instruction=="RESW" || Array[i][dummy].instruction=="RESB")	dummy++;
                    break;
                }
                if(format[Array[i][dummy].instruction] == 3 && Array[i][dummy].e == 0){
                    dummy++;	y+=3;
                }
                else if(format[Array[i][dummy].instruction] == 3 && Array[i][dummy].e == 1){
                    dummy++;	y+=4;                  
                }
                else if(format[Array[i][dummy].instruction] == 2){
                    dummy++;	y+=2; 
                }
                else{
                    dummy++;	y+=1; 
                }
                if(dummy>=sizeOfCsect[i])	break;
                if(y+format[Array[i][dummy].instruction]>30 || (y+format[Array[i][dummy].instruction]>31 && Array[i][dummy].e==1))
                    break;
            }
            outfile<<intToString(y,2);
            for(int x= 0;x<30;){											
                if(stat>=sizeOfCsect[i])	break;
                if(Array[i][stat].instruction=="RESW" || Array[i][stat].instruction=="RESB"){
                    while(Array[i][stat].instruction=="RESW" || Array[i][stat].instruction=="RESB")	stat++;
                    break;
                }
                if(format[Array[i][stat].instruction] == 3 && Array[i][stat].e == 0){
                    outfile<<intToString(Array[i][stat].objectCode);
                    stat++;	x+=3;
                }
                else if(format[Array[i][stat].instruction] == 3 && Array[i][stat].e == 1){
                    outfile<<intToString(Array[i][stat].objectCode, 8);
                    stat++;	x+=4;                  
                }
                else if(format[Array[i][stat].instruction] == 2){
                    outfile<<intToString(Array[i][stat].objectCode, 4);
                    stat++;	x+=2; 
                }
                else{
                    outfile<<intToString(Array[i][stat].objectCode, 2);
                    stat++;	x+=1; 
                }
                if(stat>=sizeOfCsect[i])	break;
                if(x+format[Array[i][stat].instruction]>30 || (x+format[Array[i][stat].instruction]>31 && Array[i][stat].e==1))
                    break;
            }
            outfile<<endl;
        }
        map<int, string>::iterator it;										
        for(it = modmap.begin();it != modmap.end();it++)
        	outfile<<"M"<<intToString(it->first)<<intToString(5,2)<<"+"<<it->second<<endl;
        outfile<<"E"<<intToString(0,6)<<endl<<endl;							
    }
}

void printIntermediate(int nCsect)
{
    ofstream outfile;
    outfile.open("intermediate.txt");
	for(int i=0;i<nCsect;i++)
	{

		for(int j=0;j<Array[i].size();j++)
		{	
			outfile<<hex<<intToString(Array[i][j].address,4)<<emptyBlocks(0,4);
			outfile<<i<<emptyBlocks(0,5);
			outfile<<Array[i][j].label<<emptyBlocks(Array[i][j].label.length(),10);
			if((Array[i][j].address + 4) == Array[i][j+1].address)
			{
				outfile<<"+"<<Array[i][j].instruction<<emptyBlocks(Array[i][j].instruction.length(),10);
			}
			else
			{
				outfile<<Array[i][j].instruction<<emptyBlocks(Array[i][j].instruction.length(),10);
			}
			
			
			outfile<<Array[i][j].operand<<emptyBlocks(Array[i][j].operand.length(),10)<<dec<<"\n";
			
		}
	}
	outfile.close();
}

//Main program to start the Task;	
int main(int argc,char *argv[])
{
    	regcode['A'] = 0;	
	regcode['X'] = 1;	
	regcode['L'] = 2;	
	regcode['B'] = 3;	
	regcode['S'] = 4;	
	regcode['T'] = 5;	
	regcode['F'] = 6;

	initOpTab();
	
	int nCsect = firstPass(argv[1]);

	printIntermediate(nCsect);
	cout<<"\nIntermediate program is stored in 'intermediate.txt' file  in the same directory\n\n";
   	
	secondpass(nCsect);
											
													
	printObjectProgram(nCsect);	
	cout<<"\nObject program is stored in 'Output.txt' file  in the same directory\n\n";
										
    
	return 0;
}
