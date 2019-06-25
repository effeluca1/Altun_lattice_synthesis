
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
  string MultiUnary;            // indicate if is multiunary
  vector< vector<AElement> > Content; // vector osf all elements (Column x Row)
  vector< vector< vector <AElement> > > ContentMulti; // vector osf all elements (Column x Row)
  vector< vector< vector <AElement> > > ContentOpt; // vector osf all elements (Column x Row)
  vector< vector< vector <AElement> > > ContentMultiOrd; // vector osf all elements (Column x Row)
  vector< vector< vector <AElement> > > ContentDisOpt; // vector osf all elements (Column x Row)

  int dimension[2];             // Col Row dimensions
  int NumVar;                   // Number of literals
  int NumOut;                   // Number of outputs
 
  vector< vector<AElement> > equation;
  vector< vector<AElement> > equation_dual;
  AElement FindCommonLiteral(vector<AElement> term, vector<AElement> termD);
  vector< AElement> FindCommonLiteral_multi(vector<AElement> term, vector<AElement> termD);
  
public:
  //int OptFindVar();
  vector<string> OptVecVAR; // vecotr of all the variables for optimization
  vector<string> OptVecVARm; // vecotr of all the variables for multi-variable optimization
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
  void OptFindVarM();
  void print_data(string FileName);
  void print_data_multi(string FileName);
  bool FindOptPos(string lit,int r, int c);
  bool FindOptPosM(string lit,int r, int c);
  void Recompose_optimized(string FileName);
  void optimized_vec_col_M(string FileName, string FileName2);
  void optimized_vec_col(string FileName, string FileName2);
  void optimized_vec_col_row(string FileName, string FileName2,string FileName3);
  void optimized_vec_row(string FileName, string FileName2);
  void OptCost(string Filename);
  void OptLiterals();
  void DisOptLiterals();
  AElement OptimizeMultiValue(unsigned int J, unsigned int I);
  AElement DisOptimizeMultiValue(unsigned int J, unsigned int I);
  void PrintLatticeOpt();
  void PrintLatticeDisOpt();
  void Print2File_multi_opt(string FileName);
  void Print2File_multi_dis_opt(string FileName);
  void setMulti(string s);
  void OptLiterals_M();
  void PrintLatticeOpt_M();
  void Print2File_multi_opt_M(string FileName);
  void optimized_vec_row_M(string FileName, string FileName2);
  void optimized_vec_col_row_M(string FileName, string FileName2, string FileName3);
  
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
  bool operator ==(const AElement& d)  {
    if (lit == d.lit && sign == d.sign) return true;
    else return false;
  }
};

// function prototype declaration
void dual( string inFile, string outFile);


int main(int argc, char *argv[])
{

  printf( "usage:\n-m\t multi variables(per cell) \n-u\t  unary cells  \n-OptCol\t  optimize column \n-OptRow\t  optimize rows  \n-DisOptCol\t  dis-optimize column \n-DisOptRow\t  dis-optimize row \n-OptCol and -OptRow work  with -uand -m (needs GLPK)\n\n// ./AltunSinthesis file.pla lattice -m \n");
  (void)argc;
  fstream inputfile;
  string inFile(argv[1]);
  string MultiUnary(argv[3]);
  bool DISOPTcol=false;
  bool DISOPTrow=false;
  bool OPTcol=false;
  bool OPTrow=false; 
  cout << argc;
  
  if (argc >= 4)
    {
      string OptStr(argv[4]);
      string OptStr2(argv[5]);
      if (OptStr=="-DisOptCol")  DISOPTcol=true;
      if (OptStr=="-OptCol")     OPTcol=true;
      if (OptStr2=="-OptRow")    OPTrow=true; // bisogna fare un ponte per farle tutte e due o un disable della sintesi per fare solo ordinamento
      if (OptStr2=="-DisOptRow") DISOPTrow=true;
    }
  string line;
  
  // cout << "YYYYY" << endl;      
    
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
      app.setMulti(MultiUnary);
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
          
          //  app.Print2File_multi(inFile+".lattice"+i_str.str());
          lattices.push_back(app);
          app.Print2File_multi(inFile+".latticeM"+i_str.str());

          cout << "OPT!" << endl;
          app.BuildLattice();
          app.OptLiterals();
          cout << "OPT!" << endl;
          app.PrintLatticeOpt();
          app.Print2File_multi_opt(inFile+".latticeLITOPT"+i_str.str());
          cout << "OPT!" << endl;

          // DISOPT!!! -- uncomment if needed
          // cout << "DisOPT!" << endl;
          // app.BuildLattice();
          // app.DisOptLiterals();
          // cout << "SisDisOPT!" << endl;
          // app.PrintLatticeDisOpt();
          // app.Print2File_multi_dis_opt(inFile+".latticeLIT_dis_OPT"+i_str.str());
          // cout << "DisOPT!" << endl;
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
      
    
      // if (app.getNumVar()>9)
      //   {
      //     cout << app.getNumVar() <<" VARs are too much! :-P"<<endl;
      //     return 2;
      //   }
      
      cout << "$$$$$$$$$" <<  app.OptVecVAR.size() << "VAR $$$$$$$$$";
      
      if (DISOPTcol==true)
        {
          cout << "########## COLUMN  DISOPTIMIZATION!  ############"<< endl;
          //          int col=app.GetColNum(),row=app.GetRowNum();
          app.OptFindVar();
          string Datafile = inFile+".data"+to_string(i);
          string LogFile = inFile+".ResultColGLPK"+to_string(i);
          string GLPKoutput = inFile+".OutColGLPK"+to_string(i);
          app.print_data(Datafile);
          char* OptCommand;
          OptCommand = (char*)malloc(sizeof(char)*512);
          sprintf(OptCommand,"glpsol --tmlim 360 --memlim 8000 --model ColDisOpt.mod  --data %s --output %s > %s", Datafile.c_str(),GLPKoutput.c_str(),LogFile.c_str());
          cout << OptCommand<< endl;
          system(OptCommand);

          sprintf(OptCommand,"cat %s | grep y | grep val | grep '\\= 1'>app%sCol",LogFile.c_str(),inFile.c_str()); // removes the first and last column that werw added to fix the linear problem mapping
          cout << OptCommand<< endl;
          system(OptCommand);
          app.optimized_vec_col("app"+inFile+"Col", inFile+".lattice"+to_string(i)+"optimizedCol");

          ///  cout <<"EEE"<< GLPKoutput.c_str() ;
          app.OptCost(LogFile.c_str());

          // ---plot results------------------------
          string ResOptCol="ResOptCol";
          char* NAMEappCol;
          NAMEappCol = (char*)malloc(sizeof(char)*512);
          fstream f3, fo;
          fo.open(LogFile.c_str(), ios::in);
          f3.open(ResOptCol.c_str(), ios::app|ios::out);
          f3 << inFile <<i<< " "   << app.GetRowNum()<< " " << app.GetColNum() << " "<< app.OptVecVAR.size() <<" ";

          string COLstring, GAPcol;
          while( getline(fo , line))
            {
              if (line[0]=='T' && line[1]=='i' && line[2]=='m')
                COLstring=line.substr(line.find(':')+1,line.find("sec")-1-line.find(':')) ;
              if (line[0]=='+' && line.find('%') != std::string::npos)
                GAPcol=line.substr(line.find('%')-5,5)  ;     // + 28822: mip =   1.300000000e+02 <=   2.380000000e+02  83.1% (205W
            }
          f3 << COLstring << " " << GAPcol<< endl;
          cout <<" GAP"<< COLstring << " " << GAPcol<< endl;
          f3.close();
          // ---------------------------------------
          cout << "##########  ############  ############"<< endl;
        }

      if (OPTcol==true)
        {
          cout << "########## COLUMN  OPTIMIZATION!  ############"<< endl;
          //          int col=app.GetColNum(),row=app.GetRowNum();
          if (MultiUnary == "-m")
            {
              cout << "***********MULTI************" << endl;
              app.OptFindVarM();
              
              string Datafile_M = inFile+".data_M"+to_string(i);
              string LogFile_M = inFile+".ResultColGLPK_M"+to_string(i);
              string GLPKoutput_M = inFile+".OutColGLPK_M"+to_string(i);

              app.print_data_multi(Datafile_M);
              
              char* OptCommand_M;
              OptCommand_M = (char*)malloc(sizeof(char)*512);
              sprintf(OptCommand_M,"glpsol --tmlim 3600 --memlim 8000 --model ColOpt.mod  --data %s --output %s > %s", Datafile_M.c_str(),GLPKoutput_M.c_str(),LogFile_M.c_str());
              cout << OptCommand_M<< endl;
              system(OptCommand_M);

              sprintf(OptCommand_M,"cat %s | grep y | grep val | grep '\\= 1'>app%sCol",LogFile_M.c_str(),inFile.c_str()); // removes the first and last column that were added to fix the linear problem mapping
              cout << OptCommand_M<< endl;
              system(OptCommand_M);
              app.optimized_vec_col_M("app"+inFile+"Col", inFile+".latticeM"+to_string(i)+"optimizedCol");

              ///  cout <<"EEE"<< GLPKoutput.c_str() ;
              //              cout << "!!!!!! COST !!!!!!!!" << endl;
              app.OptCost(LogFile_M.c_str());
              //              cout << "!!!!!!!!!!!!!!!!!!!!!!"<< endl;

              // ---plot results------------------------
              string ResOptCol="ResOptCol";
              char* NAMEappCol;
              NAMEappCol = (char*)malloc(sizeof(char)*512);
              fstream f3, fo;
              fo.open(LogFile_M.c_str(), ios::in);
              f3.open(ResOptCol.c_str(), ios::app|ios::out);
              f3 << inFile <<i<< " "   << app.GetRowNum()<< " " << app.GetColNum() << " "<< app.OptVecVAR.size() <<" ";

              string COLstring, GAPcol;
              while( getline(fo , line))
                {   
                  if (line[0]=='T' && line[1]=='i' && line[2]=='m')
                    COLstring=line.substr(line.find(':')+1,line.find("sec")-1-line.find(':')) ;
                  if (line[0]=='+' && line.find('%') != std::string::npos)
                    GAPcol=line.substr(line.find('%')-5,5)  ;     // + 28822: mip =   1.300000000e+02 <=   2.380000000e+02  83.1% (205W                       
                }
              f3 << COLstring << " " << GAPcol<< endl;
              cout <<" GAP"<< COLstring << " " << GAPcol<< endl;
              f3.close();
              // ---------------------------------------



              //  aeo;dfojś
              app.OptLiterals_M();
              cout << "OPTM!" << endl;
              app.PrintLatticeOpt_M();

              ostringstream i_str;	// stream used for the conversion
              i_str << i;
              app.Print2File_multi_opt_M(inFile+".latticeLITOPT_col_M_"+i_str.str());
              cout << "OPTM!" << endl;

            }
          else {
            cout << "*******SINGLE*******"<< endl;
            app.OptFindVar();
            string Datafile = inFile+".data"+to_string(i);
            string LogFile = inFile+".ResultColGLPK"+to_string(i);
            string GLPKoutput = inFile+".OutColGLPK"+to_string(i);
            app.print_data(Datafile);
            char* OptCommand;
            OptCommand = (char*)malloc(sizeof(char)*512);
            sprintf(OptCommand,"glpsol --tmlim 3600 --memlim 8000 --model ColOpt.mod  --data %s --output %s > %s", Datafile.c_str(),GLPKoutput.c_str(),LogFile.c_str());
            cout << OptCommand<< endl;
            system(OptCommand);

            sprintf(OptCommand,"cat %s | grep y | grep val | grep '\\= 1'>app%sCol",LogFile.c_str(),inFile.c_str()); // removes the first and last column that were added to fix the linear problem mapping
            cout << OptCommand<< endl;
            system(OptCommand);
            app.optimized_vec_col("app"+inFile+"Col", inFile+".lattice"+to_string(i)+"optimizedCol");

            ///  cout <<"EEE"<< GLPKoutput.c_str() ;
            app.OptCost(LogFile.c_str());

            // ---plot results------------------------
            string ResOptCol="ResOptCol";
            char* NAMEappCol;
            NAMEappCol = (char*)malloc(sizeof(char)*512);
            fstream f3, fo;
            fo.open(LogFile.c_str(), ios::in);
            f3.open(ResOptCol.c_str(), ios::app|ios::out);
            f3 << inFile <<i<< " "   << app.GetRowNum()<< " " << app.GetColNum() << " "<< app.OptVecVAR.size() <<" ";

            string COLstring, GAPcol;
            while( getline(fo , line))
              {   
                if (line[0]=='T' && line[1]=='i' && line[2]=='m')
                  COLstring=line.substr(line.find(':')+1,line.find("sec")-1-line.find(':')) ;
                if (line[0]=='+' && line.find('%') != std::string::npos)
                  GAPcol=line.substr(line.find('%')-5,5)  ;     // + 28822: mip =   1.300000000e+02 <=   2.380000000e+02  83.1% (205W                       
              }
            f3 << COLstring << " " << GAPcol<< endl;
            cout <<" GAP"<< COLstring << " " << GAPcol<< endl;
            f3.close();
            // ---------------------------------------

          }
          
          cout << "##########  ############  ############"<< endl;
        }

      cout<<"OPTTTTY" << OPTrow<< endl;
      if (OPTrow==true)
        {
          cout << "########## ROW  OPTIMIZATION!  ############"<< endl;

          if (MultiUnary == "-m")
            {
              cout << "***********MULTI************" << endl;
              app.OptFindVarM();
              string Datafile_M = inFile+".data_M"+to_string(i);
              string LogFile_M = inFile+".ResultRowGLPK_M"+to_string(i);
              string GLPKoutput_M = inFile+".OutRowGLPK_M"+to_string(i);
                  
              app.print_data_multi(Datafile_M);
                  
              char* OptCommand_M;
              OptCommand_M = (char*)malloc(sizeof(char)*512);
              sprintf(OptCommand_M,"glpsol  --tmlim 3600 --memlim 8000 --model RowOpt.mod  --data %s --output %s > %s", Datafile_M.c_str(),GLPKoutput_M.c_str(),LogFile_M.c_str());
              cout << OptCommand_M<< endl;
          
              system(OptCommand_M);

              sprintf(OptCommand_M,"cat %s | grep y | grep val | grep '\\= 1'>app%sRow",LogFile_M.c_str(),inFile.c_str()); // removes the first and last column that werw added to fix the linear problem mapping
              cout << OptCommand_M<< endl;
              system(OptCommand_M);

                      
              app.optimized_vec_row_M("app"+inFile+"Row", inFile+".latticeM"+to_string(i)+"optimizedRow");

              // cout <<"EEE"<< GLPKoutput.c_str() ;
              app.OptCost(LogFile_M.c_str());


              
              // ---plot results------------------------
              string ResOptRow="ResOptRow";
              char* NAMEappRow;
              NAMEappRow = (char*)malloc(sizeof(char)*512);
              fstream f3, fo;
              fo.open(LogFile_M.c_str(), ios::in);
              f3.open(ResOptRow.c_str(), ios::app|ios::out);
              f3 << inFile <<i<< " "   << app.GetRowNum()<< " " << app.GetColNum() << " "<< app.OptVecVAR.size() <<" ";

              string ROWstring, GAProw;
              while( getline(fo , line))
                {
                  if (line[0]=='T' && line[1]=='i' && line[2]=='m')
                    ROWstring=line.substr(line.find(':')+1,line.find("sec")-1-line.find(':')) ;
                  if (line[0]=='+' && line.find('%') != std::string::npos)
                    GAProw=line.substr(line.find_last_of('%')-5,5)  ;     // + 28822: mip =   1.300000000e+02 <=   2.380000000e+02  83.1% (205W                       
                }
              f3 << ROWstring << " " << GAProw<< endl;
              f3.close();

              app.OptLiterals_M();
              cout << "OPTM!" << endl;
              app.PrintLatticeOpt_M();

              ostringstream i_str;	// stream used for the conversion
              i_str << i;
              app.Print2File_multi_opt_M(inFile+".latticeLITOPT_row_M_"+i_str.str());
              cout << "OPTM!" << endl;
               
            }
          else
            {
              //          int col=app.GetColNum(),row=app.GetRowNum();
              app.OptFindVar();
              string Datafile = inFile+".data"+to_string(i);
              string LogFile = inFile+".ResultRowGLPK"+to_string(i);
              string GLPKoutput = inFile+".OutRowGLPK"+to_string(i);
              app.print_data(Datafile);
              char* OptCommand;
              OptCommand = (char*)malloc(sizeof(char)*512);
              sprintf(OptCommand,"glpsol  --tmlim 3600 --memlim 8000 --model RowOpt.mod  --data %s --output %s > %s", Datafile.c_str(),GLPKoutput.c_str(),LogFile.c_str());
              cout << OptCommand<< endl;
          
              system(OptCommand);

              sprintf(OptCommand,"cat %s | grep y | grep val | grep '\\= 1'>app%sRow",LogFile.c_str(),inFile.c_str()); // removes the first and last column that werw added to fix the linear problem mapping
              cout << OptCommand<< endl;
              system(OptCommand);
              app.optimized_vec_row("app"+inFile+"Row", inFile+".lattice"+to_string(i)+"optimizedRow");

              // cout <<"EEE"<< GLPKoutput.c_str() ;
              app.OptCost(LogFile.c_str());

              // ---plot results------------------------
              string ResOptRow="ResOptRow";
              char* NAMEappRow;
              NAMEappRow = (char*)malloc(sizeof(char)*512);
              fstream f3, fo;
              fo.open(LogFile.c_str(), ios::in);
              f3.open(ResOptRow.c_str(), ios::app|ios::out);
              f3 << inFile <<i<< " "   << app.GetRowNum()<< " " << app.GetColNum() << " "<< app.OptVecVAR.size() <<" ";

              string ROWstring, GAProw;
              while( getline(fo , line))
                {
                  if (line[0]=='T' && line[1]=='i' && line[2]=='m')
                    ROWstring=line.substr(line.find(':')+1,line.find("sec")-1-line.find(':')) ;
                  if (line[0]=='+' && line.find('%') != std::string::npos)
                    GAProw=line.substr(line.find_last_of('%')-5,5)  ;     // + 28822: mip =   1.300000000e+02 <=   2.380000000e+02  83.1% (205W                       
                }
              f3 << ROWstring << " " << GAProw<< endl;
              f3.close();         
              // ---------------------------------------

            }
     
          cout << "##########  ############  ############"<< endl;
        }
      cout << "##" << endl;

      if (DISOPTrow==true)
        {
          cout << "########## ROW  DISOPTIMIZATION!  ############"<< endl;
          //          int col=app.GetColNum(),row=app.GetRowNum();
          app.OptFindVar();
          string Datafile = inFile+".data"+to_string(i);
          string LogFile = inFile+".ResultRowGLPK"+to_string(i);
          string GLPKoutput = inFile+".OutRowGLPK"+to_string(i);
          app.print_data(Datafile);
          char* OptCommand;
          OptCommand = (char*)malloc(sizeof(char)*512);
          sprintf(OptCommand,"glpsol  --tmlim 360 --memlim 8000 --model RowDisOpt.mod  --data %s --output %s > %s", Datafile.c_str(),GLPKoutput.c_str(),LogFile.c_str());
          cout << OptCommand<< endl;
          
          system(OptCommand);

          sprintf(OptCommand,"cat %s | grep y | grep val | grep '\\= 1'>app%sRow",LogFile.c_str(),inFile.c_str()); // removes the first and last column that werw added to fix the linear problem mapping
          cout << OptCommand<< endl;
          system(OptCommand);
          app.optimized_vec_row("app"+inFile+"Row", inFile+".lattice"+to_string(i)+"optimizedRow");

          // cout <<"EEE"<< GLPKoutput.c_str() ;
          app.OptCost(LogFile.c_str());

          // ---plot results------------------------
          string ResOptRow="ResOptRow";
          char* NAMEappRow;
          NAMEappRow = (char*)malloc(sizeof(char)*512);
          fstream f3, fo;
          fo.open(LogFile.c_str(), ios::in);
          f3.open(ResOptRow.c_str(), ios::app|ios::out);
          f3 << inFile <<i<< " "   << app.GetRowNum()<< " " << app.GetColNum() << " "<< app.OptVecVAR.size() <<"K ";

          string ROWstring, GAProw;
          while( getline(fo , line))
            {
              if (line[0]=='T' && line[1]=='i' && line[2]=='m')
                ROWstring=line.substr(line.find(':')+1,line.find("sec")-1-line.find(':')) ;
              if (line[0]=='+' && line.find('%') != std::string::npos)
                GAProw=line.substr(line.find_last_of('%')-5,5)  ;     // + 28822: mip =   1.300000000e+02 <=   2.380000000e+02  83.1% (205W
            }
          f3 << ROWstring << " " << GAProw<< endl;
          f3.close();      
          // ---------------------------------------

         
          cout << "##########  ############  ############"<< endl;
        }
      cout << "##" << endl;

      
      if ((OPTrow==true && OPTcol==true)|| (DISOPTrow==true && DISOPTcol==true))
        {
          cout << "########## ROW & COLUMN  OPTIMIZATION(/DIS)!  ############"<< endl;
          cout << "NAME " << "app"+inFile+"Col "<<  inFile+".latticeM"+to_string(i)+"optimizedRowCol "<< "app"+inFile+"Row" << endl;

          app.optimized_vec_col_row_M("app"+inFile+"Col", inFile+".latticeM"+to_string(i)+"optimizedRowCol","app"+inFile+"Row");
          
        }

      app.OptLiterals_M();
      cout << "OPTM!" << endl;
      app.PrintLatticeOpt_M();

      ostringstream i_str;	// stream used for the conversion
      i_str << i;
      app.Print2File_multi_opt_M(inFile+".latticeLITOPT_ColRow_M_"+i_str.str());
      cout << "OPTM!" << endl;

      
    }

  
return 0;
}


// ***FUNCTIONS***

void ALattice::DisOptLiterals()
{         
  int MultiMap[ContentMulti.size()][ContentMulti[0].size()];
  ContentDisOpt=ContentMulti;
  
  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {
          if (ContentMulti[j][i].size()==1)
            MultiMap[j][i]=1;
          else
            MultiMap[j][i]=0;
        }
    }

  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {

          if (ContentMulti[j][i].size()>1)
            {

              ContentDisOpt[j][i][0]=DisOptimizeMultiValue(j,i);
              ContentDisOpt[j][i].resize(1);
              cout << j << " "<< i << " "<<"Out " <<ContentDisOpt[j][i][0].getSign()<< ContentDisOpt[j][i][0].getLit()<<endl;
            }
          else cout << j << " "<< i <<  "single " << endl;
        }
    }
}

AElement ALattice::DisOptimizeMultiValue(unsigned int J, unsigned int I)
{
  vector<int> NumOccurrU(ContentDisOpt[J][I].size(), 0);
  vector<int> NumOccurrM(ContentDisOpt[J][I].size(), 0);
  vector<int> NumOccurrMplusU(ContentDisOpt[J][I].size(), 0);
  vector<int> NumOccurrUsorted(ContentDisOpt[J][I].size(), 0);
  vector<int> NumOccurrMsorted(ContentDisOpt[J][I].size(), 0);
  vector<int> NumOccurrMplusUsorted(ContentDisOpt[J][I].size(), 0);

  for (unsigned int k=0 ; k<ContentDisOpt[J][I].size(); k++) // check Fixed literals
    {
      if( I<ContentDisOpt[J].size()-1) // NOT IN DX COLUMN
        {
          if (ContentDisOpt[J][I+1].size()==1  ) // if is unary
            {
              if (ContentDisOpt[J][I+1][0] == ContentDisOpt[J][I][k])
                NumOccurrU[k]++;
            }
          else  if(ContentDisOpt[J][I+1].size()>1)
            { 
              if  (find (ContentDisOpt[J][I+1].begin(), ContentDisOpt[J][I+1].end(), ContentDisOpt[J][I][k])  != ContentDisOpt[J][I+1].end() )
                NumOccurrM[k]++;
            }
        }
 
      if(J<ContentDisOpt.size()-1) // not in  bottom row
        {
          if (ContentDisOpt[J+1][I].size()==1  ) // if is unary  
            {
              if (ContentDisOpt[J+1][I][0] == ContentDisOpt[J][I][k])
                NumOccurrU[k]++;
            }
          else if  (ContentDisOpt[J+1][I].size()>1  ) // if is multi
            {
              if  (find (ContentDisOpt[J+1][I].begin(), ContentDisOpt[J+1][I].end(), ContentDisOpt[J][I][k])  != ContentDisOpt[J+1][I].end() )
                NumOccurrM[k]++;
            }
        }

      if( J>0) // not in left column
        {
          if (ContentDisOpt[J-1][I].size()==1) // if is unary 
            {
              if (ContentDisOpt[J-1][I][0] == ContentDisOpt[J][I][k])
                NumOccurrU[k]++;
            }
          else if  (ContentDisOpt[J-1][I].size()>1  ) // if is multi
            {
              if  (find (ContentDisOpt[J-1][I].begin(), ContentDisOpt[J-1][I].end(), ContentDisOpt[J][I][k])  != ContentDisOpt[J-1][I].end() )
                NumOccurrM[k]++;
            }
        }

      if( I>0 ) // not in top row
        {
          if (ContentDisOpt[J][I-1].size()==1) // if is unary
            {
              if (ContentDisOpt[J][I-1][0] == ContentDisOpt[J][I][k])
                NumOccurrU[k]++;
            }
          else if  (ContentDisOpt[J][I-1].size()>1 ) // if is multi
            {
              if  (find (ContentDisOpt[J][I-1].begin(), ContentDisOpt[J][I-1].end(), ContentDisOpt[J][I][k])  != ContentDisOpt[J][I-1].end() )
                NumOccurrM[k]++;
            }
          //   cout << "###OPT IN!" << endl;
        }
    }


  
  for (unsigned int e=0; e < ContentDisOpt[J][I].size(); e++ )
    {
      NumOccurrUsorted[e] = NumOccurrU[e];
      NumOccurrMsorted[e] = NumOccurrM[e];
      NumOccurrMplusU[e]= NumOccurrU[e] + NumOccurrM[e];
      NumOccurrMplusUsorted[e]= NumOccurrU[e] + NumOccurrM[e];
    }
      
 
  sort(NumOccurrUsorted.begin(), NumOccurrUsorted.end()); // sort the vector
  sort(NumOccurrMsorted.begin(), NumOccurrMsorted.end());    // sort the vector
  sort(NumOccurrMplusUsorted.begin(), NumOccurrMplusUsorted.end());    // sort the vector


  // 1-- scelgo letterale che non ha occorrenze
  
  // 2--- scelgo il letterale con meno occorrenze

  // 3 -- in pratica scelgo sempre il primo letterale del vettore
  



  

  // if (NumOccurrUsorted[ContentDisOpt[J][I].size()-1] > NumOccurrUsorted[ContentDisOpt[J][I].size()-2])
  //   {
  for (unsigned int e=0; e < ContentDisOpt[J][I].size(); e++ )
    {//cout << "HEREU" <<NumOccurrUsorted[ContentDisOpt[J][I].size() -1]<< " "<< NumOccurrU[e] <<  endl;
      if (NumOccurrUsorted[0]== NumOccurrU[e])
        {
          cout <<"inU "<< ContentDisOpt[J][I][e].getSign() << ContentDisOpt[J][I][e].getLit()<<endl;
          return ContentDisOpt[J][I][e];
          break;
        }
    }
  //   }
  
  // else if (NumOccurrMplusUsorted[ContentDisOpt[J][I].size()-1] > NumOccurrMplusUsorted[ContentDisOpt[J][I].size()-2])
  //   { 
  //     for (unsigned int e=0; e < ContentDisOpt[J][I].size(); e++ )
  //       { //cout << "HERE" <<NumOccurrMplusUsorted[ContentDisOpt[J][I].size() -1]<< " "<< NumOccurrMplusU[e] <<  endl;
  //         if (NumOccurrMplusUsorted[ContentDisOpt[J][I].size()-1]== NumOccurrMplusU[e])
  //           { 
  //             cout <<"inM "<< ContentDisOpt[J][I][e].getSign() << ContentDisOpt[J][I][e].getLit()<<endl;
  //             return ContentDisOpt[J][I][e];
  //             break;
  //           }
  //       }
  //   }
  // else
  //   {
  //   for (unsigned int e=0; e < ContentDisOpt[J][I].size(); e++ )
  //       { cout << "HERE" <<NumOccurrMplusUsorted[ContentDisOpt[J][I].size() -1]<< " "<< NumOccurrMplusU[e] <<  endl;
  //         if (NumOccurrMplusUsorted[ContentDisOpt[J][I].size()-1]== NumOccurrMplusU[e])
  //           { 
  //             cout <<"inP "<< ContentDisOpt[J][I][e].getSign() << ContentDisOpt[J][I][e].getLit()<<endl;
  //             return ContentDisOpt[J][I][e];
  //             break;
  //           }
  //       }
  //   }
} 








void ALattice::OptLiterals()
{         
  int MultiMap[ContentMulti.size()][ContentMulti[0].size()];

  ContentOpt=ContentMulti;
  
  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {
          if (ContentMulti[j][i].size()==1)
            MultiMap[j][i]=1;
          else
            MultiMap[j][i]=0;
        }
    }

  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {
          if (ContentMulti[j][i].size()>1)
            {
              ContentOpt[j][i][0]=OptimizeMultiValue(j,i);
              ContentOpt[j][i].resize(1);
              cout << i << " "<< j << " "<<"Out " <<ContentOpt[j][i][0].getSign()<< ContentOpt[j][i][0].getLit()<<endl;
            }
          else cout << i << " "<< j <<  "single " << endl;
        }
    }

  
}

void ALattice::OptLiterals_M()
{         
  int MultiMap[ContentMulti.size()][ContentMulti[0].size()];

  ContentOpt=ContentMultiOrd;
  for(unsigned int j=0; j<ContentMultiOrd.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMultiOrd[j].size();i++) //for each column
        {
          if (ContentMultiOrd[j][i].size()==1)
            MultiMap[j][i]=1;
          else
            MultiMap[j][i]=0;
          cout <<"MM" <<  MultiMap[j][i] << endl;
          
        }
    }

  //DEBUG
  cout << "DEBUG" << endl;
  
   for(unsigned int j=0; j<ContentMultiOrd.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMultiOrd[j].size();i++) //for each column
        {	 
          if (i!=0) cout << " | ";
          for(unsigned int k=0; k<ContentMultiOrd[j][i].size();k++) //for each site
            {
              if(!ContentMultiOrd[j][i][k].getSign())
                cout << "-";
              else
                cout << " ";
              
              cout << ContentMultiOrd[j][i][k].getLit();
              if (k<(ContentMultiOrd[j][i].size() - 1 ))
                cout << " ; " ; 
                
            }
        }
      cout<<endl;
    }
  cout<<endl<<endl;



   for(unsigned int j=0; j<ContentMultiOrd.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMultiOrd[j].size();i++) //for each column
        {
          cout << MultiMap[j][i];
        }
      cout << endl;
    }
   
  //END DEBUG

  for(unsigned int j=0; j<ContentMultiOrd.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMultiOrd[j].size();i++) //for each column
        {
          if (ContentMultiOrd[j][i].size()>1)
            {
              ContentOpt[j][i][0]=OptimizeMultiValue(j,i);
              ContentOpt[j][i].resize(1);
              cout << j << " "<< i << " "<<"Out " <<ContentOpt[j][i][0].getSign()<< ContentOpt[j][i][0].getLit()<<endl;
            }
          else cout << j << " "<< i <<  "single " << endl;
        }
    }
}

AElement ALattice::OptimizeMultiValue(unsigned int J, unsigned int I)
{
  vector<int> NumOccurrU(ContentOpt[J][I].size(), 0);
  vector<int> NumOccurrM(ContentOpt[J][I].size(), 0);
  vector<int> NumOccurrMplusU(ContentOpt[J][I].size(), 0);
  vector<int> NumOccurrUsorted(ContentOpt[J][I].size(), 0);
  vector<int> NumOccurrMsorted(ContentOpt[J][I].size(), 0);
  vector<int> NumOccurrMplusUsorted(ContentOpt[J][I].size(), 0);

  
  //int  NumOccurrU[ContentOpt[J][I].size()];
  //int  NumOccurrM[ContentOpt[J][I].size()];
  //int  NumOccurrUsorted[ContentOpt[J][I].size()];
  //int  NumOccurrMsorted[ContentOpt[J][I].size()];

  // for (unsigned int e=0; e < ContentOpt[J][I].size(); e++ )
  //   {
  //     NumOccurrU[e]=0;
  //     NumOccurrM[e]=0;
  //   }
  // int NumCells;

  for (unsigned int k=0 ; k<ContentOpt[J][I].size(); k++) // check Fixed literals
    {
      if( I<ContentOpt[J].size()-1) // NOT IN DX COLUMN
        {
          if (ContentOpt[J][I+1].size()==1  ) // if is unary
            {
              if (ContentOpt[J][I+1][0] == ContentOpt[J][I][k])
                NumOccurrU[k]++;
            }
          else  if(ContentOpt[J][I+1].size()>1)
            { 
              if  (find (ContentOpt[J][I+1].begin(), ContentOpt[J][I+1].end(), ContentOpt[J][I][k])  != ContentOpt[J][I+1].end() )
                NumOccurrM[k]++;
            }
        }
 
      if(J<ContentOpt.size()-1) // not in  bottom row
        {
          if (ContentOpt[J+1][I].size()==1  ) // if is unary  
            {
              if (ContentOpt[J+1][I][0] == ContentOpt[J][I][k])
                NumOccurrU[k]++;
            }
          else if  (ContentOpt[J+1][I].size()>1  ) // if is multi
            {
              if  (find (ContentOpt[J+1][I].begin(), ContentOpt[J+1][I].end(), ContentOpt[J][I][k])  != ContentOpt[J+1][I].end() )
                NumOccurrM[k]++;
            }
        }

      if( J>0) // not in left column
        {
          if (ContentOpt[J-1][I].size()==1) // if is unary 
            {
              if (ContentOpt[J-1][I][0] == ContentOpt[J][I][k])
                NumOccurrU[k]++;
            }
          else if  (ContentOpt[J-1][I].size()>1  ) // if is multi
            {
              if  (find (ContentOpt[J-1][I].begin(), ContentOpt[J-1][I].end(), ContentOpt[J][I][k])  != ContentOpt[J-1][I].end() )
                NumOccurrM[k]++;
            }
        }

      if( I>0 ) // not in top row
        {
          if (ContentOpt[J][I-1].size()==1) // if is unary
            {
              if (ContentOpt[J][I-1][0] == ContentOpt[J][I][k])
                NumOccurrU[k]++;
            }
          else if  (ContentOpt[J][I-1].size()>1 ) // if is multi
            {
              if  (find (ContentOpt[J][I-1].begin(), ContentOpt[J][I-1].end(), ContentOpt[J][I][k])  != ContentOpt[J][I-1].end() )
                NumOccurrM[k]++;
            }
          //   cout << "###OPT IN!" << endl;
        }
    }


  
  for (unsigned int e=0; e < ContentOpt[J][I].size(); e++ )
    {
      NumOccurrUsorted[e] = NumOccurrU[e];
      NumOccurrMsorted[e] = NumOccurrM[e];
      NumOccurrMplusU[e]= NumOccurrU[e] + NumOccurrM[e];
      NumOccurrMplusUsorted[e]= NumOccurrU[e] + NumOccurrM[e];
    }
      
 
  sort(NumOccurrUsorted.begin(), NumOccurrUsorted.end()); // sort the vector
  sort(NumOccurrMsorted.begin(), NumOccurrMsorted.end());    // sort the vector
  sort(NumOccurrMplusUsorted.begin(), NumOccurrMplusUsorted.end());    // sort the vector


  if (NumOccurrUsorted[ContentOpt[J][I].size()-1] > NumOccurrUsorted[ContentOpt[J][I].size()-2])
    {
      for (unsigned int e=0; e < ContentOpt[J][I].size(); e++ )
        {//cout << "HEREU" <<NumOccurrUsorted[ContentOpt[J][I].size() -1]<< " "<< NumOccurrU[e] <<  endl;
          if (NumOccurrUsorted[ContentOpt[J][I].size() -1]== NumOccurrU[e])
            {
              cout <<"inU "<< ContentOpt[J][I][e].getSign() << ContentOpt[J][I][e].getLit()<<endl;
              return ContentOpt[J][I][e];
              break;
            }
        }
    }
  
  else if (NumOccurrMplusUsorted[ContentOpt[J][I].size()-1] > NumOccurrMplusUsorted[ContentOpt[J][I].size()-2])
    { 
      for (unsigned int e=0; e < ContentOpt[J][I].size(); e++ )
        { //cout << "HERE" <<NumOccurrMplusUsorted[ContentOpt[J][I].size() -1]<< " "<< NumOccurrMplusU[e] <<  endl;
          if (NumOccurrMplusUsorted[ContentOpt[J][I].size()-1]== NumOccurrMplusU[e])
            { 
              cout <<"inM "<< ContentOpt[J][I][e].getSign() << ContentOpt[J][I][e].getLit()<<endl;
              return ContentOpt[J][I][e];
              break;
            }
        }
    }
  else
    {
      for (unsigned int e=0; e < ContentOpt[J][I].size(); e++ )
        { cout << "HERE" <<NumOccurrMplusUsorted[ContentOpt[J][I].size() -1]<< " "<< NumOccurrMplusU[e] <<  endl;
          if (NumOccurrMplusUsorted[ContentOpt[J][I].size()-1]== NumOccurrMplusU[e])
            { 
              cout <<"inP "<< ContentOpt[J][I][e].getSign() << ContentOpt[J][I][e].getLit()<<endl;
              return ContentOpt[J][I][e];
              break;
            }
        }
    }
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


void ALattice::optimized_vec_col_row(string FileName, string FileName2, string FileName3)
{
  fstream f,fR;
  string line;
  vector<int> orderOptC, orderOptR;
  f.open(FileName.c_str(), ios::in);
  fR.open(FileName3.c_str(), ios::in);
  while( getline(f , line))
    {
      orderOptC.push_back(0);
    }
  f.close();

  while( getline(fR , line))
    {
      orderOptR.push_back(0);
    }
  fR.close();

  
  f.open(FileName.c_str(), ios::in);
  fR.open(FileName3.c_str(), ios::in);
  while( getline(f , line))
    {
      int i=0;
      int arrive= stoi(line.substr(line.find(',')+1,line.find(']')- line.find(',')-1));
      int start = stoi(line.substr(2, line.find(',')-2).c_str());
      cout << "ASC "<<start<<","<< arrive<<endl;
      //    orderOptC[start-1]=arrive-1;
      orderOptC[start-1]=arrive-1;
      i++;
    }
  f.close();
  cout <<FileName3<< "ASR ";
  while( getline(fR , line))
    {
      int i=0;
      int arrive= stoi(line.substr(line.find(',')+1,line.find(']')- line.find(',')-1));
      int start = stoi(line.substr(2, line.find(',')-2).c_str());
      cout << "ASR "<<start<<","<< arrive<<endl;
      //    orderOpt[start-1]=arrive-1;
      orderOptR[start-1]=arrive-1;
      i++;
    }
  fR.close();
  for(unsigned int i=0; i<orderOptC.size();i++) //for each column
    {
      cout << "EC"<< orderOptC[i]<<endl;

    }
  for(unsigned int i=0; i<orderOptR.size();i++) //for each column
    {
      cout << "ER"<< orderOptR[i]<<endl;

    }
  cout << "## print optimized lattice ##" << endl;
  for(unsigned int j=0; j<Content.size();j++) //for each row
    {
      for(unsigned int i=0; i<Content[j].size();i++) //for each column
        {
	 
          if (i!=0) cout << " | ";
          if(!Content[orderOptR[j]][orderOptC[i]].getSign())
            cout << "-";
          else
            cout << " ";
          cout << Content[orderOptR[j]][orderOptC[i]].getLit();
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
          if(!Content[orderOptR[j]][orderOptC[i]].getSign())
            f2 << "-";
          else
            f2 << " ";
          f2 << Content[orderOptR[j]][orderOptC[i]].getLit();
        }
      f2<<endl;
    }
  



  f2.close();
  cout<<"##"<<endl<<endl;
}

void ALattice::optimized_vec_col_row_M(string FileName, string FileName2, string FileName3)
{
  ContentMultiOrd = ContentMulti;
  fstream f,fR;
  string line;
  vector<int> orderOptC, orderOptR;
  f.open(FileName.c_str(), ios::in);
  fR.open(FileName3.c_str(), ios::in);
  while( getline(f , line))
    {
      orderOptC.push_back(0);
    }
  f.close();

  while( getline(fR , line))
    {
      orderOptR.push_back(0);
    }
  fR.close();

  
  f.open(FileName.c_str(), ios::in);
  fR.open(FileName3.c_str(), ios::in);
  while( getline(f , line))
    {
      int i=0;
      int arrive= stoi(line.substr(line.find(',')+1,line.find(']')- line.find(',')-1));
      int start = stoi(line.substr(2, line.find(',')-2).c_str());
      cout << "ASC "<<start<<","<< arrive<<endl;
      //    orderOptC[start-1]=arrive-1;
      orderOptC[start-1]=arrive-1;
      i++;
    }
  f.close();
  cout <<FileName3<< "ASR ";
  while( getline(fR , line))
    {
      int i=0;
      int arrive= stoi(line.substr(line.find(',')+1,line.find(']')- line.find(',')-1));
      int start = stoi(line.substr(2, line.find(',')-2).c_str());
      cout << "ASR "<<start<<","<< arrive<<endl;
      //    orderOpt[start-1]=arrive-1;
      orderOptR[start-1]=arrive-1;
      i++;
    }
  fR.close();
  for(unsigned int i=0; i<orderOptC.size();i++) //for each column
    {
      cout << "EC"<< orderOptC[i]<<endl;

    }
  for(unsigned int i=0; i<orderOptR.size();i++) //for each column
    {
      cout << "ER"<< orderOptR[i]<<endl;

    }
  cout << "## print optimized lattice ##" << endl;
  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {
	   ContentMultiOrd[j][i] =  ContentMulti[orderOptR[j]][orderOptC[i]];
          if (i!=0) cout << " | ";
           for(unsigned int k=0; k<ContentMulti[orderOptR[j]][orderOptC[i]].size();k++) //for each site
            {
          if(!ContentMulti[orderOptR[j]][orderOptC[i]][k].getSign())
            cout << "-";
          else
            cout << " ";
          cout << ContentMulti[orderOptR[j]][orderOptC[i]][k].getLit();

          
              if (k<(ContentMulti[orderOptR[j]][orderOptC[i]].size() - 1 ))
                cout << " ; " ; 
      

            }
        }
      
      cout<<endl;
    }
  cout<<endl<<endl;
  

  //string FileName2=FileName + "optimized";
  cout<< FileName2;
  fstream f2;
  f2.open(FileName2.c_str(), ios::out);

  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {
	 
          if (i!=0) f2 << " | ";
          for(unsigned int k=0; k<ContentMulti[orderOptR[j]][orderOptC[i]].size();k++) //for each site
            {
             if(!ContentMulti[orderOptR[j]][orderOptC[i]][k].getSign())
            f2 << "-";
          else
            f2 << " ";
          f2 << ContentMulti[orderOptR[j]][orderOptC[i]][k].getLit();

              if (k<(ContentMulti[orderOptR[j]][orderOptC[i]].size() - 1 ))
                f2 << " ; " ; 
            }
        }
      f2<<endl;
    }
  



  f2.close();
  cout<<"##"<<endl<<endl;
}


void ALattice::optimized_vec_row_M(string FileName, string FileName2)
{
  ContentMultiOrd = ContentMulti;
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
      cout << "ASR "<<start<<","<< arrive<<endl;
      //    orderOpt[start-1]=arrive-1;
      orderOpt[start-1]=arrive-1;
      i++;
    }

  for(unsigned int i=0; i<orderOpt.size();i++) //for each column
    {
      cout << "E"<< orderOpt[i]<<endl;

    }
  cout << "## print optimized lattice ##" << endl;
  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      ContentMultiOrd[j] =  ContentMulti[orderOpt[j]];
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {
          if (i!=0) cout << " | ";
          //      for(unsigned int k=0; k<ContentMulti[j][orderOpt[i]].size();k++) //for each site
          for(unsigned int k=0; k<ContentMulti[orderOpt[j]][i].size();k++) //for each site
            {
              if(!ContentMulti[orderOpt[j]][i][k].getSign())
                cout << "-";
              else
                cout << " ";
              cout << ContentMulti[orderOpt[j]][i][k].getLit();


              if (k<(ContentMulti[orderOpt[j]][i].size() - 1 ))
                cout << " ; " ; 
      
            }
        }
      
      cout<<endl;
    }
  cout<<endl<<endl;
  

  //string FileName2=FileName + "optimized";
  cout<< FileName2;
  fstream f2;
  f2.open(FileName2.c_str(), ios::out);

  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[orderOpt[j]].size();i++) //for each column
        {
          if (i!=0) f2 << " | ";
          for(unsigned int k=0; k<ContentMulti[orderOpt[j]][i].size();k++) //for each site
            {
              
              //    if (i!=0) f2 << " | ";
              if(!ContentMulti[orderOpt[j]][i][k].getSign())
                f2 << "-";
              else
                f2 << " ";
              f2 << ContentMulti[orderOpt[j]][i][k].getLit();
              
              if (k<(ContentMulti[orderOpt[j]][i].size() - 1 ))
                f2 << " ; " ; 
      
            }
        }
      
      f2<<endl;
    }
  



  f2.close();
  cout<<"##"<<endl<<endl;
}

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
      cout << "ASR "<<start<<","<< arrive<<endl;
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
      cout << "ASC "<<start<<","<< arrive<<endl;
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

void ALattice::optimized_vec_col_M(string FileName, string FileName2)
{
  ContentMultiOrd = ContentMulti;
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
      cout << "ASC "<<start<<","<< arrive<<endl;
      //    orderOpt[start-1]=arrive-1;
      orderOpt[start-1]=arrive-1;
      i++;
    }

  for(unsigned int i=0; i<orderOpt.size();i++) //for each column
    {
      cout << "E"<< orderOpt[i]<<endl;

    }
  cout << "## print optimized lattice ##" << endl;


  // for(unsigned int j=0; j<Content.size();j++) //for each row
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

  
  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {	 
              ContentMultiOrd[j][i] =  ContentMulti[j][orderOpt[i]];
          if (i!=0) cout << " | ";
          for(unsigned int k=0; k<ContentMulti[j][orderOpt[i]].size();k++) //for each site
            {
              if(!ContentMulti[j][orderOpt[i]][k].getSign())
                cout << "-";
              else
                cout << " ";
              
              cout << ContentMulti[j][orderOpt[i]][k].getLit();
              if (k<(ContentMulti[j][orderOpt[i]].size() - 1 ))
                cout << " ; " ; 
                
            }
        }
      cout<<endl;
    }
  cout<<endl<<endl;
  

  //string FileName2=FileName + "optimized";
  cout<< FileName2;
  fstream f2;
  f2.open(FileName2.c_str(), ios::out);

  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {	 
          if (i!=0) f2 << " | ";
          for(unsigned int k=0; k<ContentMulti[j][orderOpt[i]].size();k++) //for each site
            {
              if (i!=0) f2 << " | ";
              if(!ContentMulti[j][orderOpt[i]][k].getSign())
                f2 << "-";
              else
                f2 << " ";
              
              f2 << ContentMulti[j][orderOpt[i]][k].getLit();
              if (k<(ContentMulti[j][orderOpt[i]].size() - 1 ))
                f2 << " ; " ; 
                
            }
        }
      f2<<endl;
    }

  f2.close();
  cout<<"##"<<endl<<endl;
}




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

void ALattice::print_data_multi(string FileName)
{
  fstream f;
  f.open(FileName.c_str(), ios::out);
  bool debug=false;
  if (!debug)
    {
      f << "param m:=" << GetColNum() << ";" <<endl;
      f << "param n:=" << GetRowNum()<< ";" <<endl;
      f << "param q:=" << OptVecVARm.size()<< ";" <<endl <<endl;
      f << "param a:=" <<  endl;

      for(unsigned int i=0; i<OptVecVARm.size(); i++)
        {
          f << "#"<<OptVecVARm[i] << endl << "[*,*,"<<i+1<< "]:         ";
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
                  //       cout << "QQQQ" <<  OptVecVARm[i] <<" " << endl;
                  f << FindOptPosM(OptVecVARm[i],j,k) <<" ";
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

bool ALattice::FindOptPosM(string lit,int r, int c)
{
  string varIN;
  int app=0;
  // cout << "A" << varIN << endl;
  //  cout << "B" << varIN <<endl;
  // !ContentMulti[r][c].getSign()
  for(unsigned int k=0; k<ContentMulti[r][c].size();k++) //for each site
    {
      if(!ContentMulti[r][c][k].getSign())
        {
          varIN="-" + std::to_string(ContentMulti[r][c][k].getLit());
        }
      else
        {
          varIN=std::to_string(ContentMulti[r][c][k].getLit());
        }
      // cout <<"  EE" << varIN <<  " " << lit << endl;
      if (varIN==lit)
        app=1;
   
    }
  return app;
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


void ALattice::OptFindVarM()
{
  //  vector<string> OptVecVar;
  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {
      
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {
          for(unsigned int k=0; k<ContentMulti[j][i].size();k++) //for each site
            {
              string appStr;
              if(!ContentMulti[j][i][k].getSign())
                {
                  appStr="-" + std::to_string(ContentMulti[j][i][k].getLit());
                }
              else
                appStr= std::to_string(ContentMulti[j][i][k].getLit());

              //         cout << appStr << endl;
              std::vector<string>::iterator pos;
              pos = find (OptVecVARm.begin(), OptVecVARm.end(), appStr);
  
              if (pos == OptVecVARm.end())
                {
                  OptVecVARm.push_back(appStr);
                  //   cout << appStr<<"q";
                  //cout << OptVecVar[OptVecVar.size()-1]<<endl;
                }
              //          else
              //  std::cout << "Element not found in myvector\n";
            }
          //      cout << endl;
        }
      // return OptVecVar;
    }
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


void ALattice::PrintLatticeDisOpt()
{
  for(unsigned int j=0; j<ContentDisOpt.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentDisOpt[j].size();i++) //for each column
        {
	 
          if (i!=0) cout << " | ";
          if(!ContentDisOpt[j][i][0].getSign())
            cout << "-";
          else
            cout << " ";
          cout << ContentDisOpt[j][i][0].getLit();
        }
      cout<<endl;
    }
  cout<<endl<<endl;
}


void ALattice::PrintLatticeOpt()
{
  for(unsigned int j=0; j<ContentOpt.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentOpt[j].size();i++) //for each column
        {
	 
          if (i!=0) cout << " | ";
          if(!ContentOpt[j][i][0].getSign())
            cout << "-";
          else
            cout << " ";
          cout << ContentOpt[j][i][0].getLit();
        }
      cout<<endl;
    }
  cout<<endl<<endl;
}

void ALattice::PrintLatticeOpt_M()
{
  for(unsigned int j=0; j<ContentOpt.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentOpt[j].size();i++) //for each column
        {
	 
          if (i!=0) cout << " | ";
          if(!ContentOpt[j][i][0].getSign())
            cout << "-";
          else
            cout << " ";
          cout << ContentOpt[j][i][0].getLit();
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
          for(unsigned int k=0; k<ContentMulti[j][i].size();k++) //for each site
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
  //  outputfile<<endl<<endl; for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
  for(unsigned int j=0; j<ContentMulti.size();j++) //for each row
    {    
      for(unsigned int i=0; i<ContentMulti[j].size();i++) //for each column
        {	 
          if (i!=0) outputfile << " | ";
          for(unsigned int k=0; k <ContentMulti[j][i].size();k++) //for each site
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
      if( j<ContentOpt.size()-1)
        outputfile<<endl;
    }
  outputfile.close();
    
  //   cout<<endl<<endl;
 
  //   for(unsigned int j=0; j<ContentOpt.size();j++) //for each row
  //     {
  //       for(unsigned int i=0; i<ContentOpt[j].size();i++) //for each column
  //         {
	 
  //           if (i!=0) outputfile << " | ";
  //           if(!ContentOpt[j][i][0].getSign())
  //             outputfile << "-";
  //           else
  //             outputfile << " ";
  //           outputfile << ContentOpt[j][i][0].getLit();
  //         }
  //       outputfile<<endl;
  //     }
  //   outputfile.close();
}

 
void ALattice::Print2File_multi_opt(string FileName)
{
  fstream outputfile;
  outputfile.open(FileName.c_str() , ios::out); // read dimension of .i e .o
  //  outputfile<<endl<<endl;
  for(unsigned int j=0; j<ContentOpt.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentOpt[j].size();i++) //for each column
        {
	 
          if (i!=0) outputfile << " | ";
          if(!ContentOpt[j][i][0].getSign())
            outputfile << "-";
          else
            outputfile << " ";
          outputfile << ContentOpt[j][i][0].getLit();
        }
      outputfile<<endl;
    }
  outputfile.close();
}

void ALattice::Print2File_multi_opt_M(string FileName)
{
  fstream outputfile; 
  outputfile.open(FileName.c_str() , ios::out); // read dimension of .i e .o
  //  outputfile<<endl<<endl;
  for(unsigned int j=0; j<ContentOpt.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentOpt[j].size();i++) //for each column
        {
	 
          if (i!=0) outputfile << " | ";
          if(!ContentOpt[j][i][0].getSign())
            outputfile << "-";
          else
            outputfile << " ";
          outputfile << ContentOpt[j][i][0].getLit();
        }
      outputfile<<endl;
    }
  outputfile.close();
}

void ALattice::Print2File_multi_dis_opt(string FileName)
{
  fstream outputfile;
  outputfile.open(FileName.c_str() , ios::out); // read dimension of .i e .o
  //  outputfile<<endl<<endl;
  for(unsigned int j=0; j<ContentDisOpt.size();j++) //for each row
    {
      for(unsigned int i=0; i<ContentDisOpt[j].size();i++) //for each column
        {
	 
          if (i!=0) outputfile << " | ";
          if(!ContentDisOpt[j][i][0].getSign())
            outputfile << "-";
          else
            outputfile << " ";
          outputfile << ContentDisOpt[j][i][0].getLit();
        }
      outputfile<<endl;
    }
  outputfile.close();
} 

void ALattice::setMulti(string s)
{
  MultiUnary=s;
}
