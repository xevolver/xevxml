<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevLoopUnswitching_CopyStatement">
		<xsl:copy-of select="." />
	</xsl:template>

	<xsl:template match="SgIfStmt" mode="xevLoopUnswitching_CopyStatement">
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="xevLoopUnswitching">
		<SgIfStmt end="1" then="1">
			<SgExprStatement>
				<xsl:copy-of select="SgBasicBlock/SgIfStmt/SgExprStatement/*" />
			</SgExprStatement>
			<SgBasicBlock>
				<SgFortranDo style="0" end="1" slabel="">
					<xsl:copy-of select="./*[1]" />
					<xsl:copy-of select="./*[2]" />
					<xsl:copy-of select="./*[3]" />
					<SgBasicBlock>
						<xsl:apply-templates select="SgBasicBlock/*"
							mode="xevLoopUnswitching_CopyStatement" />
						<xsl:copy-of select="./SgBasicBlock/SgIfStmt/SgBasicBlock/*" />
					</SgBasicBlock>
				</SgFortranDo>
			</SgBasicBlock>
			<SgBasicBlock>
				<SgFortranDo style="0" end="1" slabel="">
					<SgAssignOp>
						<SgVarRefExp name="i" lvalue="1" />
						<SgIntVal value="1" string="1" />
					</SgAssignOp>
					<SgSubtractOp>
						<SgVarRefExp name="n" />
						<SgIntVal value="1" string="1" />
					</SgSubtractOp>
					<SgNullExpression />
					<SgBasicBlock>
						<SgExprStatement>
							<SgAssignOp>
								<SgPntrArrRefExp lvalue="1">
									<SgVarRefExp name="A" />
									<SgExprListExp>
										<SgVarRefExp name="i" />
									</SgExprListExp>
								</SgPntrArrRefExp>
								<SgAddOp>
									<SgPntrArrRefExp>
										<SgVarRefExp name="A" />
										<SgExprListExp>
											<SgVarRefExp name="i" />
										</SgExprListExp>
									</SgPntrArrRefExp>
									<SgPntrArrRefExp>
										<SgVarRefExp name="B" />
										<SgExprListExp>
											<SgVarRefExp name="i" />
										</SgExprListExp>
									</SgPntrArrRefExp>
								</SgAddOp>
							</SgAssignOp>
						</SgExprStatement>
					</SgBasicBlock>
				</SgFortranDo>
			</SgBasicBlock>
			<xsl:copy-of select="PreprocessingInfo" />
		</SgIfStmt>
	</xsl:template>

</xsl:stylesheet>
