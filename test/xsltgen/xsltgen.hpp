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
#ifndef ___XSLTGEN_HEAD___
#define ___XSLTGEN_HEAD___

const char xsltheader[]=
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "  <xsl:stylesheet version=\"1.0\"\n"
  "    xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" xmlns:exslt=\"http://exslt.org/common\">\n"
  "  <xsl:import href=\"../libCHiLL.xsl\" />\n"
  "  <xsl:output method=\"xml\" encoding=\"UTF-8\" />\n";

const char xsltfooter[]="\n</xsl:stylesheet>\n";

const char* xslttmpl[] =
  {
    /* 0 */                               /* 1 */
    "  <xsl:template match=\"",/*tagret*/ "\">\n",
    /* 2 */
    "    <xsl:choose>\n",
    /* 3 */                                                                          /* 4 */
    "      <xsl:when test=\"preceding-sibling::*[1]/SgPragma/@pragma = '",/*pragma*/ "'\">\n",
    /* call rule_callers of a directive */
    /* 5 */
    "      </xsl:when>\n",
    /* 6 */
    "      <xsl:otherwise>\n",
    /* 7 - 13 */
    "        <xsl:copy>\n",
    "          <xsl:copy-of select=\"@*\" />\n",
    "          <xsl:apply-templates />\n",
    "        </xsl:copy>\n",
    "      </xsl:otherwise>\n",
    "    </xsl:choose>\n",
    "  </xsl:template>\n"
  };

const char* xlstcaller[] =
  {
    "  <xsl:template match=\"*\" mode=\"call_%s\">\n",
    "    <xsl:apply-templates select=\".\" mode=\"%s\">\n",
    "      <xsl:with-param name=\"%s\" select=\"%s\" />\n",
    "    </xsl:apply-templates>\n",
    "  </xsl:template>\n"
  };


#endif
