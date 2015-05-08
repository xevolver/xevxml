<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevLoopTileUndo">
		<SgFortranDo style="0" end="1" slabel="">
			<SgAssignOp>
				<xsl:copy-of
					select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp" />
				<xsl:copy-of select="SgAssignOp/SgIntVal" />
			</SgAssignOp>
			<xsl:copy-of
				select="SgAddOp/SgMultiplyOp/SgFunctionCallExp/SgExprListExp/SgDivideOp/SgFunctionCallExp/SgExprListExp/SgSubtractOp/SgVarRefExp" />
			<SgNullExpression />
			<SgBasicBlock>
				<SgFortranDo style="0" end="1" slabel="">
					<SgAssignOp>
						<xsl:copy-of
							select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp" />
						<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp/SgIntVal" />
					</SgAssignOp>
					<xsl:copy-of
						select="SgBasicBlock/SgFortranDo/SgAddOp/SgMultiplyOp/SgFunctionCallExp/SgExprListExp/SgDivideOp/SgFunctionCallExp/SgExprListExp/SgSubtractOp/SgVarRefExp" />
					<SgNullExpression />
					<xsl:apply-templates
						select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgBasicBlock/SgFortranDo/SgBasicBlock" />
				</SgFortranDo>
			</SgBasicBlock>
			<xsl:copy-of select="PreprocessingInfo" />
		</SgFortranDo>
	</xsl:template>

</xsl:stylesheet>
