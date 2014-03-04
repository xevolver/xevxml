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
	</xsl:template>

	<!-- remove SgPragmaDeclaration -->
	<xsl:template match="SgPragmaDeclaration">
		<xsl:comment>
			SgPragmaDeclaration
		</xsl:comment>
	</xsl:template>

	<xsl:template match="SgExprStatement">
		<xsl:choose>
			<xsl:when
				test="preceding-sibling::SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='conditional']/CLAUSE[@name='if']">
				<xsl:text>if (</xsl:text>
				<xsl:value-of
					select="preceding-sibling::SgPragmaDeclaration/SgPragma/DIRECTIVE[@name='conditional']/CLAUSE/LI/@value" />
				<xsl:text> ) then</xsl:text>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates />
				</xsl:copy>
				<xsl:text>end if</xsl:text>
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
