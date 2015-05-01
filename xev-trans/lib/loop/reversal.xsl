<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="SgFortranDo" mode="xevLoopReversal">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<SgAssignOp>
				<xsl:apply-templates select="./SgAssignOp/SgVarRefExp" />
				<xsl:apply-templates select="./*[2]" />
			</SgAssignOp>
			<xsl:apply-templates select="./SgAssignOp/*[2]" />

			<xsl:apply-templates select="./*[3]"
				mode="xevLoopReversal_Stride" />
			<xsl:apply-templates select="SgBasicBlock" />
			<xsl:apply-templates select="PreprocessingInfo" />
		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgNullExpression" mode="xevLoopReversal_Stride">
		<SgMinusOp>
			<SgIntVal value="1" string="1" />
		</SgMinusOp>
	</xsl:template>

	<xsl:template match="*" mode="xevLoopReversal_Stride">
		<SgMinusOp>
			<xsl:apply-templates select="." />
		</SgMinusOp>
	</xsl:template>

</xsl:stylesheet>
