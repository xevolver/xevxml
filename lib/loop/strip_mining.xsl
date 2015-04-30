<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="SgFortranDo" mode="xevLoopStripMining">
		<xsl:param name="size" />
		<SgFortranDo style="0" end="1" slabel="">
			<SgAssignOp>
				<xsl:element name="SgVarRefExp">
					<xsl:attribute name="name">
					  <xsl:value-of select="SgAssignOp/SgVarRefExp/@name" />
					  <xsl:text>_sm</xsl:text>
					</xsl:attribute>
				</xsl:element>
				<xsl:copy-of select="SgAssignOp/*[2]" />
			</SgAssignOp>
			<xsl:copy-of select="./*[2]" />
			<xsl:element name="SgIntVal">
				<xsl:attribute name="value">
					  <xsl:value-of select="$size" />
				</xsl:attribute>
				<xsl:attribute name="string">
					  <xsl:value-of select="$size" />
				</xsl:attribute>
			</xsl:element>
			<SgBasicBlock>
				<SgFortranDo style="0" end="1" slabel="">
					<SgAssignOp>
						<xsl:copy-of select="SgAssignOp/SgVarRefExp" />
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
					       <xsl:value-of select="SgAssignOp/SgVarRefExp/@name" />
					  <xsl:text>_sm</xsl:text>
					</xsl:attribute>
						</xsl:element>
					</SgAssignOp>
					<SgFunctionCallExp>
						<SgFunctionRefExp name="min" kind="1" />
						<SgExprListExp>
							<xsl:copy-of select="./*[2]" />
							<SgSubtractOp>
								<SgAddOp>
									<xsl:element name="SgVarRefExp">
										<xsl:attribute name="name">
										  <xsl:value-of select="SgAssignOp/SgVarRefExp/@name" />
										  <xsl:text>_sm</xsl:text>
										</xsl:attribute>
									</xsl:element>
									<xsl:element name="SgIntVal">
										<xsl:attribute name="value">
									<xsl:value-of select="$size" />
									</xsl:attribute>
										<xsl:attribute name="string">
										<xsl:value-of select="$size" />
									</xsl:attribute>
									</xsl:element>
								</SgAddOp>
								<SgIntVal value="1" string="1" />
							</SgSubtractOp>
						</SgExprListExp>
					</SgFunctionCallExp>
					<SgNullExpression />
					<xsl:apply-templates select="SgBasicBlock" />
				</SgFortranDo>
			</SgBasicBlock>
			<xsl:copy-of select="PreprocessingInfo" />
		</SgFortranDo>
	</xsl:template>

</xsl:stylesheet>
