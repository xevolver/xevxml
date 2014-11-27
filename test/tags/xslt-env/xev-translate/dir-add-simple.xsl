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
		<xsl:apply-templates />
	</xsl:template>

	<xsl:template match="SgPragma">
		<xsl:comment>
			SgPragma 2222 SgPragma 2222
		</xsl:comment>
		<xsl:apply-templates select="DIRECTIVE/CLAUSE" />
	</xsl:template>

	<xsl:template match="CLAUSE">
		<xsl:if test="@name='add'">
			<xsl:element name="SgPragma">
				<xsl:attribute name="pragma"><xsl:for-each
					select="LI">
			<xsl:value-of select="@value" /><xsl:text> </xsl:text>
			</xsl:for-each></xsl:attribute>
			</xsl:element>
		</xsl:if>
	</xsl:template>

</xsl:stylesheet>
