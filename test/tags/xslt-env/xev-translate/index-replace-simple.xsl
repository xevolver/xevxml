<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
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

	<xsl:template match="SgVarRefExp">
		<xsl:choose>
			<xsl:when
				test="ancestor::SgExprStatement/preceding-sibling::SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='index']/CLAUSE[@name='replace']/LI[1]/@value=@name">
				<xsl:copy>
					<xsl:attribute name="name">
					<xsl:value-of
						select="ancestor::SgExprStatement/preceding-sibling::SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='index']/CLAUSE[@name='replace']/LI[2]/@value" />
					</xsl:attribute>
					<xsl:apply-templates></xsl:apply-templates>
				</xsl:copy>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*"></xsl:copy-of>
					<xsl:apply-templates></xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

</xsl:stylesheet>
