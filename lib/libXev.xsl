<?xml version="1.0" encoding="UTF-8"?>

<!-- Xev standard translation library -->

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<!-- loop/* -->
	<xsl:import href="loop/unroll.xsl" />

	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="/">
		<xsl:apply-templates />
	</xsl:template>

	<xsl:template match="*">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>




	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo">
		<xsl:comment>
			PreprocessingInfo
		</xsl:comment>
		<!-- <xsl:apply-templates /> -->
	</xsl:template>

	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration">
		<xsl:comment>
			SgPragmaDeclaration
		</xsl:comment>
	</xsl:template>

</xsl:stylesheet>
