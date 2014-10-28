<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
	<xsl:import href="./libXev.xsl" />

	<xsl:template match="SgFortranDo" mode="chill_unroll_jam">
		<xsl:param name="max" />
		<xsl:param name="var" />
		<xsl:comment>
			libCHiLL.xsl chill_unroll_jam
		</xsl:comment>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<!-- 変数 初期値 -->
			<xsl:copy-of select="./*[1]" />
			<!-- 最終値 -->
			<xsl:copy-of select="./*[2]" />
			<!-- 刻み幅 -->
			<xsl:element name="SgIntVal">
				<xsl:attribute name="value">
							<xsl:value-of select="$max" />
						</xsl:attribute>
			</xsl:element>

			<xsl:apply-templates select="./SgBasicBlock"
				mode="loop_unroll">
				<xsl:with-param name="max" select="$max" />
				<xsl:with-param name="var" select="$var" />
			</xsl:apply-templates>

		</xsl:copy>
	</xsl:template>

	<xsl:template match="SgFortranDo" mode="chill_unroll">
		<xsl:param name="max" />
		<xsl:param name="var" />
		<xsl:comment>
			libCHiLL.xsl chill_unroll
		</xsl:comment>
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<!-- 変数 初期値 -->
			<xsl:copy-of select="./*[1]" />
			<!-- 最終値 -->
			<xsl:copy-of select="./*[2]" />
			<!-- 刻み幅 -->
			<xsl:element name="SgIntVal">
				<xsl:attribute name="value">
							<xsl:value-of select="$max" />
						</xsl:attribute>
			</xsl:element>

			<xsl:apply-templates select="./SgBasicBlock"
				mode="loop_unroll">
				<xsl:with-param name="max" select="$max" />
				<xsl:with-param name="var" select="$var" />
			</xsl:apply-templates>

		</xsl:copy>
	</xsl:template>


	<xsl:template match="SgExprStatement[last()]" mode="loop_unroll">
		<xsl:param name="max" />		<!-- STEP数 -->
		<xsl:param name="var" />		<!-- 置き換える変数 -->

		<!-- 自分を出力する -->
		<xsl:apply-templates select="." />

		<!-- コピーする【SgExprStatement】行を設定 -->
		<xsl:param name="copy-stm" select="../SgExprStatement" />

		<!-- コピーする【SgExprStatement】行を設定 -->
		<xsl:for-each select="(//*)[position() &lt; $max]">
			<xsl:apply-templates select="$copy-stm" mode="loop_unroll_1">
				<xsl:with-param name="max" select="$max" />
				<xsl:with-param name="var" select="$var" />
				<xsl:with-param name="cnt" select="position()" />
			</xsl:apply-templates>
		</xsl:for-each>

	</xsl:template>
	<xsl:template match="*" mode="loop_unroll">
		<xsl:param name="max" />
		<xsl:param name="var" />
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop_unroll">
				<xsl:with-param name="max" select="$max" />
				<xsl:with-param name="var" select="$var" />
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>

	<!-- remove PreprocessingInfo -->
	<xsl:template match="PreprocessingInfo" mode="loop_unroll">
	</xsl:template>
	<!-- <xsl:template match="SgPragmaDeclaration[1]" mode="loop_unroll"> </xsl:template> -->


	<xsl:template match="SgVarRefExp" mode="loop_unroll_1">
		<xsl:param name="max" />
		<xsl:param name="var" />
		<xsl:param name="cnt" />

		<xsl:if test="./@name=$var">
			<SgAddOp>
				<xsl:copy-of select="." />
				<SgIntVal>
					<xsl:attribute name="value">
						<xsl:value-of select="$cnt" />
					</xsl:attribute>
				</SgIntVal>
			</SgAddOp>
		</xsl:if>

		<xsl:if test="./@name!=$var">
			<xsl:copy-of select="." />
		</xsl:if>
	</xsl:template>

	<xsl:template match="*" mode="loop_unroll_1">
		<xsl:param name="max" />
		<xsl:param name="var" />
		<xsl:param name="cnt" />
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop_unroll_1">
				<xsl:with-param name="max" select="$max" />
				<xsl:with-param name="var" select="$var" />
				<xsl:with-param name="cnt" select="$cnt" />
			</xsl:apply-templates>
		</xsl:copy>
	</xsl:template>


	<!-- SgPragmaDeclaration 削除 -->
	<xsl:template match="SgPragmaDeclaration" mode="loop_unroll_Epilog">
	</xsl:template>
	<!-- PreprocessingInfo 削除 -->
	<xsl:template match="PreprocessingInfo" mode="loop_unroll_Epilog">
	</xsl:template>

	<!-- その他は全て出力する -->
	<xsl:template match="*" mode="loop_unroll_Epilog">
		<xsl:copy>
			<xsl:copy-of select="@*" />
			<xsl:apply-templates mode="loop_unroll_Epilog" />
		</xsl:copy>
	</xsl:template>
	<!--+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ -->

</xsl:stylesheet>