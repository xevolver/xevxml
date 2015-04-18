/**
 * @file     xmlutils.cpp
 * @brief    Utility functions for handling XML data
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
#include "common.hpp"
#include "xmlparser.hpp"

#include <xmlutils.hpp>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xalanc/XalanTransformer/XalanTransformer.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>

namespace xe=xercesc;
namespace xa=xalanc;
using namespace std;

namespace XevXml {

static string REPLACE( string str1, string str2, string str3 )
{
  string::size_type  Pos( str1.find( str2 ) );

  while( Pos != string::npos ){
    str1.replace( Pos, str2.length(), str3 );
    Pos = str1.find( str2, Pos + str3.length() );
  }

  return str1;
}

void XmlInitialize(void)
{
  try {
    xe::XMLPlatformUtils::Initialize();
    xa::XalanTransformer::initialize();
  }
  catch (const xe::XMLException& toCatch) {
    // Do your failure processing here
    XEV_ABORT();
  }
  return;
}

void XmlFinalize(void)
{
  xa::XalanTransformer::terminate();
  xe::XMLPlatformUtils::Terminate();
}

string XmlStr2Entity( string str )
{
  str = REPLACE( str,"&","&amp;" );
  str = REPLACE( str,"<","&lt;" );
  str = REPLACE( str,">","&gt;" );
  str = REPLACE( str,"\"","&quot;" );
  str = REPLACE( str,"\'","&apos;" );
  return str;
}

string XmlEntity2Str( string str )
{
  str = REPLACE( str,"&amp;","&" );
  str = REPLACE( str,"&lt;","<" );
  str = REPLACE( str,"&gt;",">" );
  str = REPLACE( str,"&quot;","\"" );
  str = REPLACE( str,"&apos;","\'" );
    return str;
}

bool XmlWriteToString( xe::DOMNode* node, std::stringstream& str )
{
  XMLCh tempStr[100];
  xe::XMLString::transcode("LS", tempStr, 99);
  xe::DOMImplementation *impl = xe::DOMImplementationRegistry::getDOMImplementation(tempStr);
  xe::DOMLSSerializer *ser = ((xe::DOMImplementationLS*)impl)->createLSSerializer();
  if (ser-> getDomConfig()-> canSetParameter(xe::XMLUni::fgDOMWRTFormatPrettyPrint, true)){
    ser-> getDomConfig()-> setParameter(xe::XMLUni::fgDOMWRTFormatPrettyPrint, true);
  }
  ser-> setNewLine(xe::XMLString::transcode("\r\n"));

  //XMLFormatTarget *formatTarget = new LocalFileFormatTarget( tempFilePath );
  //xe::XMLFormatTarget *formatTarget = new xe::StdOutFormatTarget();
  xe::MemBufFormatTarget *formatTarget = new xe::MemBufFormatTarget();
  xe::DOMLSOutput *output = ((xe::DOMImplementationLS*)impl)-> createLSOutput();
  output-> setByteStream(formatTarget);
  ser->write(node,output);

  char* s = (char*)formatTarget->getRawBuffer();
  str << s;
  ser->release();
  delete formatTarget;
  output->release();
  return true;
}

template <>
bool XmlGetAttributeValue( xercesc::DOMNode* node, const char* name, std::string* val)
{
  xercesc::DOMNamedNodeMap* amap = node->getAttributes();
  xercesc::DOMNode*         att  = 0;
  XMLCh* xbuf;
  char* cbuf;

  if(amap) {
    xbuf = xercesc::XMLString::transcode(name);
    att=amap->getNamedItem(xbuf);
    xercesc::XMLString::release(&xbuf);
  }
  if(att && val) {
    cbuf = xercesc::XMLString::transcode(att->getNodeValue());
    *val =  cbuf;
    xercesc::XMLString::release(&cbuf);
    return true;
  }
  return false;
}



string XmlGetNodePosition(xercesc::DOMNode* node)
{
  char* buf;
  string path;
  stringstream pos;

  const XevDOMParser::XmlLoc* loc = XevDOMParser::getXmlLoc(node);
  if(loc){
    pos << "LINE = " << loc->line << std::endl;
    pos << "COL  = " << loc->col  << std::endl;
  }

  while(node!=NULL){
    buf = xercesc::XMLString::transcode(node->getNodeName());
    path = '/' + path;
    path = buf + path;
    xercesc::XMLString::release(&buf);
    node = node->getParentNode();
  }
  path = "NODE = " + path;
  return pos.str() + path;
}

static XMLCh* attKey = 0;

class XevDOMParser::XevDataHandler : public xe::DOMUserDataHandler {
private:
  XevDOMParser* parser_;

public:
  XevDataHandler():parser_(0) {}
  virtual ~XevDataHandler(){}

  void setParser(XevDOMParser* p){
    parser_ = p;
  }

  void handle(DOMOperationType operation,
              const XMLCh* const key,
              void* data,
              const xe::DOMNode* src,
              xe::DOMNode* dst) {
    XmlLoc* loc = (XmlLoc*)data;
    switch(operation){
    case NODE_IMPORTED:
    case NODE_CLONED: loc->inc(); break;
    case NODE_DELETED:
      loc->dec();
      if(loc->cnt <= 0 ) delete loc;
      break;
    case NODE_RENAMED:
      break;
    }
  }
};
  XevDOMParser::XevDOMParser() : handler_(new XevDataHandler()){ handler_->setParser(this); }

  void XevDOMParser::startElement( const xe::XMLElementDecl &elemDecl,
                                   const unsigned int uriId, const XMLCh *const prefixName,
                                   const xe::RefVectorOf< xe::XMLAttr > &attrList,
                                   const XMLSize_t attrCount, const bool isEmpty, const bool isRoot ) {

    XercesDOMParser::startElement(elemDecl, uriId, prefixName, attrList, attrCount, isEmpty, isRoot);

    //if(!isEmpty){
    if(1){
      XmlLoc* loc = new XmlLoc();
      const xe::Locator* locator = getScanner()->getLocator();
      loc->line = locator->getLineNumber();
      loc->col  = locator->getColumnNumber();
      if(attKey==0){
        attKey = xe::XMLString::transcode("LocAttrib1");
      }
      XercesDOMParser::fCurrentNode->setUserData(attKey, loc, handler_);
      loc->inc();
    }
  }

  const XevDOMParser::XmlLoc* XevDOMParser::getXmlLoc(const xe::DOMNode* node){
    return (XevDOMParser::XmlLoc*)(node->getUserData(attKey));
  }
}
