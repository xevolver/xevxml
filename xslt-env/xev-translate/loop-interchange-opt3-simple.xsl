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
				test="preceding::SgPragma/DIRECTIVE[@name='loop']/CLAUSE/@name='interchange-opt3'">
				DO I=2,NNODE
				IF (DNPOLD(I,L).GE.SMALL) then
				DO J=2,NNODE
				if (DNPOLD(J,M).GE.SMALL) then
				<xsl:copy-of select="SgBasicBlock/SgIfStmt/SgBasicBlock/SgFortranDo/SgBasicBlock/SgIfStmt/SgBasicBlock">
				</xsl:copy-of>
				end if
				END DO
				end if
				END DO
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
