<?xml version="1.0" encoding="shift_jis"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" encoding="Shift_JIS"/>

  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>


  <xsl:template match="*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="SgForStatement">
    <xsl:if test=".//*=SgForStatement">
    startLoopNest(); /* inserted */
    </xsl:if>
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
    </xsl:copy>
    <xsl:if test=".//*=SgForStatement">
    endLoopNest(); /* inserted */
    </xsl:if>
  </xsl:template>
</xsl:stylesheet>
