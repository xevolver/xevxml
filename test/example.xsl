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
  <!-- remove return stmts -->
  <xsl:template match="SgReturnStmt">
    <!--
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates/>
      </xsl:copy>
    -->
    <SgReturnStmt>
      <SgIntVal value="10"/>
    </SgReturnStmt>
  </xsl:template>
</xsl:stylesheet>
