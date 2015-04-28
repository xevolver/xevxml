<?xml version="1.0" encoding="UTF-8"?>
  <xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">
  <xsl:import href="libCHiLL.xsl" />
  <xsl:output method="xml" encoding="UTF-8" />
  <xsl:template match="SgFortranDo">
    <xsl:choose>
      <xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag1'">
        <xsl:apply-templates mode="call_chill_unroll0" select="." />
      </xsl:when>
      <xsl:otherwise>
        <xsl:copy>
          <xsl:copy-of select="@*" />
          <xsl:apply-templates />
        </xsl:copy>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="SgIfStmt">
    <xsl:choose>
      <xsl:when test="preceding-sibling::*[1]/SgPragma/@pragma = 'xev loop_tag2'">
return 0;
      </xsl:when>
      <xsl:otherwise>
        <xsl:copy>
          <xsl:copy-of select="@*" />
          <xsl:apply-templates />
        </xsl:copy>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  <xsl:template match="*" mode="call_chill_unroll0">
    <xsl:apply-templates select="." mode="chill_unroll">
        <xsl:with-param name="factor" select="2" />
        <xsl:with-param name="loopName" select="'i'" />
    </xsl:apply-templates>
  </xsl:template>

</xsl:stylesheet>
