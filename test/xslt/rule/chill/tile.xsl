<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:import href="../libXev.xsl" />

  <xsl:template match="*" mode="chill_tile">
    <xsl:param name="loopName" />
    <xsl:param name="start" />
    <xsl:param name="end" />
    <xsl:param name="size" />
    <xsl:apply-templates select="." mode="loop_tile">
      <xsl:with-param name="loopName" select="$loopName" />
      <xsl:with-param name="start" select="$start" />
      <xsl:with-param name="end" select="$end" />
      <xsl:with-param name="size" select="$size" />
    </xsl:apply-templates>
  </xsl:template>
</xsl:stylesheet>
