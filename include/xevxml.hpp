/**
 * \license This project is released under the BSD 2-clause license
 *
 * Copyright (C) 2010-2013 Hiroyuki TAKIZAWA. All rights reserved.
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
#ifndef ___XEVXML_H___
#define ___XEVXML_H___
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <rose.h>

//#define XEV_USE_ROSEHPCT // experimental

#ifdef XEV_USE_ROSEHPCT
#include <rosehpct/rosehpct.hh>
#endif

namespace XevXML {

  class XevConversionHelper ; // see defined later

  extern void XevInitialize(void);
  extern void XevFinalize(void);
  extern bool XevConvertXmlToAst(std::stringstream& str, SgProject** prj, XevConversionHelper* help = NULL);
  extern bool XevConvertAstToXml(std::stringstream& str, SgProject** prj, XevConversionHelper* help = NULL);


  // --- for custom transformation between ROSE AST and XML AST ---
  class XevConversionHelper {
    bool addressFlag_;
    bool rosehpctFlag_;

    int level_;
#ifdef XEV_USE_ROSEHPCT
    RoseHPCT::ProgramTreeList_t profiles_;
#endif
  public:
    /* Ctors and Dtor */
    // -------------------------------------------------------------------------
   XevConversionHelper& operator=(const XevConversionHelper& h) {
      addressFlag_=h.getAddressFlag();
      level_ = h.getLevel();
      return *this;
    }
    XevConversionHelper(const XevConversionHelper& h){
      rosehpctFlag_ = false;
      *this = h;
    }
    XevConversionHelper() {addressFlag_=false;rosehpctFlag_=false;level_=0;}
    ~XevConversionHelper() {}
    // -------------------------------------------------------------------------



    /* user-defined callback functions */
    // -------------------------------------------------------------------------
    /*
      XML syntax and the positision where each function is called.

          <E    A="..."  >   ...  </E>
        ^     ^        ^   ^    ^     ^
       (1)   (2)      (3) (4)  (5)   (6)
    */
    virtual void beforeXmlElement         (SgNode*&){} //(1)
    virtual void afterXmlElement          (SgNode*&){} //(4)
    virtual void beforeXmlAttribute       (SgNode*&){} //(2)
    virtual void afterXmlAttribute        (SgNode*&){} //(3)
    virtual void beforeXmlClosingElement  (SgNode*&){} //(5)
    virtual void afterXmlClosingElement   (SgNode*&){} //(6)
    // -------------------------------------------------------------------------

    // set true to write the memory address of each node (for debugging)
    bool getAddressFlag()  const{ return addressFlag_; }
    void setAddressFlag(bool f) { addressFlag_ = f; }  

    // depth of the visited node from the root node
    int  getLevel()        const{ return level_; }
    void setLevel(int l)        { level_ = l; }
  };

  // --- XML utility functions ---
  extern void XmlInitialize(void);
  extern void XmlFinalize(void);
  extern std::string XmlStr2Entity( std::string);
  extern std::string XmlEntity2Str( std::string);


}

#endif
