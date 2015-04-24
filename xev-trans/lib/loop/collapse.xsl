<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

	<xsl:template match="*" mode="xevLoopCollapse">
		<xsl:param name="firstLoop" />
		<xsl:param name="secondLoop" />
		<xsl:apply-templates select="."
			mode="loop_collapse_find_first">
			<xsl:with-param name="firstLoop" select="$firstLoop" />
			<xsl:with-param name="secondLoop" select="$secondLoop" />
		</xsl:apply-templates>
	</xsl:template>

	<xsl:template match="*" mode="loop_collapse_find_first">
		<xsl:param name="firstLoop" />
		<xsl:param name="secondLoop" />
		<xsl:variable name="currentNode" select="." /> <!-- for debug -->
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $firstLoop">
				<xsl:comment>
					test-loop_collapse.xsl found firstLoop
				</xsl:comment>

				<!-- get loop end from first loop -->
				<xsl:variable name="end" select="./*[2]" />
				<xsl:apply-templates select="self::SgFortranDo/SgBasicBlock/SgFortranDo"
					mode="loop_collapse_find_second">
					<xsl:with-param name="firstEnd" select="$end" />
					<xsl:with-param name="firstLoop" select="$firstLoop" />
					<xsl:with-param name="secondLoop" select="$secondLoop" />
				</xsl:apply-templates>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="loop_collapse_find_first">
						<xsl:with-param name="firstLoop" select="$firstLoop" />
						<xsl:with-param name="secondLoop" select="$secondLoop" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="*" mode="loop_collapse_find_second">
		<xsl:param name="firstEnd" />
		<xsl:param name="firstLoop" />
		<xsl:param name="secondLoop" />
		<xsl:variable name="currentNode" select="." /> <!-- for debug -->
		<xsl:choose>
			<xsl:when
				test="self::SgFortranDo/SgAssignOp/SgVarRefExp/@name = $secondLoop">
				<!--change end -->
				<xsl:comment>
					test-loop_collapse.xsl found secondLoop
				</xsl:comment>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:copy-of select="./*[1]" />
					<SgMultiplyOp>
						<xsl:copy-of select="./*[2]" />
						<xsl:comment>
							test-loop_collapse.xsl multiply
						</xsl:comment>
						<xsl:apply-templates select="$firstEnd" />
					</SgMultiplyOp>
					<!-- stride -->
					<xsl:copy-of select="./*[3]" />

					<xsl:apply-templates select="./SgBasicBlock"
						mode="loop_collapse_body">
						<xsl:with-param name="firstLoop" select="$firstLoop" />
					</xsl:apply-templates>

				</xsl:copy>

			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="loop_collapse_find_second">
						<xsl:with-param name="firstEnd" select="$firstEnd" />
						<xsl:with-param name="firstLoop" select="$firstLoop" />
						<xsl:with-param name="secondLoop" select="$secondLoop" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="*" mode="loop_collapse_body">
		<xsl:param name="firstLoop" />

		<xsl:choose>
			<xsl:when test="self::SgVarRefExp/@name = $firstLoop">
				<SgIntVal value="1" />
				<xsl:apply-templates mode="loop_collapse_body">
					<xsl:with-param name="firstLoop" select="$firstLoop" />
				</xsl:apply-templates>
			</xsl:when>
			<xsl:otherwise>
				<xsl:copy>
					<xsl:copy-of select="@*" />
					<xsl:apply-templates mode="loop_collapse_body">
						<xsl:with-param name="firstLoop" select="$firstLoop" />
					</xsl:apply-templates>
				</xsl:copy>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<!-- <xsl:template match="SgPragmaDeclaration" mode="loop_collapse"> </xsl:template> 
		<xsl:template match="PreprocessingInfo" mode="loop_collapse"> </xsl:template> -->
</xsl:stylesheet>