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
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="./*[1]"/>
      <xsl:apply-templates select="./*[2]"/>
      <xsl:apply-templates select="./*[3]"/>
      <xsl:element name="SgBasicBlock">
	<xsl:copy-of select="./*[4]"/>
	<xsl:element name="SgExprStatement">
	  <xsl:element name="SgPlusPlusOp">
	    <xsl:copy-of select="./*[3]/SgVarRefExp[1]"/>
	  </xsl:element>
	</xsl:element>
	<xsl:copy-of select="./*[4]"/>
      </xsl:element>
    </xsl:copy>
  </xsl:template>


  <xsl:template match="SgForStatement/SgPlusPlusOp">
    <xsl:variable name="PlusPlusOp">
      <xsl:element name="SgPlusAssignOp">
	<xsl:copy-of select="@*"/>
	<xsl:copy-of select="./SgVarRefExp[1]"/>
	<SgIntVal value="1"/>
      </xsl:element>
    </xsl:variable>
    <!--
    <xsl:copy-of select="./$PlusPlusOp"/>
    -->
    <!--<xsl:apply-templates select="./$PlusPlusOp"/>-->
  </xsl:template>

  <xsl:template match="SgForStatement/SgPlusAssignOp/SgIntVal">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>
