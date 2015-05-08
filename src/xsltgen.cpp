/**
 * \license This project is released under the BSD 2-clause license
 *
 * Copyright (C) 2010-2015 Hiroyuki TAKIZAWA. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <picojson.h>
#include "xsltgen.hpp"

std::vector<picojson::value::object::const_iterator> caller;

std::string GetTarget(picojson::value::object::const_iterator it)
{

  std::string target="*";
  picojson::value::object::const_iterator i;

  if( it->second.is<picojson::object>() ){
    const picojson::value::object& obj =  it->second.get<picojson::object>();
    for ( i = obj.begin(); i != obj.end(); ++i) {
      if(i->first == "target")
        target =i->second.to_str();
    }
  }
  /*
  else if( it->second.is<picojson::array>() ){
    const picojson::value::array& arr =  it->second.get<picojson::array>();
    for ( j = arr.begin(); j != arr.end(); ++j) {
      if(j->is<picojson::object>()){
        const picojson::value::object& item =  j->get<picojson::object>();
        for ( i = item.begin(); i != item.end(); ++i) {
          if(i->first == "target")
            target =i->second.to_str();
        }
      }
    }
  }
  */
  return target;
}




int WriteApplyTemplates(const picojson::value::object& dir)
{
  picojson::value::object::const_iterator i;
  picojson::value::array ::const_iterator j;
  std::vector<picojson::value::object::const_iterator> rules;
  //int nrules = 0 ,curr = 0;
  std::string str_before;
  std::string str_after;
  std::string str_replace;

  for(i=dir.begin();i!=dir.end();++i){
    if(i->second.is<picojson::object>())
      rules.push_back(i);
    else if(i->first == "rules" && i->second.is<picojson::array>()) {
      const picojson::value::array& arr = (*i).second.get<picojson::array>();
      for(j=arr.begin();j!=arr.end();++j){
        if(j->is<picojson::object>()){
          const picojson::value::object& obj = j->get<picojson::object>();
          picojson::value::object::const_iterator k;
          for(k=obj.begin();k!=obj.end();++k)
            rules.push_back(k);
        }
      }
    }
    else if(i->first == "insert-before" && i->second.is<std::string>()) {
      str_before = i->second.get<std::string>().c_str();
    }
    else if(i->first == "insert-after" && i->second.is<std::string>()) {
      str_after = i->second.get<std::string>().c_str();
    }
    else if(i->first == "replace" && i->second.is<std::string>()) {
      str_replace = i->second.get<std::string>().c_str();
    }
  }
  if(str_before.size()>0)
    std::cout << str_before << std::endl;

  if(str_replace.size()>0 )
    std::cout << str_replace << std::endl;
  else {
    for(size_t k(0);k<rules.size();++k){
      if( k+1 != rules.size() )
        std::cout << "        <xsl:variable name=\"step" << k << "\">\n";

      std::cout << "        <xsl:apply-templates mode=\"call_"
                << rules[k]->first << caller.size() << "\"";
      if(k==0)
        std::cout << " select=\".\" ";
      else
      std::cout << " select=\"exslt:node-set($step" << k-1 << ")\" ";
      std::cout << "/>\n";

      if( k+1 != rules.size() )
        std::cout << "        </xsl:variable>\n";
      caller.push_back(rules[k]);
    }
  }
  if(str_replace.size() == 0 && rules.size()==0){
    /*just copy all XML elements and attributes*/
    for(int m(12);m<16;++m)
      std::cout << xslttmpl[m];
  }
  if(str_after.size()>0)
    std::cout << str_after << std::endl;
  return rules.size();
}

void WriteCaller(void)
{
  picojson::value::object::const_iterator j;
  for(size_t i(0);i<caller.size();++i){
    std::cout << "  <xsl:template match=\"*\"";
    std::cout << " mode=\"call_" << caller[i]->first << i << "\">\n";
    std::cout << "    <xsl:apply-templates select=\".\" mode=\"" << caller[i]->first << "\">\n";
    if(caller[i]->second.is<picojson::object>()){
      const picojson::object& rule =caller[i]->second.get<picojson::object>();
      for(j=rule.begin();j!=rule.end();++j){
        std::cout << "        ";
        std::cout << "<xsl:with-param name=\"" << (*j).first << "\" ";
        if((*j).second.is<picojson::object>()){
          std::cerr << "Syntax error" << std::endl;
          abort();
        }
        else if((*j).second.is<std::string>())
          std::cout << "select=\"\'" << (*j).second.to_str() << "\'\" />\n";
        else
          std::cout << "select=\"" << (*j).second.to_str() << "\" />\n";
      }
    }
    std::cout << "    </xsl:apply-templates>\n";
    std::cout << "  </xsl:template>\n";
  }
}

void WriteXslt(const picojson::value::object& root)
{
  std::cout << xsltheader;
  std::map<std::string,std::vector<picojson::value::object::const_iterator>* > targets;

  /* for each child of the root node (== for each directive definition) */
  picojson::value::object::const_iterator i;
  for ( i = root.begin(); i != root.end(); ++i) {
    std::string target = GetTarget(i);      // get the target information if any
    if(targets.find(target)==targets.end()) // the target is not registered yet?
      targets[target] = new std::vector<picojson::value::object::const_iterator>();
    targets[target]->push_back(i); // each target has a list of directives
  }

  std::map<std::string,std::vector<picojson::value::object::const_iterator>* >::iterator j;
  /* for each target */
  for(j=targets.begin();j!=targets.end();++j){
    std::cout << xslttmpl[0];
    std::cout << j->first;
    std::cout << xslttmpl[1];
    std::cout << xslttmpl[2];
    std::vector<picojson::value::object::const_iterator>* dirs =j->second;
    std::vector<picojson::value::object::const_iterator>::iterator k;
    /* for each directive */
    for(k=dirs->begin();k!=dirs->end();++k){
      if((*k)->second.is<picojson::object>()){
        if((*k)->first != "*" ){
          std::cout << xslttmpl[3];
          std::cout << (*k)->first;
          std::cout << xslttmpl[4];
        }
        else {
	  std::cout << " <xsl:apply-template mode=\"xevFindDirective\"/>\n";
          // directive name is *. don't check if a directive exists.
	  // std::cout << "      <xsl:when test=\"true()\">\n";
        }
	for(int m(5);m<11;++m)
	  std::cout << xslttmpl[m];

	// rules associated with the directive
	WriteApplyTemplates((*k)->second.get<picojson::object>());
	std::cout << xslttmpl[11];
      }
    }
  }

  WriteCaller();
  std::cout << xsltfooter;
}

int main(int argc,char** argv)
{
  picojson::value v;

  // parse the input
  std::cin >> v;
  std::string err = picojson::get_last_error();
  if (! err.empty()) {
    std::cerr << err << std::endl;
    exit(1);
  }

  // check if the type of the value is "object"
  if (! v.is<picojson::object>()) {
    std::cerr << "The root of the JSON file is not an object" << std::endl;
    exit(2);
  }

  // obtain a const reference to the map, and print the contents
  const picojson::value::object& root = v.get<picojson::object>();
  WriteXslt(root);
  return 0;
}
