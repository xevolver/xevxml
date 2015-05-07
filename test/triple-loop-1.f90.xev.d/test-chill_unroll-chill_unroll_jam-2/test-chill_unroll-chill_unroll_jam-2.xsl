<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libCHiLL.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="*" mode="xevInitHook">
		<xsl:apply-templates select="." mode="xevFindDirective">
			<xsl:with-param name="directiveName" select="'xev loop_tag'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevMoveHook">
		<xsl:apply-templates select="." mode="xevGoToHere" />
	</xsl:template>

	<xsl:template match="*" mode="xevTransformationHook">

		<xsl:variable name="step1_result">
			<xsl:apply-templates select="." mode="step1" />
		</xsl:variable>

		<xsl:variable name="step2_result">
			<xsl:apply-templates select="exslt:node-set($step1_result)"
				mode="step2" />
		</xsl:variable>

		<xsl:apply-templates select="exslt:node-set($step2_result)" />

	</xsl:template>

	<xsl:template match="*" mode="step1">
		<xsl:apply-templates select="." mode="chillUnroll">
			<xsl:with-param name="factor" select="2" />
			<xsl:with-param name="loopName" select="'i'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="step2">
		<xsl:apply-templates select="." mode="chillUnrollJam">
			<xsl:with-param name="factor" select="4" />
			<xsl:with-param name="loopName" select="'k'" />
		</xsl:apply-templates>
	</xsl:template>

</xsl:stylesheet>
	