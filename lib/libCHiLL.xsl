<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:import href="./libXev.xsl" />

	<xsl:template match="SgFortranDo" mode="chill_unroll">
		<xsl:comment>
			libCHiLL.xsl chill_unroll
		</xsl:comment>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates />
		</xsl:copy>

	</xsl:template>

	<xsl:template match="SgFortranDo" mode="chill_unroll_jam">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>



</xsl:stylesheet>