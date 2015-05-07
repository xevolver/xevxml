<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:import href="../../../lib/libXev.xsl" />

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
		<xsl:apply-templates select="." mode="xevLoopTile">
			<xsl:with-param name="loopName" select="'i'" />
			<xsl:with-param name="start" select="'1'" />
			<xsl:with-param name="end" select="'n - 1'" />
			<xsl:with-param name="size" select="'4'" />
		</xsl:apply-templates>
	</xsl:template>

</xsl:stylesheet>
	