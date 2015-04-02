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
	</xsl:template>

	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<xsl:when
				test="preceding::SgPragma/DIRECTIVE[@name='loop']/CLAUSE/@name='fusion'">
				<xsl:text>
				DO NK=0,NFRAC*(NNODE-1)-1</xsl:text>
				<xsl:text>				
				n = nk/(nnode-1)+1</xsl:text>
				<xsl:text>
				k = mod(nk,nnode-1)+2</xsl:text>
				<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock">
				</xsl:copy-of>
				<xsl:text>
				end do</xsl:text>
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
