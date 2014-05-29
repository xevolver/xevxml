<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:output method="xml" encoding="UTF-8" />

	<xsl:template match="/">
		<xsl:apply-templates />
	</xsl:template>

	<xsl:template match="*">
		<xsl:variable name="currentNode" select="self::node()" />
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates />
		</xsl:copy>
	</xsl:template>

	<xsl:template match="PreprocessingInfo">
	</xsl:template>

	<xsl:template match="SgBasicBlock">
		<xsl:choose>
			<xsl:when test="SgPragmaDeclaration[1]/SgPragma/DIRECTIVE/@name='nt_opt'">
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates select="SgFortranDo" mode="nt_opt">
						<xsl:with-param name="start" select="1" />
						<xsl:with-param name="end" select="'inum'" />
					</xsl:apply-templates>
					<xsl:copy-of select="SgReturnStmt" />
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

	<xsl:template name="if-filter">
		<xsl:param name="checkIndex"></xsl:param>
		<xsl:param name="arrayStart"></xsl:param>
		<xsl:param name="arrayEnd"></xsl:param>
		<xsl:param name="arrayIndex"></xsl:param>
		<xsl:element name="SgIfStmt">
			<xsl:element name="SgExprStatement">
				<xsl:element name="SgAndOp">
					<xsl:element name="SgGreaterOrEqualOp">
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name"><xsl:value-of
								select="$checkIndex" /></xsl:attribute>
						</xsl:element>
						<xsl:element name="SgFunctionCallExp">
							<xsl:element name="SgFunctionRefExp">
								<xsl:attribute name="symbol"><xsl:value-of
									select="$arrayStart" /></xsl:attribute>
							</xsl:element>
							<xsl:element name="SgExprListExp">
								<xsl:element name="SgVarRefExp">
									<xsl:attribute name="name"><xsl:value-of
										select="$arrayIndex" /></xsl:attribute>
								</xsl:element>
							</xsl:element>
						</xsl:element>
					</xsl:element>
					<xsl:element name="SgLessOrEqualOp">
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name"><xsl:value-of
								select="$checkIndex" /></xsl:attribute>
						</xsl:element>
						<xsl:element name="SgFunctionCallExp">
							<xsl:element name="SgFunctionRefExp">
								<xsl:attribute name="symbol"><xsl:value-of
									select="$arrayEnd" /></xsl:attribute>
							</xsl:element>
							<xsl:element name="SgExprListExp">
								<xsl:element name="SgVarRefExp">
									<xsl:attribute name="name"><xsl:value-of
										select="$arrayIndex" /></xsl:attribute>
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
	</xsl:template>






	<xsl:template match="SgFortranDo" mode="nt_opt">
		<xsl:param name="start" />
		<xsl:param name="end" />
		<xsl:variable name="ntOptNode" select="self::node()" />
		<xsl:copy> <!-- SgFortranDo -->
			<xsl:copy-of select="@*" />
			<xsl:element name="SgAssignOp">
				<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]" />
				<xsl:element name="SgIntVal">
					<xsl:attribute name="value"><xsl:value-of
						select="$start" /></xsl:attribute>
				</xsl:element>
			</xsl:element>
			<xsl:element name="SgVarRefExp">
				<xsl:attribute name="name"><xsl:value-of select="$end" /></xsl:attribute>
			</xsl:element>
			<xsl:copy-of select="SgNullExpression" />
			<xsl:element name="SgBasicBlock">
				<xsl:element name="SgFortranDo"> <!-- SgFortranDo -->
					<xsl:element name="SgAssignOp">
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name"><xsl:value-of
								select="SgAssignOp/SgVarRefExp[1]/@name" /></xsl:attribute>
						</xsl:element>
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name"><xsl:value-of
								select="SgAssignOp/SgVarRefExp[2]/@name" /></xsl:attribute>
						</xsl:element>
					</xsl:element>
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name"><xsl:value-of
							select="SgVarRefExp/@name" /></xsl:attribute>
					</xsl:element>
					<xsl:element name="SgNullExpression"></xsl:element>
					<xsl:element name="SgBasicBlock">
						<xsl:call-template name="if-filter">
							<xsl:with-param name="checkIndex"
								select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]/@name" />
							<xsl:with-param name="arrayStart"
								select="SgBasicBlock/SgExprStatement[1]/SgAssignOp/SgFunctionCallExp/SgFunctionRefExp/@symbol" />
							<xsl:with-param name="arrayEnd"
								select="SgBasicBlock/SgExprStatement[2]/SgAssignOp/SgFunctionCallExp/SgFunctionRefExp/@symbol" />
							<xsl:with-param name="arrayIndex"
								select="SgBasicBlock/SgExprStatement[1]/SgAssignOp/SgFunctionCallExp/SgExprListExp/SgVarRefExp/@name" />
						</xsl:call-template>
						<xsl:apply-templates select="SgBasicBlock/SgFortranDo/SgBasicBlock" />
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgFortranDo2">
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

</xsl:stylesheet>
