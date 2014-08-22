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
			<xsl:when
				test="self::node()/SgPragmaDeclaration/SgPragma/DIRECTIVE/@name  = 'nt_opt200'">
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates select="./SgFortranDo"
						mode="nt_opt1">
						<xsl:with-param name="label" select='200' />
						<xsl:with-param name="depth" select='1' />
					</xsl:apply-templates>
					<xsl:copy-of select="SgReturnStmt" />
				</xsl:copy>
			</xsl:when>
			<xsl:when
				test="self::node()/SgPragmaDeclaration/SgPragma/DIRECTIVE/@name  = 'nt_opt2'">
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates select="./SgFortranDo/SgBasicBlock/SgFortranDo"
						mode="nt_opt2">
						<xsl:with-param name="label" select='300' />
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

	<xsl:template match="PreprocessingInfo" mode="nt_opt1">
	</xsl:template>

	<xsl:template match="*" mode="nt_opt1">
		<xsl:param name="nlabel" />
		<xsl:param name="depth" />
		<xsl:variable name="currentNode" select="self::node()" />

		<xsl:choose>
			<xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp[@name='L']">
				<xsl:apply-templates select="SgBasicBlock/SgFortranDo" mode="nt_opt1">
					<xsl:with-param name="nlabel" select="$nlabel" />
					<xsl:with-param name="depth" select="$depth" />
				</xsl:apply-templates>
			</xsl:when>
			<xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp[@name='I']">
						<xsl:text>!$acc loop vector</xsl:text>
				DO	<xsl:value-of select="$nlabel" />	I=1,inum
				!$acc loop seq
				DO L=lstart,lend
				IF (I.ge.IS(L) .and.
				I.le.IT(L)) EXIT
				END DO
				<xsl:apply-templates select="SgBasicBlock" />
			</xsl:when>
			<xsl:when test="self::SgFortranDo">
				<xsl:choose>
					<xsl:when test="$depth = 1">
						<xsl:text>!$acc loop private(L)</xsl:text>
					</xsl:when>
					<xsl:when test="$depth = 2">
						<xsl:text>!$acc loop gang</xsl:text>
					</xsl:when>
					<xsl:when test="$depth = 3">
						<xsl:text>!$acc loop gang,vector</xsl:text>
					</xsl:when>
					<xsl:when test="$depth = 4">
						<xsl:text>!$acc loop vector</xsl:text>
					</xsl:when>
					<xsl:otherwise>
						<xsl:comment>
							<xsl:value-of select="$depth" />
						</xsl:comment>
					</xsl:otherwise>
				</xsl:choose>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="nt_opt1">
						<xsl:with-param name="nlabel" select="$nlabel" />
						<xsl:with-param name="depth" select="$depth + 1" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:when>

			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="nt_opt1">
						<xsl:with-param name="nlabel" select="$nlabel" />
						<xsl:with-param name="depth" select="$depth" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="PreprocessingInfo" mode="nt_opt2">
	</xsl:template>

	<xsl:template match="*" mode="nt_opt2">
		<xsl:choose>
			<xsl:when test="self::SgFortranDo/SgAssignOp/SgVarRefExp[@name='I']">
				DO
				<xsl:value-of select="self::SgFortranDo/@nlabel" />
				I=1,inum
				!$acc loop seq
				DO L=lstart,lend
				IF (I.ge.IS(L) .and.
				I.le.IT(L)) EXIT
				END DO
				<xsl:apply-templates select="SgBasicBlock" />
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="nt_opt2" />
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="SgFortranDo_20140531_00" mode="nt_opt">
		<xsl:param name="to" />
		<xsl:param name="start" />
		<xsl:param name="end" />
		<xsl:copy>
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" /> <!-- SgFortranDo -->
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
				<xsl:element name="SgFortranDo">
					<xsl:copy-of select="@*" /> <!-- SgFortranDo -->
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
						<xsl:apply-templates
							select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgExprStatement" />
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:copy>
	</xsl:template>


	<xsl:template name="if-filter-20140530-20">
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
						<xsl:element name="SgPntrArrRefExp">
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name"><xsl:value-of
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
						<xsl:element name="SgPntrArrRefExp">
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name"><xsl:value-of
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
						<xsl:element name="SgPntrArrRefExp">
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name"><xsl:value-of
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
						<xsl:element name="SgPntrArrRefExp">
							<xsl:element name="SgVarRefExp">
								<xsl:attribute name="name"><xsl:value-of
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





	<!-- <xsl:template match="SgFortranDo_20140530_23" mode="nt_opt"> <xsl:param 
		name="skipNum" /> <xsl:variable name="loopNum" select="./@nlabel" /> DO <xsl:value-of 
		select="$loopNum" /> I=1,inum <xsl:apply-template name="copy-mid-fortrando"> 
		<xsl:with-param name="skipNum" value="$skipNum" /> <xsl:with-param name="lLoop"> 
		<xsl:variable name="leaf"> !$acc loop seq DO L=lstart,lend IF (I.ge.IS(L) 
		.and. I.le.IT(L)) EXIT END DO <xsl:apply-templates select="SgBasicBlock/SgFortranDo/SgBasicBlock" 
		/> </xsl:variable> </xsl:with-param> </xsl:apply-template> END DO </xsl:template> 
		<xsl:template name="copy-mid-fortrando"> </xsl:template> -->

	<xsl:template match="SgFortranDo_20140530_2113" mode="nt_opt">
		DO 200 I=1,inum
		!$acc loop seq
		DO L=lstart,lend
		IF (I.ge.IS(L) .and.
		I.le.IT(L)) EXIT
		END DO
		<xsl:apply-templates select="SgBasicBlock/SgFortranDo/SgBasicBlock" />
		END DO
	</xsl:template>


	<xsl:template match="SgFortranDo_20140530_20" mode="nt_opt">
		<xsl:param name="start" />
		<xsl:param name="end" />
		<xsl:variable name="checkIndex"
			select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]/@name" />
		<xsl:variable name="arrayStart"
			select=".//SgPntrArrRefExp[1]/SgVarRefExp/@name" />
		<xsl:variable name="arrayIndex"
			select=".//SgPntrArrRefExp[1]/SgExprListExp/SgVarRefExp/@name" />
		<xsl:variable name="arrayEnd"
			select=".//SgPntrArrRefExp[2]/SgVarRefExp/@name" />
		<xsl:copy>
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" /> <!-- SgFortranDo -->
			<xsl:element name="SgAssignOp">
				<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]" />
				<xsl:element name="SgIntVal">
					<xsl:attribute name="value">
          <xsl:value-of select="$start" /></xsl:attribute>
				</xsl:element>
			</xsl:element>
			<xsl:element name="SgVarRefExp">
				<xsl:attribute name="name">
        <xsl:value-of select="$end" /></xsl:attribute>
			</xsl:element>
			<xsl:copy-of select="SgNullExpression" />
			<xsl:element name="SgBasicBlock">
				<xsl:element name="SgFortranDo">
					<xsl:copy-of select="@*" /> <!-- SgFortranDo -->
					<xsl:element name="SgAssignOp">
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
              <xsl:value-of select="SgAssignOp/SgVarRefExp[1]/@name" />
                </xsl:attribute>
						</xsl:element>
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
              <xsl:value-of select="SgAssignOp/SgVarRefExp[2]/@name" />
                </xsl:attribute>
						</xsl:element>
					</xsl:element>
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name">
            <xsl:value-of select="SgVarRefExp/@name" /></xsl:attribute>
					</xsl:element>
					<xsl:element name="SgNullExpression"></xsl:element>
					<xsl:element name="SgBasicBlock">
						<xsl:call-template name="if-filter">
							<xsl:with-param name="checkIndex" select="$checkIndex" />
							<xsl:with-param name="arrayStart" select="$arrayStart" />
							<xsl:with-param name="arrayEnd" select="$arrayEnd" />
							<xsl:with-param name="arrayIndex" select="$arrayIndex" />
						</xsl:call-template>
						<xsl:apply-templates
							select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgExprStatement" />
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:copy>
	</xsl:template>


	<xsl:template match="SgFortranDo_2014053015" mode="nt_opt">
		<xsl:param name="start" />
		<xsl:param name="end" />
		<xsl:variable name="checkIndex"
			select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]/@name" />
		<xsl:variable name="arrayStart"
			select="SgBasicBlock/SgExprStatement[1]/SgAssignOp/SgPntrArrRefExp/SgVarRefExp/@name" />
		<xsl:variable name="arrayEnd"
			select="SgBasicBlock/SgExprStatement[2]/SgAssignOp/SgPntrArrRefExp/SgVarRefExp/@name" />
		<xsl:variable name="arrayIndex"
			select="SgBasicBlock/SgExprStatement[1]/SgAssignOp/SgPntrArrRefExp/SgExprListExp/SgVarRefExp/@name" />
		<xsl:copy>
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" /> <!-- SgFortranDo -->
			<xsl:element name="SgAssignOp">
				<xsl:copy-of select="SgBasicBlock/SgFortranDo/SgAssignOp/SgVarRefExp[1]" />
				<xsl:element name="SgIntVal">
					<xsl:attribute name="value">
					<xsl:value-of select="$start" /></xsl:attribute>
				</xsl:element>
			</xsl:element>
			<xsl:element name="SgVarRefExp">
				<xsl:attribute name="name">
				<xsl:value-of select="$end" /></xsl:attribute>
			</xsl:element>
			<xsl:copy-of select="SgNullExpression" />
			<xsl:element name="SgBasicBlock">
				<xsl:element name="SgFortranDo">
					<xsl:copy-of select="@*" /> <!-- SgFortranDo -->
					<xsl:element name="SgAssignOp">
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
							<xsl:value-of select="SgAssignOp/SgVarRefExp[1]/@name" />
								</xsl:attribute>
						</xsl:element>
						<xsl:element name="SgVarRefExp">
							<xsl:attribute name="name">
							<xsl:value-of select="SgAssignOp/SgVarRefExp[2]/@name" />
								</xsl:attribute>
						</xsl:element>
					</xsl:element>
					<xsl:element name="SgVarRefExp">
						<xsl:attribute name="name">
						<xsl:value-of select="SgVarRefExp/@name" /></xsl:attribute>
					</xsl:element>
					<xsl:element name="SgNullExpression"></xsl:element>
					<xsl:element name="SgBasicBlock">
						<xsl:call-template name="if-filter">
							<xsl:with-param name="checkIndex" select="$checkIndex" />
							<xsl:with-param name="arrayStart" select="$arrayStart" />
							<xsl:with-param name="arrayEnd" select="$arrayEnd" />
							<xsl:with-param name="arrayIndex" select="$arrayIndex" />
						</xsl:call-template>
						<xsl:apply-templates
							select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgExprStatement" />
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:copy>
	</xsl:template>



	<xsl:template match="SgFortranDo_20140529_1549" mode="nt_opt">
		<xsl:param name="start" />
		<xsl:param name="end" />
		<xsl:copy>
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" /> <!-- SgFortranDo -->
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
				<xsl:element name="SgFortranDo">
					<xsl:copy-of select="@*" /> <!-- SgFortranDo -->
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
						<xsl:apply-templates
							select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgExprStatement" />
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgFortranDo_20140529" mode="nt_opt">
		<xsl:param name="start" />
		<xsl:param name="end" />
		<xsl:copy>
			<xsl:copy-of select="SgBasicBlock/SgFortranDo/@*" /> <!-- SgFortranDo -->
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
				<xsl:element name="SgFortranDo">
					<xsl:copy-of select="@*" /> <!-- SgFortranDo -->
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
						<xsl:apply-templates
							select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgExprStatement" />
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgFortranDo_bkup2" mode="nt_opt">
		<xsl:param name="start" />
		<xsl:param name="end" />
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
						<xsl:apply-templates
							select="SgBasicBlock/SgFortranDo/SgBasicBlock/SgExprStatement" />
					</xsl:element>
				</xsl:element>
			</xsl:element>
		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgFortranDo_bkup1">
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
