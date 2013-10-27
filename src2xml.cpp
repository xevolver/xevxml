#include "xevxml.hpp"
#include "ast2xml.hpp"
#include "xml2ast.hpp"

#include <getopt.h>
#include <vector>
#include <string>

using namespace std;

static vector<string> cmdopt(int argc, char** argv, xevxml::Ast2XmlOpt* opt)
{
  int c;
  int digit_optind = 0;
  vector<string> args;
  
  args.push_back(argv[0]);

  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"xev:verbose", required_argument, 0,  0 },
      {"xev:address", required_argument, 0,  0 },
      {0,         0,                 0,  0 }
    };
    
    c = getopt_long(argc, argv, "",
		    long_options, &option_index);
    if (c == -1) {
      break;
    }

    switch (c) {
    case 0:
      printf("option %s", long_options[option_index].name);
      if (optarg) {
	printf(" with arg %s", optarg);
	if(option_index==0)
	  SgProject::set_verbose(atoi(optarg));
	if(option_index==1)
	  opt->address = atoi(optarg);
      }
      printf("\n");
      break;

    case '?':
      break;
      
    default:
      printf("?? getopt returned character code 0%o ??\n", c);
    }
  }
  for(int i(optind);i<argc;i++)
    args.push_back(argv[i]);
  return args;
}


int main(int argc, char** argv)
{
  int             fd=0;
  stringstream    xmlString1;
  SgProject*      sageProject=0;
  SgFile*         file=0;
  xevxml::Ast2XmlOpt opt;

  fd = dup(1); 
  dup2(2,1); // printf messages are written to stderr  
  ///SgProject::set_verbose(10);
  sageProject = frontend(cmdopt(argc,argv,&opt));
  dup2(fd,1); // printf messages are written to stdout  
  file = &sageProject->get_file(sageProject->numberOfFiles()-1);

  xevxml::XmlInitialize();
  /*
  xevxml::Ast2Xml(xmlString1,file,&opt);

  cout << xmlString1.str();
  */
  char c;
  while((c=cin.get()) != cin.eof()){
    xmlString1 << c;
  }

  xevxml::Xml2Ast(xmlString1,sageProject,"dummy.f");
  sageProject->unparse();

  xevxml::XmlFinalize();
  return 0;
}
