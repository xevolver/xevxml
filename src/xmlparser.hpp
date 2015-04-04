/**
 * @file     xmlparser.hpp
 * @brief    custom XML parser class
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

#ifndef ___XMLPARSER_HPP___
#define ___XMLPARSER_HPP___

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMUserDataHandler.hpp>
#include <xercesc/internal/XMLScanner.hpp>

namespace XevXml {

class XevDOMParser : public xercesc::XercesDOMParser {
  class XevDataHandler;
  friend class XevDataHandler;

private:
  XevDataHandler* handler_;

public:
  struct XmlLoc {
    void inc() {cnt++;}
    void dec() {cnt--;}

    int line;
    int col;
    int cnt;
  };

  XevDOMParser();
  ~XevDOMParser() {}

  virtual void startElement
  (
   const   xercesc::XMLElementDecl& elemDecl
   , const unsigned int            urlId
   , const XMLCh* const            elemPrefix
   , const xercesc::RefVectorOf<xercesc::XMLAttr>&   attrList
   , const XMLSize_t               attrCount
   , const bool                    isEmpty
   , const bool                    isRoot
  );

  static const XevDOMParser::XmlLoc* getXmlLoc(const xercesc::DOMNode* node);
};

}

#endif
