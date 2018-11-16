
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

//class declaration

//how it works
// ./AltunSinthesis z4.pla z4.plalattice -m
// -m for multi -a for first match



class AElement;

class ALattice
{
private:
  string Name;                  // name of the lattice
  vector< vector<AElement> > Content; // vector osf all elements (Column x Row)
  vector< vector< vector <AElement> > > ContentMulti; // vector osf all elements (Column x Row)
  vector<string> OptVecVAR; // vecotr of all the variables for optimization
  int dimension[2];             // Col Row dimensions
  int NumVar;                   // Number of literals
  int NumOut;                   // Number of outputs
 
  vector< vector<AElement> > equation;
  vector< vector<AElement> > equation_dual;
  AElement FindCommonLiteral(vector<AElement> term, vector<AElement> termD);
  vector< AElement> FindCommonLiteral_multi(vector<AElement> term, vector<AElement> termD);
  
public:
  //int OptFindVar();
  void setNumVar(int num);
  void setNumOut(int);
  int getNumVar();
  int getNumOut();
  void SetDimension(int c, int r);
  void SetName(string s);
  void read_synth_file(string InFileName, int NumOut,int NumIn, bool true_if_dual);
  void print_equations();
  void BuildLattice();
  void BuildLattice_multi();
  void PrintLattice();
  void PrintLattice_multi();
  void Print2File(string FileName);
  void Print2File_multi(string FileName);
  int GetRowNum();
  int GetColNum();
  void OptFindVar();
  void print_data(string FileName);
  bool FindOptPos(string lit,int r, int c);
  void Recompose_optimized(string FileName);
  void optimized_vec_col(string FileName, string FileName2);
  void optimized_vec_row(string FileName, string FileName2);
  void OptCost(string Filename);

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
  cout << "usage -m is for multi variables -u for unary -OptCol to optimize columns --only with -u (needs GLPK)" << endl;
  (void)argc;
  fstream inputfile;
  string inFile(argv[1]);
  string MultiUnary(argv[3]);
  bool OPTcol=false;
  bool OPTrow=false; 
  cout << argc;
  if (argc > 4) {
    string OptStr(argv[4]);
    if (OptStr=="-OptCol") OPTcol=true;
    if (OptStr=="-OptRow") OPTrow=true; // bisogna fare un ponte per farle tutte e due o un disable della sintesi per fare solo ordinamento
  }
  string line;



    
  vector<ALattice> lattices;  

  unsigned int inputNum=0, outputNum=0;

  char* Command;	 
  Command = (char*)malloc(sizeof(char)*512);

  // Synthesis of function
  sprintf(Command,"./espresso -Dexact -Dso %s > %sf.eq", inFile.c_str(),inFile.c_str()); 
  system(Command);
  cout << Command << endl;
  // compute the dual function
  // OLD  dual(inFile, inFile+"_dual");
  string Feq = inFile+"f.eq";
  dual(Feq, inFile+"_dual");

  // Synthesis of the dual function
  sprintf(Command,"./espresso -Dexact -Dso -epos %s > %sf_dual.eq", (inFile+"_dual").c_str(),inFile.c_str());
  system(Command);
  cout << Command << endl<<endl;

  cout << "Espresso done"<<endl<<endl;
  
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
  for(unsigned int i=0; i<outputNum; i++)
    {
      ALattice app;
      app.setNumOut(i); 
      app.read_synth_file(inFile+"f.eq", i , inputNum , false);
      app.read_synth_file(inFile+"f_dual.eq", i , inputNum , true);
      app.print_equations();
      
      if (MultiUnary == "-m")
        {
          cout << "MULTI" << endl;
          app.BuildLattice_multi();
          app.PrintLattice_multi();
          ostringstream i_str;	// stream used for the conversion
          i_str << i;
          
          app.Print2File_multi(inFile+".lattice"+i_str.str());
          lattices.push_back(app);
        }
      else
        {
          cout << "SINGLETON" << endl<< endl;
          app.BuildLattice();
          app.PrintLattice();
          ostringstream i_str;	// stream used for the conversion
          i_str << i;
          
          app.Print2File(inFile+".lattice"+i_str.str());
          lattices.push_back(app);
        }
      
    

      if (OPTcol==true)
        {
          cout << "########## COLUMN  OPTIMIZATION!  ############"<< endl;
          //          int col=app.GetColNum(),row=app.GetRowNum();
          app.OptFindVar();
          string Datafile = inFile+".data"+to_string(i);
          string LogFile = inFile+".ResultColGLPK"+to_string(i);
          string GLPKoutput = inFile+".OutColGLPK"+to_string(i);
          app.print_data(Datafile);
          char* OptCommand;
          OptCommand = (char*)malloc(sizeof(char)*512);
          sprintf(OptCommand,"glpsol  --memlim 12000 --model ColOpt.mod  --data %s --output %s > %s", Datafile.c_str(),GLPKoutput.c_str(),LogFile.c_str());
          cout << OptCommand<< endl;
          system(OptCommand);

          sprintf(OptCommand,"cat %s | grep y | grep val | grep '\\= 1'>app%s",LogFile.c_str(),inFile.c_str()); // removes the first and last column that werw added to fix the linear problem mapping
          cout << OptCommand<< endl;
          system(OptCommand);
          app.optimized_vec_col("app"+inFile, inFile+".lattice"+to_string(i)+"optimizedRow");

          cout <<"EEE"<< GLPKoutput.c_str() ;
          app.OptCost(LogFile.c_str());

          // ---plot results------------------------
          string ResOptCol="ResOptCol";
          char* NAMEappCol;
         NAMEappCol = (char*)malloc(sizeof(char)*512);
          fstream f3, fo;
          fo.open(LogFile.c_str(), ios::in);
          f3.open(ResOptCol.c_str(), ios::app|ios::out);
          f3 << inFile <<i<< " "   << app.GetRowNum()<< " " << app.GetColNum() << " ";

          while( getline(fo , line))
            {

              if (line[0]=='T' && line[1]=='i' && line[2]=='m')
                {
                  
                f3 << line.substr(line.find(':')+1,line.find("sec")-1-line.find(':'))<<endl;
                }
            }     
          // ---------------------------------------

          
          cout << "##########  ############  ############"<< endl;
        }


       if (OPTrow==true)
        {
          cout << "########## ROW  OPTIMIZATION!  ############"<< endl;
          //          int col=app.GetColNum(),row=app.GetRowNum();
          app.OptFindVar();
          string Datafile = inFile+".data"+to_string(i);
          string LogFile = inFile+".ResultRowGLPK"+to_string(i);
          string GLPKoutput = inFile+".OutRowGLPK"+to_string(i);
          app.print_data(Datafile);
          char* OptCommand;
          OptCommand = (char*)malloc(sizeof(char)*512);
          sprintf(OptCommand,"glpsol  --memlim 12000 --model RowOpt.mod  --data %s --output %s > %s", Datafile.c_str(),GLPKoutput.c_str(),LogFile.c_str());
          cout << OptCommand<< endl;
          system(OptCommand);

          sprintf(OptCommand,"cat %s | grep y | grep val | grep '\\= 1'>app%s",LogFile.c_str(),inFile.c_str()); // removes the first and last column that werw added to fix the linear problem mapping
          cout << OptCommand<< endl;
          system(OptCommand);
          app.optimized_vec_row("app"+inFile, inFile+".lattice"+to_string(i)+"optimizedCol");

          cout <<"EEE"<< GLPKoutput.c_str() ;
          app.OptCost(LogFile.c_str());

         
          cout << "##########  ############  ############"<< endl;
        }
      
    }
  return 0;
}


// ***FUNCTIONS***
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
	      outputfile << line << endl;
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

// void ALattice::Recompose_optimized(string FileName)
// {

// }

void ALattice::optimized_vec_row(string FileName, string FileName2)
{
  fstream f;
  string line;
  vector<int> orderOpt;
  f.open(FileName.c_str(), ios::in);
  while( getline(f , line))
    {
      orderOpt.push_back(0);
    }
  f.close();
  

  
  f.open(FileName.c_str(), ios::in);
  while( getline(f , line))
    {
      int i=0;
      int arrive= stoi(line.substr(line.find(',')+1,line.find(']')- line.find(',')-1));
      int start = stoi(line.substr(2, line.find(',')-2).c_str());
      cout << "AS "<<start<<","<< arrive<<endl;
      //    orderOpt[start-1]=arrive-1;
      orderOpt[start-1]=arrive-1;
      i++;
    }

  for(unsigned int i=0; i<orderOpt.size();i++) //for each column
    {
      cout << "E"<< orderOpt[i]<<endl;

    }
  cout << "## print optimized lattice ##" << endl;
  for(unsigned int j=0; j<Content.size();j++) //for each row
    {
      for(unsigned int i=0; i<Content[j].size();i++) //for each column
        {
	 
          if (i!=0) cout << " | ";
          if(!Content[orderOpt[j]][i].getSign())
            cout << "-";
          else
            cout << " ";
          cout << Content[orderOpt[j]][i].getLit();
        }
      cout<<endl;
    }
  cout<<endl<<endl;
  

  //string FileName2=FileName + "optimized";
  cout<< FileName2;
  fstream f2;
  f2.open(FileName2.c_str(), ios::out);

  for(unsigned int j=0; j<Content.size();j++) //for each row
    {
      for(unsigned int i=0; i<Content[j].size();i++) //for each column
        {
	 
          if (i!=0) f2 << " | ";
          if(!Content[orderOpt[j]][i].getSign())
            f2 << "-";
          else
            f2 << " ";
          f2 << Content[orderOpt[j]][i].getLit();
        }
      f2<<endl;
    }
  



  f2.close();
  cout<<"##"<<endl<<endl;
}


void ALattice::optimized_vec_col(string FileName, string FileName2)
{
  fstream f;
  string line;
  vector<int> orderOpt;
  f.open(FileName.c_str(), ios::in);
  while( getline(f , line))
    {
      orderOpt.push_back(0);
    }
  f.close();
  

  
  f.open(FileName.c_str(), ios::in);
  while( getline(f , line))
    {
      int i=0;
      int arrive= stoi(line.substr(line.find(',')+1,line.find(']')- line.find(',')-1));
      int start = stoi(line.substr(2, line.find(',')-2).c_str());
      cout << "AS "<<start<<","<< arrive<<endl;
      //    orderOpt[start-1]=arrive-1;
      orderOpt[start-1]=arrive-1;
      i++;
    }

  for(unsigned int i=0; i<orderOpt.size();i++) //for each column
    {
      cout << "E"<< orderOpt[i]<<endl;

    }
  cout << "## print optimized lattice ##" << endl;
  for(unsigned int j=0; j<Content.size();j++) //for each row
    {
      for(unsigned int i=0; i<Content[j].size();i++) //for each column
        {
	 
          if (i!=0) cout << " | ";
          if(!Content[j][orderOpt[i]].getSign())
            cout << "-";
          else
            cout << " ";
          cout << Content[j][orderOpt[i]].getLit();
        }
      cout<<endl;
    }
  cout<<endl<<endl;
  

  //string FileName2=FileName + "optimized";
  cout<< FileName2;
  fstream f2;
  f2.open(FileName2.c_str(), ios::out);

  for(unsigned int j=0; j<Content.size();j++) //for each row
    {
      for(unsigned int i=0; i<Content[j].size();i++) //for each column
        {
	 
          if (i!=0) f2 << " | ";
          if(!Content[j][orderOpt[i]].getSign())
            f2 << "-";
          else
            f2 << " ";
          f2 << Content[j][orderOpt[i]].getLit();
        }
      f2<<endl;
    }

  f2.close();
  cout<<"##"<<endl<<endl;
}



// void ALattice::optimized_vec_(string FileName, string FileName2)
// {
//   fstream f;
//   string line;
//   vector<int> orderOpt;
//   f.open(FileName.c_str(), ios::in);
//   while( getline(f , line))
//     {
//       orderOpt.push_back(0);
//     }
//   f.close();
      
//   f.open(FileName.c_str(), ios::in);
//   while( getline(f , line))
//     {
//       int i=0;
//       int arrive= stoi(line.substr(line.find(',')+1,line.find(']')- line.find(',')-1));
//       int start= stoi(line.substr(2, line.find(',')-2).c_str());
//       cout << arrive<<","<< start<<endl;
//       orderOpt[arrive-1]=start-1;
//       i++;
//     }


//   cout << "## print optimized lattice ##" << endl;
//   for(unsigned int j=0; j<Content.size();j++) //for each row
//     {
//       for(unsigned int i=0; i<Content[j].size();i++) //for each column
//         {
	 
//           if (i!=0) cout << " | ";
//           if(!Content[j][orderOpt[i]].getSign())
//             cout << "-";
//           else
//             cout << " ";
//           cout << Content[j][orderOpt[i]].getLit();
//         }
//       cout<<endl;
//     }
//   cout<<endl<<endl;
  

//   //string FileName2=FileName + "optimized";
//   cout<< FileName2;
//   fstream f2;
//   f2.open(FileName2.c_str(), ios::out);

//   for(unsigned int j=0; j<Content.size();j++) //for each row
//     {
//       for(unsigned int i=0; i<Content[j].size();i++) //for each column
//         {
	 
//           if (i!=0) f2 << " | ";
//           if(!Content[j][orderOpt[i]].getSign())
//             f2 << "-";
//           else
//             f2 << " ";
//           f2 << Content[j][orderOpt[i]].getLit();
//         }
//       f2<<endl;
//     }

//   f2.close();
//   cout<<"##"<<endl<<endl;

  

// }



// void ALattice::print_data(string FileName)
// {
//   fstream f;
//    f.open(FileName.c_str(), ios::out);
//    bool debug=false;
//     if (!debug)
//       {
//         f << "param m:=" << GetColNum()<< ";" <<endl;
//         f << "param n:=" << GetRowNum()<< ";" <<endl;
//         f << "param q:=" << OptVecVAR.size()<< ";" <<endl <<endl;
//   f << "param a:=" <<  endl;
//   //param m := 5;
//   //param q := 7;
//   for(unsigned int i=0; i<OptVecVAR.size(); i++)
//     {
//       f << "#"<<OptVecVAR[i] << endl << "[*,*,"<<i+1<< "]:         ";
//       for(unsigned int j=0; j<(unsigned int)GetColNum(); j++)
//         {
//           f << j+1 << " ";
//         }
//       f << " := " << endl;
//       for(unsigned int j=0; j<(unsigned int)GetColNum(); j++)
//         {
//           f << endl << "              "<<j+1<<"  ";
//           for(unsigned int k=0; k<(unsigned int)GetRowNum(); k++)
//             f << FindOptPos(OptVecVAR[i],j,k) <<" ";
//         };
//         f << endl<< endl;
//     }

//   f << ";" << endl <<"end;" << endl;
//   f.close();
//       }
//     else
//       {
//    cout << ";" << endl <<"end;" << endl;
//       }
// }


void ALattice::print_data(string FileName)
{
  fstream f;
  f.open(FileName.c_str(), ios::out);
  bool debug=false;
  if (!debug)
    {
      f << "param m:=" << GetColNum() << ";" <<endl;
      f << "param n:=" << GetRowNum()<< ";" <<endl;
      f << "param q:=" << OptVecVAR.size()<< ";" <<endl <<endl;
      f << "param a:=" <<  endl;
      //param m := 5;
      //param q := 7;
      for(unsigned int i=0; i<OptVecVAR.size(); i++)
        {
          f << "#"<<OptVecVAR[i] << endl << "[*,*,"<<i+1<< "]:         ";
          for(unsigned int j=0; j<(unsigned int)GetColNum(); j++)
            {
              f << j+1 << " ";
            }
          f << " := " << endl;

          for(unsigned int j=0; j<(unsigned int)GetRowNum(); j++)
            {
              f << endl << "              "<<j+1<<"  ";
    
              for(unsigned int k=0; k<(unsigned int)GetColNum(); k++)
                {
                  // if (k==0 || k==(unsigned int)GetColNum())
                  //   f << "0 ";
                  // else
                  //   {
                  f << FindOptPos(OptVecVAR[i],j,k) <<" ";
                  // }
                }
            };
          f << endl<< endl;
        }

      f << ";" << endl <<"end;" << endl;
      f.close();
    }
  else
    {
      cout << ";" << endl <<"end;" << endl;
    }
}

bool ALattice::FindOptPos(string lit,int r, int c)
{
  string varIN;
  // cout << "A" << varIN << endl;
  //  cout << "B" << varIN <<endl;
  // !Content[r][c].getSign()
  if(!Content[r][c].getSign())
    {
      
      varIN="-" + std::to_string(Content[r][c].getLit());
    }




  else
    {
      varIN=std::to_string(Content[r][c].getLit());
    }

  if (varIN==lit)
    return 1;
  else
    return 0;
      
}

void ALattice::OptCost(string Filename)
{
  string line,ResOpt;
  ResOpt="ResOpt";
  fstream f3, fo;
  fo.open(Filename.c_str(), ios::in);
  f3.open(ResOpt, ios::app|ios::out);
  f3 << Filename << " "<< GetColNum() << " " << GetRowNum()<< " " << OptVecVAR.size();
  while( getline(fo , line))
    {
      if (line[0]=='T' && line[1]=='i' && line[2]=='m')
        f3 << line.substr(line.find(':')+1,line.find("sec")-1-line.find(':'));
        
      if (line[0]=='M' && line[1]=='e' && line[2]=='m')
        f3 << line.substr(line.find('(')+1,line.find("by")-1-line.find('('));
          
    }
  f3 << endl;
  fo.close();
  f3.close();
}

void ALattice::OptFindVar()
{
  //  vector<string> OptVecVar;
  for(unsigned int j=0; j<Content.size();j++) //for each row
    {
      for(unsigned int i=0; i<Content[j].size();i++) //for each column
        {
          string appStr;
          if(!Content[j][i].getSign())
            appStr="-" + std::to_string(Content[j][i].getLit());
          else
            appStr= std::to_string(Content[j][i].getLit());


          
          std::vector<string>::iterator pos;
          pos = find (OptVecVAR.begin(), OptVecVAR.end(), appStr);

          if (pos == OptVecVAR.end())
            {
              OptVecVAR.push_back(appStr);
              //   cout << appStr<<"q";
              //cout << OptVecVar[OptVecVar.size()-1]<<endl;
            }
          //          else
          //  std::cout << "Element not found in myvector\n";
          
        }
    

    }
  // return OptVecVar;
}


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

int ALattice::GetColNum()
{
  return dimension[0];
}

int ALattice::GetRowNum()
{
  return dimension[1];
}



void ALattice::SetName(string s)
{
  Name=s;
}

void ALattice::print_equations()
{
  cout << "f"<<getNumOut()<<"= ";
  for (unsigned int i=0; i<equation.size();i++)
    {
      if (i!=0)
	cout << " +";
      for(unsigned int j=0; j<equation[i].size(); j++)
	{
	  if (equation[i][j].getSign())
	    cout <<" x"<< equation[i][j].getLit();
	  if (!equation[i][j].getSign())
	    cout <<" !x"<< equation[i][j].getLit();
	}
      
    }
  cout<<endl<<"fd"<<getNumOut()<<"= ";
  
  for (unsigned int i=0; i<equation_dual.size();i++)
    {
      if (i!=0)
	cout << " +";
      for(unsigned int j=0; j<equation_dual[i].size(); j++)
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
  // char firstChar;
  string line;
  //int outputNum=0, inputNum=0;
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
                      //for(unsigned int t=0; t < minterm.size(); t++) 
                      //   cout <<"D"<< minterm[t].getLit();
                      //cout << endl;
		    }
		  else
		    {
		      equation.push_back(minterm);
                      //for(unsigned int t=0; t < minterm.size(); t++)
                      // cout <<"L"<< minterm[t].getLit();
                      //cout << endl;
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
  for (unsigned int Di=0; Di<equation_dual.size();Di++)  // for each row
    {
      appRow.clear();
      for  (unsigned int i=0; i<equation.size();i++)
	{
	  appRow.push_back(FindCommonLiteral(equation[i], equation_dual[Di]));
	}
      //  cout <<"write new row"<<endl;
      Content.push_back(appRow);
    }
  //  cout << equation_dual.size()<<endl;

  SetDimension(equation.size(),equation_dual.size());
}

void ALattice::BuildLattice_multi()
{
  vector< vector< AElement > > appRow;
  for (unsigned int Di=0; Di<equation_dual.size();Di++)  // for each row
    {
      appRow.clear();
      for  (unsigned int i=0; i<equation.size();i++)
	{
	  appRow.push_back(FindCommonLiteral_multi(equation[i], equation_dual[Di]));
	}
      //  cout <<"write new row"<<endl;
      ContentMulti.push_back(appRow);
    }
  //  cout << equation_dual.size()<<endl;
  // SetDimension(equation_dual.size(),equation.size());
  SetDimension(equation.size(),equation_dual.size());
}

AElement ALattice::FindCommonLiteral(vector<AElement> term, vector<AElement> termD)
{
  for(unsigned int i=0; i<term.size(); i++)
    for(unsigned int j=0; j<termD.size(); j++)
      {
	//	cout << "termD"<<termD[j].getSign()<<" "<<termD[j].getLit()<<" ---"<<"term"<<term[i].getSign()<<" "<<term[i].getLit()<<endl;
	if (term[i].getLit()==termD[j].getLit() && term[i].getSign()==termD[j].getSign())
	  {
	    //cout << endl;
	    return term[i]; 
          }
      }
}


vector<AElement> ALattice::FindCommonLiteral_multi(vector<AElement> term, vector<AElement> termD)
{
  vector<AElement> MatchVector;
  MatchVector.clear();
  for(unsigned int i=0; i<term.size(); i++)
    for(unsigned int j=0; j<termD.size(); j++)
      {
	//	cout << "termD"<<termD[j].getSign()<<" "<<termD[j].getLit()<<" ---"<<"term"<<term[i].getSign()<<" "<<term[i].getLit()<<endl;
	if (term[i].getLit()==termD[j].getLit() && term[i].getSign()==termD[j].getSign())
	  {
	    //cout << endl;
	    MatchVector.push_back(term[i]);
          }
      }
  return MatchVector;
}

void ALattice::PrintLattice()
{
  for(unsigned int j=0; j<Content.size();j++) //for each row
    {
      for(unsigned int i=0; i<Content[j].size();i++) //for each column
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

void ALattice::PrintLattice_multi()
{
  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {	 
          if (i!=0) cout << " | ";
          for(unsigned int k=0; k
                <ContentMulti[j][i].size();k++) //for each site
            {
              if(!ContentMulti[j][i][k].getSign())
                cout << "-";
              else
                cout << " ";
              
              cout << ContentMulti[j][i][k].getLit();
              if (k<(ContentMulti[j][i].size() - 1 ))
                cout << " ; " ; 
                
            }
        }
      cout<<endl;
    }
  cout<<endl<<endl;
}

void ALattice::Print2File(string FileName)
{
  fstream outputfile;
  outputfile.open(FileName.c_str() , ios::out); // read dimension of .i e .o 
  for(unsigned int j=0; j<Content.size();j++) //for each row
    {
      for(unsigned int i=0; i<Content[j].size();i++) //for each column
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
  //  outputfile<<endl<<endl;
}

void ALattice::Print2File_multi(string FileName)
{
  fstream outputfile;
  outputfile.open(FileName.c_str() , ios::out); // read dimension of .i e .o
  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {	 
          if (i!=0) outputfile << " | ";
          for(unsigned int k=0; k
                <ContentMulti[j][i].size();k++) //for each site
            {
              if(!ContentMulti[j][i][k].getSign())
                outputfile << "-";
              else
                outputfile << " ";
              
              outputfile << ContentMulti[j][i][k].getLit();
              if (k<(ContentMulti[j][i].size() - 1 ))
                outputfile << " ; " ; 
                
            }
        }
      outputfile<<endl;
    }
  outputfile<<endl<<endl;
  outputfile.close();
}

