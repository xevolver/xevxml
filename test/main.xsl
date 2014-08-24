<?xml version="1.0" encoding="shift_jis"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:output method="xml" encoding="Shift_JIS"/>

<!-- default -->
  <xsl:template match="/">
    <xsl:apply-templates/>
  </xsl:template>
  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="node()|@*"/>
    </xsl:copy>
  </xsl:template>

<!-- including custom rules -->
  <xsl:include href="1to2.xsl"/>
</xsl:stylesheet>
