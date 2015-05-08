<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:exslt="http://exslt.org/common">

	<xsl:template match="*" mode="xevLoopTile">
		<xsl:param name="size1" />
		<xsl:param name="size2" />

		<SgFortranDo style="0" end="1" slabel="">
			<SgAssignOp>
				<xsl:element name="SgVarRefExp">
					<xsl:attribute name="name">
					  <xsl:value-of select="SgAssignOp/SgVarRefExp/@name" />
					  <xsl:text>_tile</xsl:text>
					</xsl:attribute>
				</xsl:element>
				<xsl:copy-of select="SgAssignOp/*[2]" />
			</SgAssignOp>
			<xsl:copy-of select="./*[2]" />
			<xsl:element name="SgIntVal">
				<xsl:attribute name="value">
					  <xsl:value-of select="$size1" />
				</xsl:attribute>
				<xsl:attribute name="string">
					  <xsl:value-of select="$size1" />
				</xsl:attribute>
			</xsl:element>
			<SgBasicBlock>
				<SgFortranDo style="0" end="1" slabel="">
					<SgAssignOp>
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name"><xsl:value-of
								select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp/@name" /><xsl:text>_tile</xsl:text></xsl:attribute>
						</xsl:element>
						<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp/*[2]" />
					</SgAssignOp>
					<xsl:copy-of select="./SgBasicBlock/SgFortranDo/*[2]" />
					<xsl:element name="SgIntVal">
						<xsl:attribute name="value"><xsl:value-of
							select="$size2" /></xsl:attribute>
						<xsl:attribute name="string"><xsl:value-of
							select="$size2" /></xsl:attribute>
					</xsl:element>
					<SgBasicBlock>
						<SgFortranDo style="0" end="1" slabel="">
							<SgAssignOp>
								<xsl:copy-of select="SgAssignOp/SgVarRefExp" />
								<xsl:element name="SgVarRefExp">
									<xsl:attribute name="name">
									  <xsl:value-of select="SgAssignOp/SgVarRefExp/@name" />
									  	  <xsl:text>_tile</xsl:text>
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
												<xsl:attribute name="name"><xsl:value-of
													select="SgAssignOp/SgVarRefExp/@name" /><xsl:text>_tile</xsl:text></xsl:attribute>
											</xsl:element>
											<xsl:element name="SgIntVal">
												<xsl:attribute name="value"><xsl:value-of
													select="$size1" /></xsl:attribute>
												<xsl:attribute name="string"><xsl:value-of
													select="$size1" /></xsl:attribute>
											</xsl:element>
										</SgAddOp>
										<SgIntVal value="1" string="1" />
									</SgSubtractOp>
								</SgExprListExp>
							</SgFunctionCallExp>
							<SgNullExpression />
							<SgBasicBlock>
								<SgFortranDo style="0" end="1" slabel="">
									<SgAssignOp>
										<xsl:element name="SgVarRefExp">
											<xsl:attribute name="name"><xsl:value-of
												select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp/@name" /></xsl:attribute>
										</xsl:element>
										<xsl:element name="SgVarRefExp">
											<xsl:attribute name="name"><xsl:value-of
												select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp/@name" /><xsl:text>_tile</xsl:text></xsl:attribute>
										</xsl:element>
									</SgAssignOp>
									<SgFunctionCallExp>
										<SgFunctionRefExp name="min" kind="1" />
										<SgExprListExp>
											<xsl:copy-of select="./SgBasicBlock/SgFortranDo/*[2]" />
											<SgSubtractOp>
												<SgAddOp>
													<xsl:element name="SgVarRefExp">
														<xsl:attribute name="name"><xsl:value-of
															select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp/@name" /><xsl:text>_tile</xsl:text></xsl:attribute>
													</xsl:element>
													<xsl:element name="SgIntVal">
														<xsl:attribute name="value"><xsl:value-of
															select="$size2" /></xsl:attribute>
														<xsl:attribute name="string"><xsl:value-of
															select="$size2" /></xsl:attribute>
													</xsl:element>
												</SgAddOp>
												<SgIntVal value="1" string="1" />
											</SgSubtractOp>
										</SgExprListExp>
									</SgFunctionCallExp>
									<SgNullExpression />
									<xsl:apply-templates select="./SgBasicBlock/SgFortranDo/SgBasicBlock" />
								</SgFortranDo>
							</SgBasicBlock>
						</SgFortranDo>
					</SgBasicBlock>
				</SgFortranDo>
			</SgBasicBlock>
		</SgFortranDo>

	</xsl:template>

</xsl:stylesheet>
