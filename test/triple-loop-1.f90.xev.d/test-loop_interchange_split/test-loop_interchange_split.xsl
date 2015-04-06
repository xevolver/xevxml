<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:import href="../../../lib/libXev.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />


	<xsl:template match="*">
		<xsl:apply-templates select="." mode="find_directive">
			<xsl:with-param name="directiveName" select="'xev loop_tag'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="xev_move_hook">
		<xsl:apply-templates select="." mode="move_loop">
			<xsl:with-param name="loopName" select="'j'" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="xev_transformation_hook">
		<xsl:apply-templates select="."
			mode="loop_interchange_rule" />
	</xsl:template>

</xsl:stylesheet>
	