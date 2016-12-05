#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>
using namespace std;

//function prototypes
int readX(string FileName);
int readY(string FileName);
double Parse_Espresso_time(string inF);
string Int2String (int i);

//class declaration
class AElement;

class ALattice
{
private:
  string Name;                  // name of the lattice
  vector < vector <AElement> > Content; // vector of all elements (Column x Row)
  int dimension[2];             // Col Row dimensions
  int NumVar;                   // Number of literals
  int NumOut;                   // Number of outputs
 
  vector< vector<AElement> > equation;
  vector< vector<AElement> > equation_dual;
  AElement FindCommonLiteral(vector<AElement> term, vector<AElement> termD);

public:
  void setNumVar(int num);
  void setNumOut(int);
  int getNumVar();
  int getNumOut();
  void SetDimension(int c, int r);
  void SetName(string s);
  void read_synth_file(string InFileName, int NumOut,int NumIn, bool true_if_dual);
  void print_equations();
  void BuildLattice();
  void PrintLattice();
  void Print2File(string FileName);
};

class AElement
{

private:
  int lit;
  bool sign;  

public:
  int getLit() {return lit;}
  bool getSign() {return sign;}
  void setLit(int num) {lit=num;}
  void setSign(bool val) {sign=val;}
};

// function prototype declaration
void dual( string inFile, string outFile);

int main(int argc, char *argv[])
{
  fstream inputfile;
  string inFile(argv[1]);
  string outFile(argv[2]);
  double time=0;
  
  if (inFile.size()< 1 || outFile.size() < 1 )
    {
      cout << "Error!" << endl << "usage: ./AltunSinthesis [input.pla] [output.lattice]" << endl;
    }
   
  const char *inFileChar = inFile.c_str();
  // string outFile = "";
  char a;
  char b;
  string line;
  vector<ALattice> lattices;  

  int inputNum=0, outputNum=0;
  // ifstream inputfile(argv[1]);   //open input file
  // ostream outputfile(argv[2]);   //open input file

  char* Command;	 
  Command = (char*)malloc(sizeof(char)*512);

  // Synthesis of function
  sprintf(Command,"./espresso -Dexact -Dso -t %s > f.eq", inFile.c_str()); 
  system(Command);
  time = time + Parse_Espresso_time("f.eq");

  // compute the dual function
  dual(inFile, inFile+"_dual");

  // Synthesis of the dual function
  sprintf(Command,"./espresso -Dexact -Dso -epos -t %s > f_dual.eq", (inFile+"_dual").c_str());
  system(Command);
  time = time + Parse_Espresso_time("f_dual.eq");
  

  cout << "Espresso done"<<endl;
  
  inputfile.open(argv[1] , ios::in); // read dimension of .i e .o 
  if (inputfile.is_open())
    while( getline(inputfile , line))
      {
	if(line[0]=='.')
	  {
	    if (line[1]=='i')
	      inputNum=atoi(line.substr(2, string::npos).c_str()); // set number of input
	    else if (line[1]=='o')
	      outputNum=atoi(line.substr(2, string::npos).c_str()); // set number of output
	  }
      }
  else
    {
      cout << "There is some error with imput file"<<endl;
      return 1;
    }
  inputfile.close();

  
  // create the vector of lattices and write output number
  for(int i=0; i<outputNum;i++)
    {
      ALattice app;
      app.setNumOut(i); 
      app.read_synth_file("f.eq", i , inputNum , false);
      app.read_synth_file("f_dual.eq", i , inputNum , true);
      app.print_equations();
      app.BuildLattice();
      app.PrintLattice();

      // cout << "----->" <<Int2String(i)<<endl; 
      
      app.Print2File(outFile + Int2String(i));
      lattices.push_back(app);
      // cout << readX(outFile) << endl << readY(outFile) << endl <<time<< endl;

      fstream out;
      string OF=outFile;
      OF=OF+Int2String(i) + ".dat";
      out.open(OF.c_str() , ios::out);
      out  << readX(outFile + Int2String(i)) << endl << readY(outFile + Int2String(i)) << endl <<time<< endl;
      out.close();
    cout <<"AAA"<<endl;
    }

 

  // time 

  return 0;
}


// ***FUNCTIONS***


string Int2String (int i)
{

  stringstream ss;
  ss << i;
  return ss.str();
}

double Parse_Espresso_time(string inF)
{
  double time=0;
  fstream input;
  string line, app;
  input.open(inF.c_str(), ios::in);
  while (!input.eof())
    {
      getline(input,line); //read a line of input
      if (line[0]=='#' && line[2]=='s')
	{
	  //cout<<line<<endl;
	  size_t found = line.find("sec");
	  app = line.substr (14,found-14);
	  //cout << app <<"! " << endl;
	}
	    
	  
    }
  input.close();
  //cout<<strtod(app.c_str(), NULL)<<endl;
  return strtod(app.c_str(), NULL);
}




// int readY(string FileName)
// {
//   fstream f;
//   f.open(FileName.c_str(), ios::in);
//   int Y=0;
//  while(!f.eof())
//     {
//       Y++;
//       string line;
//       getline(f,line);
//     }
//  return Y-1;
// }
int readY(string FileName)
{
  fstream f;
  f.open(FileName.c_str(), ios::in);
  
  if(f.peek() == std::ifstream::traits_type::eof())
    {
      f.close();
      return 0;
    }
  
  
  int Y=0;
  cout << "s"<<FileName << endl;
  while(!f.eof())
    {
      Y++;
      string line;
      cout << line;
      getline(f,line);
    }
  f.close();
  return Y-1;
}
int readX(string FileName)
{
  fstream f;
  f.open(FileName.c_str(), ios::in);
  int X=0;

  if(f.peek() == std::ifstream::traits_type::eof())
    {
      f.close();
      return 0;
    }
  


  string line;
  getline(f,line);
  for( int i=0; i<line.length();i++)
    {
      if (line[i]=='|')
	X++;
    }
  f.close();
 return X+1;
}



void dual( string inFile, string outFile) // compute the input for the synthesis of the dual function
{
  string line;
  char a;
  int idx;
  fstream inputfile, outputfile;
  inputfile.open(inFile.c_str(), ios::in);
  outputfile.open(outFile.c_str(), ios::out);

  if (inputfile.is_open() && outputfile.is_open())
    {
      while (!inputfile.eof())
	{
	  getline(inputfile,line); //read a line of input
	  a=line[0];
    	  switch (a)
	    {
	    case '#': //don't write on output file
	      break;
	    case '.': //write without modify
	      outputfile << line << std::endl;
	      break;
	    default:  //write modifyinf input ( 1<->0 )
	      {
		idx=line.find(" ");
		if (idx > 0)
		  {		//substitution
		    replace( line.begin() , line.begin() + idx,'0' , 'k' ); 
		    replace( line.begin() , line.begin() + idx,'1' , '0' );
		    replace( line.begin() , line.begin() + idx,'k' , '1' ); 
		    outputfile << line << std::endl; //write line (result)
		  }
		break;
	      }
	    }
	}
      inputfile.close();
      outputfile.close();
    }
  else
    std::cout << "Something wrong!  :-(" << std::endl;
}


// ***Class ALattice variables***

int ALattice::getNumVar()
{
  return NumVar;
}

int ALattice::getNumOut()
{
  return NumOut;
}

void ALattice::setNumVar(int num)
{
  NumVar=num;
}

void ALattice::setNumOut(int num)
{
  NumOut=num;
}

void ALattice::SetDimension(int c, int r)
{
  dimension[0]=c;
  dimension[1]=r;
}

void ALattice::SetName(string s)
{
  Name=s;
}

void ALattice::print_equations()
{
  cout << "f"<<getNumOut()<<"= ";
  for (int i=0; i<equation.size();i++)
    {
      if (i!=0)
	cout << " +";
      for(int j=0; j<equation[i].size(); j++)
	{
	  if (equation[i][j].getSign())
	    cout <<" x"<< equation[i][j].getLit();
	  if (!equation[i][j].getSign())
	    cout <<" !x"<< equation[i][j].getLit();
	}
      
    }
  cout<<endl<<"fd"<<getNumOut()<<"= ";
  
  for (int i=0; i<equation_dual.size();i++)
    {
      if (i!=0)
	cout << " +";
      for(int j=0; j<equation_dual[i].size(); j++)
	{
	  if (equation_dual[i][j].getSign())
	    cout <<" x"<< equation_dual[i][j].getLit();
	  if (!equation_dual[i][j].getSign())
	    cout <<" !x"<< equation_dual[i][j].getLit();
	}
    }
  cout << endl;
}

void ALattice::read_synth_file(string InFileName, int NumOut, int NumIn, bool true_if_dual)
{
  fstream inputfile, outputfile;
  
  //outputfile.open(OutFileName.c_str(), ios::out);
  char firstChar;
  string line;
  int outputNum=0, inputNum=0;
  AElement appLit; 		// AElement used to write inside vector
  vector< AElement > minterm;

  inputfile.open(InFileName.c_str(), ios::in);
  if(inputfile.is_open())
    {	      	        
      while( getline(inputfile , line))
	{
	  minterm.clear();
	  if(line[0]=='0' || line[0]=='1' || line[0]=='-')
	    {
	      if (line[NumIn +1+ NumOut]=='1') // if the output is '1'
		{
		  for(int in=0 ; in<NumIn ; in++)
		    {
		      
		      if (line[in]=='1')
			{
			  appLit.setLit(in);
			  appLit.setSign(true);
			  minterm.push_back(appLit);
 			  // cout << "appLit " << appLit.getSign() <<"__"<< appLit.getLit() << "  ";
			}
		      if (line[in]=='0')
			{
			  appLit.setLit(in);
			  appLit.setSign(false);
			  minterm.push_back(appLit);
			  // cout << "appLit " << appLit.getSign() <<"__"<< appLit.getLit() << "  ";
			}
		    }
		  if(true_if_dual)
		    {
		      equation_dual.push_back(minterm);
		      for(int t=0; t < minterm.size(); t++) 
  		       	cout <<"D"<< minterm[t].getLit();
		      cout << endl;
		    }
		  else
		    {
		      equation.push_back(minterm);
		      // for(int t=0; t < minterm.size(); t++)
		      // 	cout << minterm[t].getLit();
		      // cout << endl;
		    }
		}
	    }
	}
      inputfile.close();
      // cout <<"finish"<<endl;   
    }
  else cout << "something wrong"<<endl; 
}



void ALattice::BuildLattice()
{
  
  vector<AElement> appRow;
  for (int Di=0; Di<equation_dual.size();Di++)  // for each row
    {
      appRow.clear();
      for  (int i=0; i<equation.size();i++)
	{
	  appRow.push_back(FindCommonLiteral(equation[i], equation_dual[Di]));
	}
      //  cout <<"write new row"<<endl;
      Content.push_back(appRow);
    }
  
}

AElement ALattice::FindCommonLiteral(vector<AElement> term, vector<AElement> termD)
{
  for(int i=0; i<term.size(); i++)
    for(int j=0; j<termD.size(); j++)
      {
	//	cout << "termD"<<termD[j].getSign()<<" "<<termD[j].getLit()<<" ---"<<"term"<<term[i].getSign()<<" "<<term[i].getLit()<<endl;
	if (term[i].getLit()==termD[j].getLit() && term[i].getSign()==termD[j].getSign())
	  {
	    //cout << endl;
	    return term[i];
	   
	  }
	
      }
    
}

void ALattice::PrintLattice()
{
  for(int j=0; j<Content.size();j++) //for each row
    {
      for(int i=0; i<Content[j].size();i++) //for each column
	{
	 
	  if (i!=0) cout << " | ";
	  if(!Content[j][i].getSign())
	    cout << "-";
	  else
	    cout << " ";
	  cout << Content[j][i].getLit();
	}
      cout<<endl;
    }
  cout<<endl<<endl;
}

void ALattice::Print2File(string FileName)
{
  fstream outputfile;
  outputfile.open(FileName.c_str() , ios::out); // read dimension of .i e .o 
  for(int j=0; j<Content.size();j++) //for each row
    {
      for(int i=0; i<Content[j].size();i++) //for each column
	{
	  if (i!=0) outputfile << " | ";
	  if(!Content[j][i].getSign())
	    outputfile << "-";
	  else
	    outputfile << " ";
	  outputfile << Content[j][i].getLit();
	}
      outputfile<<endl;
    }
}
