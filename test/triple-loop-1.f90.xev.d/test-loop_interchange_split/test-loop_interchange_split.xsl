<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />


	<xsl:template match="*">
		<xsl:apply-templates select="." mode="xevFindDirective">
			<xsl:with-param name="directiveName" select="'xev loop_tag'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevMoveHook">
		<xsl:apply-templates select="." mode="xevMoveLoop">
			<xsl:with-param name="loopName" select="'j'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="xevTransformationHook">
		<xsl:apply-templates select="." mode="xevLoopInterchange" />
	</xsl:template>

</xsl:stylesheet>
	