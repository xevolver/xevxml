<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevLoopUnroll">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />

		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $loopName">
				<xsl:apply-templates select="." mode="unroll_target_loop">
					<xsl:with-param name="factor" select="$factor" />
					<xsl:with-param name="loopName" select="$loopName" />
				</xsl:apply-templates>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="xevLoopUnroll">
						<xsl:with-param name="factor" select="$factor" />
						<xsl:with-param name="loopName" select="$loopName" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>

	</xsl:template>



	<xsl:template match="SgFortranDo" mode="unroll_target_loop">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:copy-of select="./*[1]" />
			<xsl:copy-of select="./*[2]" />
			<xsl:element name="SgIntVal">
				<xsl:attribute name="value">
							<xsl:value-of select="$factor" />
						</xsl:attribute>
			</xsl:element>

			<SgBasicBlock>
				<xsl:variable name="basic_block_content" select="./*" />

				<xsl:apply-templates select="$basic_block_content" />
				<xsl:for-each select="(//*)[position() &lt; $factor]">
					<xsl:apply-templates select="$basic_block_content"
						mode="loop_unroll_body">
						<xsl:with-param name="factor" select="$factor" />
						<xsl:with-param name="loopName" select="$loopName" />
						<xsl:with-param name="cnt" select="position()" />
					</xsl:apply-templates>

					<!-- </xsl:for-each> <xsl:apply-templates select="./SgBasicBlock" mode="xevLoopUnroll"> 
						<xsl:with-param name="factor" select="$factor" /> <xsl:with-param name="loopName" 
						select="$loopName" /> </xsl:apply-templates> -->
				</xsl:for-each>
			</SgBasicBlock>

		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="old_unroll_target_loop">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:copy-of select="./*[1]" />
			<xsl:copy-of select="./*[2]" />
			<xsl:element name="SgIntVal">
				<xsl:attribute name="value">
							<xsl:value-of select="$factor" />
						</xsl:attribute>
			</xsl:element>

			<xsl:apply-templates select="./SgBasicBlock"
				mode="xevLoopUnroll">
				<xsl:with-param name="factor" select="$factor" />
				<xsl:with-param name="loopName" select="$loopName" />
			</xsl:apply-templates>

		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgExprStatement[last()]" mode="old_xevLoopUnroll">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />

		<!-- self -->
		<xsl:apply-templates select="." />

		<!-- target statement -->
		<!-- <xsl:param name="copy_stm" select="../SgExprStatement" /> OK for libxml -->
		<xsl:variable name="copy_stm" select="../*" />
		<!-- <xsl:variable name="copy_stm" select="." /> -->

		<!-- <xsl:apply-templates select="$copy_stm" /> -->

		<!-- target statement -->
		<xsl:for-each select="(//*)[position() &lt; $factor]">
			<xsl:apply-templates select="$copy_stm" mode="loop_unroll_body">
				<xsl:with-param name="factor" select="$factor" />
				<xsl:with-param name="loopName" select="$loopName" />
				<xsl:with-param name="cnt" select="position()" />
			</xsl:apply-templates>
		</xsl:for-each>

	</xsl:template>


	<xsl:template match="SgVarRefExp" mode="loop_unroll_body">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />
		<xsl:param name="cnt" />

		<xsl:if test="./@name=$loopName">
			<SgAddOp>
				<xsl:copy-of select="." />
				<SgIntVal>
					<xsl:attribute name="value">
						<xsl:value-of select="$cnt" />
					</xsl:attribute>
				</SgIntVal>
			</SgAddOp>
		</xsl:if>

		<xsl:if test="./@name!=$loopName">
			<xsl:copy-of select="." />
		</xsl:if>
	</xsl:template>

	<xsl:template match="*" mode="loop_unroll_body">
		<xsl:param name="factor" />
		<xsl:param name="loopName" />
		<xsl:param name="cnt" />
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop_unroll_body">
				<xsl:with-param name="factor" select="$factor" />
				<xsl:with-param name="loopName" select="$loopName" />
				<xsl:with-param name="cnt" select="$cnt" />
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>

	<!-- epilog -->
	<xsl:template match="*" mode="loop_unroll_Epilog">
		<xsl:copy>
			<xsl:copy-of select="@*" />

			<SgAssignOp>
				<xsl:copy-of select="./*[1]/SgVarRefExp" />
				<SgSubtractOp>
					<xsl:copy-of select="./*[2]" />
					<SgFunctionCallExp>
						<SgFunctionRefExp symbol="mod" />
						<SgExprListExp>
							<xsl:copy-of select="./*[2]" />
							<xsl:element name="SgIntVal">
								<xsl:attribute name="value">
											<xsl:value-of
									select="preceding-sibling::*[1]/SgPragma/DIRECTIVE[@name='loop_unroll']/CLAUSE[@name='step']/ARG[1]/@value" />
										</xsl:attribute>
							</xsl:element>
						</SgExprListExp>
					</SgFunctionCallExp>
				</SgSubtractOp>
			</SgAssignOp>
			<xsl:copy-of select="./*[2]" />
			<xsl:copy-of select="./*[3]" />

			<xsl:apply-templates select="./SgBasicBlock"
				mode="loop_unroll_Epilog" />

			<xsl:apply-templates mode="loop_unroll_Epilog" />
		</xsl:copy>
	</xsl:template>
	<!--+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->




</xsl:stylesheet>
