<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="*" mode="xevInitHook">
		<xsl:apply-templates select="." mode="xevFindDirective">
			<xsl:with-param name="directiveName" select="'xev loop_tag'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevMoveHook">
		<xsl:apply-templates select="." mode="xevGoToVar">
			<xsl:with-param name="varName" select="'B'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xevTransformationHook">
		<xsl:element name="SgVarRefExp">
			<xsl:attribute name="name">
					  <xsl:value-of select="'C'" />
					</xsl:attribute>
		</xsl:element>
	</xsl:template>

</xsl:stylesheet>
	