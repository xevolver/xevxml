<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="SgFortranDo" mode="loop_interchange_rule">
		<xsl:element name="SgFortranDo">
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" />
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/*[1]" />
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/*[2]" />
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/*[3]" />
			<xsl:element name="SgBasicBlock">
				<xsl:copy-of select="SgBasicBlock/@*" />
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:copy-of select="./*[1]" />
					<xsl:copy-of select="./*[2]" />
					<xsl:copy-of select="./*[3]" />
					<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock" />
				</xsl:copy>
			</xsl:element>
		</xsl:element>
	</xsl:template>

	<xsl:template match="*" mode="loop_interchange">
		<xsl:param name="loopName" />
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $loopName">
				<xsl:apply-templates select="." mode="loop_interchange_rule" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="loop_interchange">
						<xsl:with-param name="loopName" select="$loopName" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="SgPragmaDeclaration" mode="loop_interchange">
	</xsl:template>
	<xsl:template match="PreprocessingInfo" mode="loop_interchange">
	</xsl:template>


	<xsl:template match="*" mode="loop_interchange_split">
		<xsl:element name="SgFortranDo">
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" />
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/*[1]" />
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/*[2]" />
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/*[3]" />
			<xsl:element name="SgBasicBlock">
				<xsl:copy-of select="SgBasicBlock/@*" />
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:copy-of select="./*[1]" />
					<xsl:copy-of select="./*[2]" />
					<xsl:copy-of select="./*[3]" />
					<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgBasicBlock" />
				</xsl:copy>
			</xsl:element>
		</xsl:element>
	</xsl:template>


</xsl:stylesheet>
