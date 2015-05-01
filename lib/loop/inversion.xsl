<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="SgWhileStmt" mode="xevLoopInversion">
		<SgIfStmt>
			<xsl:copy-of select="@*" />
			<xsl:copy-of select="SgExprStatement" />
			<SgBasicBlock>
				<SgFortranDo style="0" end="1" slabel="">
					<SgNullExpression />
					<SgNullExpression />
					<SgNullExpression />
					<SgBasicBlock>
						<xsl:copy-of select="SgBasicBlock/*" />
						<SgIfStmt end="0" then="0">
							<xsl:copy-of select="SgExprStatement" />
							<SgBasicBlock>
								<SgContinueStmt />
							</SgBasicBlock>
							<SgBasicBlock />
						</SgIfStmt>
						<SgBreakStmt />
					</SgBasicBlock>
				</SgFortranDo>
			</SgBasicBlock>
			<SgBasicBlock />
			<xsl:copy-of select="PreprocessingInfo" />
		</SgIfStmt>
	</xsl:template>

</xsl:stylesheet>
