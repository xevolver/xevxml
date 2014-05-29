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

	<xsl:template match="SgFortranDo">
		<xsl:choose>
			<xsl:when test="contains(string(child::PreprocessingInfo), 'remove')">
				<xsl:comment>
					<xsl:value-of select="child::PreprocessingInfo" />
				</xsl:comment>
				<xsl:apply-templates select="SgBasicBlock" />

			</xsl:when>
			<xsl:when test="contains(string(child::PreprocessingInfo), 'flatten')">
				<xsl:comment>
					flatten
				</xsl:comment>
				<xsl:copy> <!-- SgFortranDo -->
					<xsl:copy-of select="@*" />
					<xsl:element name="SgAssignOp">
						<xsl:copy-of select="SgAssignOp/@*" />
						<xsl:copy-of select="SgAssignOp/SgVarRefExp[1]" />
						<xsl:element name="SgIntVal">
							<xsl:copy-of select="SgVarRefExp/@address" />
							<xsl:attribute name="value">1</xsl:attribute>
						</xsl:element>
					</xsl:element>
					<xsl:comment>
						IntVal
					</xsl:comment>
					<xsl:element name="SgVarRefExp">
						<xsl:copy-of select="SgAssignOp/SgVarRefExp/@address" />
						<xsl:attribute name="name">inum</xsl:attribute>
					</xsl:element>

					<xsl:copy-of select="SgNullExpression" />

					<xsl:element name="SgBasicBlock">
						<xsl:element name="SgFortranDo">
							<xsl:element name="SgAssignOp">
								<xsl:element name="SgVarRefExp">
									<xsl:attribute name="name">L</xsl:attribute>
								</xsl:element>
								<!-- <SgIntVal address="0x9cf3ca4" value="1" /> -->
								<xsl:element name="SgVarRefExp">
									<xsl:attribute name="name">lstart</xsl:attribute>
								</xsl:element>
							</xsl:element>
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name">lend</xsl:attribute>
							</xsl:element>
							<xsl:element name="SgNullExpression"></xsl:element>
							<xsl:element name="SgBasicBlock">
								<xsl:element name="SgIfStmt">
									<xsl:element name="SgExprStatement">
										<xsl:element name="SgAndOp">
											<xsl:element name="SgGreaterOrEqualOp">
												<xsl:element name="SgVarRefExp">
													<xsl:attribute name="name">I</xsl:attribute>
												</xsl:element>
												<xsl:element name="SgFunctionCallExp">
													<xsl:element name="SgFunctionRefExp">
														<xsl:attribute name="symbol">IS</xsl:attribute>
													</xsl:element>
													<xsl:element name="SgExprListExp">
														<xsl:element name="SgVarRefExp">
															<xsl:attribute name="name">L</xsl:attribute>
														</xsl:element>
													</xsl:element>
												</xsl:element>
											</xsl:element>
											<xsl:element name="SgLessOrEqualOp">
												<xsl:element name="SgVarRefExp">
													<xsl:attribute name="name">I</xsl:attribute>
												</xsl:element>
												<xsl:element name="SgFunctionCallExp">
													<xsl:element name="SgFunctionRefExp">
														<xsl:attribute name="symbol">IT</xsl:attribute>
													</xsl:element>
													<xsl:element name="SgExprListExp">
														<xsl:element name="SgVarRefExp">
															<xsl:attribute name="name">L</xsl:attribute>
														</xsl:element>
													</xsl:element>
												</xsl:element>
											</xsl:element>
										</xsl:element>
									</xsl:element>
									<xsl:element name="SgBasicBlock">
										<xsl:element name="SgBreakStmt">
										</xsl:element>
									</xsl:element>
								</xsl:element>

								<xsl:apply-templates select="SgBasicBlock/SgExprStatement" />
							</xsl:element>
						</xsl:element>
					</xsl:element>
				</xsl:copy>

			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates />
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>

	<xsl:template name="flatten-assign">
		<xsl:param name="var">
			defaultVar
		</xsl:param>
		<xsl:comment>
			flatten-assigne
			<xsl:value-of select="$var" />
		</xsl:comment>
		<xsl:copy></xsl:copy>
	</xsl:template>

	<xsl:template name="flatten-intval">
		<xsl:param name="var">
			defaultVar
		</xsl:param>
		<xsl:comment>
			flatten-intval start
			<xsl:value-of select="$var" />
		</xsl:comment>
		<xsl:copy>
			<xsl:copy-of select="@address" />
			<xsl:attribute name="name">
			<xsl:value-of select="$var" />
</xsl:attribute>
		</xsl:copy>
		<xsl:comment>
			flatten-intval end
			<xsl:value-of select="$var" />
		</xsl:comment>
	</xsl:template>

</xsl:stylesheet>
